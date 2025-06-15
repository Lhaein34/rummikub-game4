#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdio>

static constexpr int PORT         = 12345;
static constexpr int BACKLOG      = 4;
static constexpr int BUFFER_SIZE  = 128;
 
int main(){
    // 1) listening socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2) bind
    sockaddr_in srv_addr{};
    srv_addr.sin_family      = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port        = htons(PORT);
    if (bind(listen_fd, (sockaddr*)&srv_addr, sizeof(srv_addr)) < 0){
        perror("bind"); return 1;
    }

    // 3) listen
    if (listen(listen_fd, BACKLOG) < 0){
        perror("listen"); return 1;
    }
    std::cout<<"[서버] 포트 "<<PORT<<" 에서 대기 중…\n";

    // 4) 첫 클라이언트 → 플레이어 수 결정
    std::cout<<"[서버] 첫 클라이언트 플레이어 수 설정 대기…\n";
    int fd0 = accept(listen_fd, nullptr, nullptr);
    if (fd0 < 0) { perror("accept"); return 1; }
    send(fd0, "SET_COUNT", 9, 0);                // 신호 보냄

    char buf[BUFFER_SIZE];
    int n = recv(fd0, buf, sizeof(buf)-1, 0);     // 플레이어 수 recv
    if (n <= 0) { perror("recv"); return 1; }
    buf[n] = '\0';


    // 숫자만 들어있는지 최소 검사
    for (int i = 0; buf[i]; ++i) {
      if (!isdigit(buf[i])) {
        std::cerr << "[서버] 잘못된 플레이어 수: " << buf << "\n";
        return 1;
      }
    }
    
    int target = atoi(buf);
    if (target<2||target>4) target=2;
    std::cout<<"[서버] 목표 인원수 = "<<target<<" 명\n";
    send(fd0,"OK",2,0);

    std::vector<int> clients;
    clients.push_back(fd0);

    // 5) 나머지 사람들 accept + “OK”
    while ((int)clients.size() < target) {
      int fd = accept(listen_fd,nullptr,nullptr);
      if (fd<0) { perror("accept"); continue; }
      clients.push_back(fd);
      std::cout<<"[서버] 클라이언트 접속 ("<<clients.size()<<"/"<<target<<")\n";
      send(fd,"OK",2,0);
    }

    // 6) 모두 모였으면 START 방송
    std::cout<<"[서버] 모든 클라이언트 접속 완료, START 방송\n";
    for(int fd:clients){
      send(fd,"START",5,0);
    }

    // 7) 게임 진행 루프 (예시: PLACE 메시지 받고, 모든 클라이언트에 UPDATE로 뿌리기)
    std::vector<std::pair<int,int>> table;
    while(true){
      for(int i=0;i<clients.size();++i){
        int fd = clients[i];
        send(fd,"YOUR_TURN",9,0);
        n = recv(fd, buf, sizeof(buf)-1, 0);
        if(n<=0) { perror("recv"); goto END; }
        buf[n]=0;

        int c,num;
        if(sscanf(buf,"PLACE %d %d",&c,&num)==2){
          table.emplace_back(c,num);
          char msg[64];
          int len = snprintf(msg,sizeof(msg),"UPDATE %d %d",c,num);
          for(int fd2:clients) send(fd2,msg,len,0);
        }
      }
    }
END:
    for(int fd:clients) close(fd);
    close(listen_fd);
    return 0;
}