#include <wepoll.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "9000"
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

// Error handling macro
#define SOCKET_CHECK(x) if ((x) == SOCKET_ERROR) { \
    printf("Error at %s:%d - %d\n", __FILE__, __LINE__, WSAGetLastError()); \
    exit(1); }

int main() {
    // 1. Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    // 2. Create listening socket
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKET_CHECK(listen_sock == INVALID_SOCKET);

    // Set socket to non-blocking mode
    u_long mode = 1;
    ioctlsocket(listen_sock, FIONBIO, &mode);

    // 3. Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(PORT));

    SOCKET_CHECK(bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)));
    SOCKET_CHECK(listen(listen_sock, SOMAXCONN));

    printf("Server listening on port %s\n", PORT);

    // 4. Create epoll instance (CORRECTED TYPE)
    HANDLE epoll_fd = epoll_create1(0);
    if (epoll_fd == NULL) {
        printf("Failed to create epoll instance\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    // 5. Add listening socket to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;  // Monitor for incoming connections
    ev.data.ptr = (void*)(intptr_t)listen_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        printf("Failed to add socket to epoll\n");
        epoll_close(epoll_fd);
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    // 6. Event loop
    struct epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    while (1) {
        // Wait for events (indefinitely)
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            printf("epoll_wait error\n");
            break;
        }

        for (int i = 0; i < num_events; ++i) {
            SOCKET current_sock = (SOCKET)(intptr_t)events[i].data.ptr;

            // 7. Handle new connections
            if (current_sock == listen_sock) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                SOCKET client_sock = accept(listen_sock, 
                                          (struct sockaddr*)&client_addr, 
                                          &client_len);
                if (client_sock == INVALID_SOCKET) {
                    printf("accept failed: %d\n", WSAGetLastError());
                    continue;
                }

                // Set client socket to non-blocking
                ioctlsocket(client_sock, FIONBIO, &mode);

                // Add client to epoll
                struct epoll_event client_ev;
                client_ev.events = EPOLLIN | EPOLLRDHUP;
                client_ev.data.ptr = (void*)(intptr_t)client_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &client_ev) == -1) {
                    printf("Failed to add client socket\n");
                    closesocket(client_sock);
                    continue;
                }

                printf("New connection from %s:%d\n", 
                      inet_ntoa(client_addr.sin_addr), 
                      ntohs(client_addr.sin_port));
            }
            // 8. Handle client data
            else {
                // Check for disconnect
                if (events[i].events & EPOLLRDHUP) {
                    printf("Client disconnected\n");
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_sock, NULL);
                    closesocket(current_sock);
                    continue;
                }

                // Handle incoming data
                if (events[i].events & EPOLLIN) {
                    int bytes_received = recv(current_sock, buffer, BUFFER_SIZE, 0);
                    if (bytes_received <= 0) {
                        printf("Connection closed or error\n");
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_sock, NULL);
                        closesocket(current_sock);
                    } else {
                        // Echo back to client
                        buffer[bytes_received] = '\0';
                        printf("Received: %s\n", buffer);
                        send(current_sock, buffer, bytes_received, 0);
                    }
                }
            }
        }
    }

    // 9. Cleanup
    epoll_close(epoll_fd);
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
