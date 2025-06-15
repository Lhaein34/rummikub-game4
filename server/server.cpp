#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <sstream>  // 🔑 추가: 문자열 줄 단위 처리용

static constexpr int PORT = 12345;
static constexpr int BACKLOG = 4;
static constexpr int BUFFER_SIZE = 1024;

std::vector<std::pair<int, int>> table;        // 테이블 위 타일들
std::vector<int> clients;                      // 클라이언트 소켓들
std::mutex table_mutex;
std::mutex client_mutex;

void broadcast_to_all(const std::string &msg, int except_fd = -1) {
    std::lock_guard<std::mutex> lock(client_mutex);
    for (int fd : clients) {
        if (fd != except_fd) {
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
}

void handle_client(int fd) {
    char buf[BUFFER_SIZE];
    while (true) {
        int n = recv(fd, buf, BUFFER_SIZE - 1, 0);
        if (n <= 0) {
            std::cerr << "[서버] 클라이언트 연결 종료 (" << fd << ")\n";
            close(fd);
            std::lock_guard<std::mutex> lock(client_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), fd), clients.end());
            return;
        }

        buf[n] = '\0';
        std::istringstream iss(buf);
        std::string line;

        while (std::getline(iss, line)) {
            int color, num;
            if (std::sscanf(line.c_str(), "PLACE %d %d", &color, &num) == 2) {
                {
                    std::lock_guard<std::mutex> lock(table_mutex);
                    table.emplace_back(color, num);
                }

                char msg[64];
                int L = std::snprintf(msg, sizeof(msg), "UPDATE %d %d\n", color, num);
                broadcast_to_all(std::string(msg, L), fd);
            }
        }

        // 턴 알림
        send(fd, "YOUR_TURN", 9, 0);
    }
}

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "[서버] 대기 중...\n";

    // 첫 클라이언트로부터 플레이어 수 결정
    int fd0 = accept(listen_fd, nullptr, nullptr);
    if (fd0 < 0) { perror("accept"); return 1; }
    send(fd0, "SET_COUNT", 9, 0);

    char buf[BUFFER_SIZE];
    int n = recv(fd0, buf, BUFFER_SIZE - 1, 0);
    if (n <= 0) { perror("recv"); return 1; }
    buf[n] = '\0';

    for (int i = 0; buf[i]; ++i) {
        if (!isdigit(buf[i])) {
            std::cerr << "잘못된 숫자 입력\n";
            return 1;
        }
    }

    int target = std::stoi(buf);
    if (target < 2 || target > 4) target = 2;

    std::cout << "[서버] 총 플레이어 수 = " << target << "\n";
    send(fd0, "OK", 2, 0);

    {
        std::lock_guard<std::mutex> lock(client_mutex);
        clients.push_back(fd0);
    }

    while ((int)clients.size() < target) {
        int fd = accept(listen_fd, nullptr, nullptr);
        if (fd < 0) {
            perror("accept");
            continue;
        }
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            clients.push_back(fd);
        }
        std::cout << "[서버] 클라이언트 접속 (" << clients.size() << "/" << target << ")\n";
        send(fd, "OK", 2, 0);
    }

    std::cout << "[서버] 모든 클라이언트 접속 완료\n";
    broadcast_to_all("START");
    // 2. broadcast 현재 테이블 상태 전체
{
    std::lock_guard<std::mutex> lock(table_mutex);
    std::string all_updates;
    for (auto &[color, number] : table) {
        all_updates += "UPDATE " + std::to_string(color) + " " + std::to_string(number) + "\n";
    }
    broadcast_to_all(all_updates);  // 새로 접속한 클라이언트가 반영 가능
}

    // 각 클라이언트마다 스레드 실행
    std::vector<std::thread> threads;
    for (int fd : clients) {
        threads.emplace_back(handle_client, fd);
    }

    for (auto &t : threads) {
        if (t.joinable()) t.join();
    }

    for (int fd : clients) close(fd);
    close(listen_fd);
    return 0;
}
