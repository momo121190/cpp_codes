#include "server.hpp"
#include <iostream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wepoll")

#define BUFFER_SIZE 1024

EpollServer::EpollServer(const std::string& port, int worker_threads)
    : port_(port), worker_count_(worker_threads), running_(false),
      listen_sock_(INVALID_SOCKET), epoll_fd_(NULL) {}

EpollServer::~EpollServer() {
    stop();
}

void EpollServer::start() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    // Create listening socket
    listen_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock_ == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    // Set socket to non-blocking mode
    u_long mode = 1;
    if (ioctlsocket(listen_sock_, FIONBIO, &mode) == SOCKET_ERROR) {
        closesocket(listen_sock_);
        WSACleanup();
        throw std::runtime_error("Failed to set non-blocking mode");
    }

    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(port_.c_str()));

    if (bind(listen_sock_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(listen_sock_);
        WSACleanup();
        throw std::runtime_error("Bind failed");
    }

    if (listen(listen_sock_, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(listen_sock_);
        WSACleanup();
        throw std::runtime_error("Listen failed");
    }

    // Create epoll instance
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == NULL) {
        closesocket(listen_sock_);
        WSACleanup();
        throw std::runtime_error("Failed to create epoll instance");
    }

    // Add listening socket to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = (void*)(intptr_t)listen_sock_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_sock_, &ev) == -1) {
        epoll_close(epoll_fd_);
        closesocket(listen_sock_);
        WSACleanup();
        throw std::runtime_error("Failed to add socket to epoll");
    }

    running_ = true;
    
    // Start worker threads
    for (int i = 0; i < worker_count_; ++i) {
        workers_.emplace_back(&EpollServer::workerThread, this, i);
    }

    // Start listener thread
    listener_ = std::thread(&EpollServer::listenerThread, this);

    std::cout << "Server started on port " << port_ << std::endl;
}

void EpollServer::stop() {
    if (!running_) return;
    
    running_ = false;
    
    // Notify all threads to wake up
    queue_cv_.notify_all();
    
    // Join all worker threads
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    // Join listener thread
    if (listener_.joinable()) {
        listener_.join();
    }
    
    // Close epoll
    if (epoll_fd_) {
        epoll_close(epoll_fd_);
        epoll_fd_ = NULL;
    }
    
    // Close listening socket
    if (listen_sock_ != INVALID_SOCKET) {
        closesocket(listen_sock_);
        listen_sock_ = INVALID_SOCKET;
    }
    
    WSACleanup();
}

void EpollServer::listenerThread() {
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    
    while (running_) {
        int num_events = epoll_wait(epoll_fd_, events, MAX_EVENTS, 100); // 100ms timeout
        if (num_events == -1) {
            if (running_) {
                std::cerr << "epoll_wait error" << std::endl;
            }
            continue;
        }

        for (int i = 0; i < num_events && running_; ++i) {
            SOCKET current_sock = (SOCKET)(intptr_t)events[i].data.ptr;

            // Handle new connections
            if (current_sock == listen_sock_) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                SOCKET client_sock = accept(listen_sock_, 
                                          (struct sockaddr*)&client_addr, 
                                          &client_len);
                if (client_sock == INVALID_SOCKET) {
                    std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
                    continue;
                }

                // Set client socket to non-blocking
                u_long mode = 1;
                ioctlsocket(client_sock, FIONBIO, &mode);

                // Add client to epoll
                struct epoll_event client_ev;
                client_ev.events = EPOLLIN | EPOLLRDHUP;
                client_ev.data.ptr = (void*)(intptr_t)client_sock;
                if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_sock, &client_ev) == -1) {
                    std::cerr << "Failed to add client socket" << std::endl;
                    closesocket(client_sock);
                    continue;
                }

                std::cout << "New connection from " 
                          << inet_ntoa(client_addr.sin_addr) << ":"
                          << ntohs(client_addr.sin_port) << std::endl;
            }
            // Handle client events
            else {
                {
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    client_queue_.push(current_sock);
                }
                queue_cv_.notify_one();
            }
        }
    }
}

void EpollServer::workerThread(int id) {
    char buffer[BUFFER_SIZE];
    
    while (running_) {
        SOCKET client_sock = INVALID_SOCKET;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this]() { 
                return !client_queue_.empty() || !running_; 
            });
            
            if (!running_ && client_queue_.empty()) {
                return;
            }
            
            if (!client_queue_.empty()) {
                client_sock = client_queue_.front();
                client_queue_.pop();
            }
        }
        
        if (client_sock != INVALID_SOCKET) {
            handleClient(client_sock);
        }
    }
}

void EpollServer::handleClient(SOCKET client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
    
    if (bytes_received <= 0) {
        if (bytes_received == SOCKET_ERROR) {
            std::cerr << "recv error: " << WSAGetLastError() << std::endl;
        }
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_sock, NULL);
        closesocket(client_sock);
        return;
    }
    
    // Echo back to client
    buffer[bytes_received] = '\0';
    std::cout << "Received: " << buffer << std::endl;
    send(client_sock, buffer, bytes_received, 0);
    
    // Return socket to epoll monitoring
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.ptr = (void*)(intptr_t)client_sock;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_sock, &ev) == -1) {
        std::cerr << "Failed to modify client socket in epoll" << std::endl;
        closesocket(client_sock);
    }
}
