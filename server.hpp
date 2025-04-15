#ifndef SERVER_HPP
#define SERVER_HPP

#include <wepoll.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

class EpollServer {
public:
    EpollServer(const std::string& port, int worker_threads = 5);
    ~EpollServer();

    void start();
    void stop();

private:
    void listenerThread();
    void workerThread(int id);
    void handleClient(SOCKET client_sock);

    std::string port_;
    int worker_count_;
    SOCKET listen_sock_;
    HANDLE epoll_fd_;

    std::atomic<bool> running_;
    std::vector<std::thread> workers_;
    std::thread listener_;

    std::queue<SOCKET> client_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
};

#endif // SERVER_HPP
