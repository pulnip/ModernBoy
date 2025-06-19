#include <cstring>
#include "network/posix/socket.hpp"

using namespace ModernBoy::POSIX;

int main(int argc, char* argv[]){
    TCPSocket clientSocket{};
    IPv4Address recvAddr(INADDR_LOOPBACK, 42424);
    if(clientSocket.connect(recvAddr))
        return -1;

    clientSocket.send(argv[1], strlen(argv[1])+1);
}