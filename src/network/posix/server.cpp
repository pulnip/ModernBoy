#include <cstring>
#include "network/posix/socket.hpp"

using namespace ModernBoy::POSIX;

int main(void){
    TCPSocket listenSocket{};
    IPv4Address address(INADDR_ANY, 42424);
    if(listenSocket.bind(address)){
        perror("bind error!");
        return -1;
    }
    if(listenSocket.listen()) {
        perror("listen error!");
        return -1;
    }
    std::vector<TCPSocket> readSockets;
    std::vector<TCPSocket*> readable;
    std::vector<TCPSocket*> disconnected;
    readSockets.emplace_back(std::move(listenSocket));

    bool isActive = true;
    while(isActive){
        if(TCPSocket::select(&readSockets, &readable,
            nullptr, nullptr, nullptr, nullptr)==0) continue;

        disconnected.clear();
        for(const auto& pSocket: readable){
            if(pSocket->listenSocket()){
                IPv4Address newAddr;
                TCPSocket newClient;
                pSocket->accept(newAddr, newClient);
                readSockets.emplace_back(std::move(newClient));
            }
            else{
                char segment[512];
                if(pSocket->receive(segment, 512)){
                    disconnected.push_back(pSocket);
                }
                else{
                    printf("%s\n", segment);
                    if(strcmp(segment, "Quit")==0)
                        isActive = false;
                }
            }
        }
        for(const auto& pSocket: disconnected){
            std::erase_if(readSockets,
                [pSocket](const auto& socket){
                    return socket == *pSocket;
                }
            );
        }
    }
}