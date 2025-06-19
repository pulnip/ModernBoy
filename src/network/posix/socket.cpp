#include <cstdio>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include "network/posix/socket.hpp"

using namespace ModernBoy;
using namespace ModernBoy::POSIX;

IPv4Address::IPv4Address(uint32_t address, uint16_t port)
:addr{.sin_family=AF_INET, .sin_port=htons(port),
.sin_addr{.s_addr=htonl(address)},
.sin_zero={0, 0, 0, 0, 0, 0, 0, 0}}{}
IPv4Address::IPv4Address(const sockaddr& address){
    static_assert(sizeof(sockaddr)==sizeof(sockaddr_in));
    memcpy(&addr, &address, sizeof(sockaddr_in));
}

UDPSocket::UDPSocket()
:socket(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)){
    if(socket == -1)
        throw std::runtime_error("Invalid UDPSocket!");
}
UDPSocket::~UDPSocket(){
    close(socket);
}
UDPSocket::UDPSocket(UDPSocket&& other){
    moveFrom(std::move(other));
}
UDPSocket& UDPSocket::operator=(UDPSocket&& other){
    moveFrom(std::move(other));
    return *this;
}
void UDPSocket::moveFrom(UDPSocket&& other){
    socket = other.socket;
    other.socket = -1;
}
bool UDPSocket::bind(const IPv4Address& toAddr){
    auto err = ::bind(socket, toAddr.get(), toAddr.size());
    return err==0 ? false : true;
}
bool UDPSocket::sentTo(const void* data, size_t len,
    const IPv4Address& toAddr
){
    int numByte = ::sendto(socket, data, len,
        0, toAddr.get(), toAddr.size());
    return 0<=numByte && numByte<len ? false : true;
}
bool UDPSocket::recvFrom(void* buf, size_t len,
    IPv4Address& fromAddr
){
    socklen_t fromAddrLen=fromAddr.size();
    int numByte = ::recvfrom(socket, buf, len,
        0, fromAddr.get(), &fromAddrLen
    );
    return 0< numByte && numByte<len ? false : true;
}
bool UDPSocket::setMode(bool nonBlock){
    int flags = ::fcntl(socket, F_GETFL, 0);
    flags = nonBlock ? (flags|O_NONBLOCK) : (flags&~O_NONBLOCK);
    int err = ::fcntl(socket, F_SETFL, flags);
    return err==0 ? false : true;
}
bool UDPSocket::operator==(const UDPSocket& other){
    return socket==other.socket;
}

TCPSocket::TCPSocket()
:socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)){
    if(socket == -1)
        throw std::runtime_error("Invalid TCPSocket!");
}
TCPSocket::~TCPSocket(){
    if(socket == -1) return;
    close(socket);
}
TCPSocket::TCPSocket(TCPSocket&& other){
    moveFrom(std::move(other));
}
TCPSocket& TCPSocket::operator=(TCPSocket&& other){
    moveFrom(std::move(other));
    return *this;
}
void TCPSocket::moveFrom(TCPSocket&& other){
    socket = other.socket;
    isListenSocket = other.isListenSocket;
    other.socket = -1;
    other.isListenSocket = false;
}
bool TCPSocket::connect(const IPv4Address& toAddr) const{
    auto err = ::connect(socket, toAddr.get(), toAddr.size());
    return err==0 ? false : true;
}
bool TCPSocket::bind(const IPv4Address& toAddr) const{
    auto err = ::bind(socket, toAddr.get(), toAddr.size());
    return err==0 ? false : true;
}
bool TCPSocket::listen(int backlog){
    isListenSocket = true;
    auto err = ::listen(socket, backlog);
    return err==0 ? false : true;
}
bool TCPSocket::accept(IPv4Address& fromAddr,
    TCPSocket& outSocket
) const{
    socklen_t fromAddrLen = fromAddr.size();
    int newSocket = ::accept(socket,
        fromAddr.get(), &fromAddrLen);
    outSocket.socket = newSocket;
    return newSocket>=0 ? false : true;
}
bool TCPSocket::send(const void* data, size_t len) const{
    int numByte = ::send(socket, data, len, 0);
    return 0<=numByte && numByte<len ? false : true;
}
bool TCPSocket::receive(void* buf, size_t len) const{
    int numByte = ::recv(socket, buf, len, 0);
    // static_cast<char*>(buf)[numByte] = '\0';
    return 0< numByte && numByte<len ? false : true;
}
bool TCPSocket::setMode(bool nonBlock){
    int flags = ::fcntl(socket, F_GETFL, 0);
    flags = nonBlock ? (flags|O_NONBLOCK) : (flags&~O_NONBLOCK);
    int err = ::fcntl(socket, F_SETFL, flags);
    return err==0 ? false : true;
}
bool TCPSocket::operator==(const TCPSocket& other){
    return socket==other.socket;
}

static int fillSet(const std::vector<TCPSocket>& v,
    fd_set& out_set);
static void fillVector(const fd_set& set,
    std::vector<TCPSocket*>& v,
    std::vector<TCPSocket>& origin);

int TCPSocket::select(
    std::vector<TCPSocket>* readSet,
    std::vector<TCPSocket*>* readable,
    std::vector<TCPSocket>* writeSet,
    std::vector<TCPSocket*>* writable,
    std::vector<TCPSocket>* exceptSet,
    std::vector<TCPSocket*>* occured
){
    fd_set read, write, except;
    int nfds=0;
    if(readSet) nfds=std::max(nfds, fillSet(*readSet, read));
    if(writeSet) nfds=std::max(nfds, fillSet(*writeSet, write));
    if(exceptSet) nfds=std::max(nfds, fillSet(*exceptSet, except));
    int ret = ::select(nfds+1, &read, &write, &except, nullptr);
    if(ret > 0){
        if(readable) fillVector(read, *readable, *readSet);
        if(writable) fillVector(write, *writable, *writeSet);
        if(occured) fillVector(except, *occured, *exceptSet);
    }
    return ret;
}

static int fillSet(const std::vector<TCPSocket>& v,
    fd_set& out_set
){
    int nfds=0;
    FD_ZERO(&out_set);
    for(const auto& socket: v){
        FD_SET(socket.get(), &out_set);
        nfds = std::max(nfds, socket.get());
    }
    return nfds;
}
static void fillVector(const fd_set& set,
    std::vector<TCPSocket*>& v,
    std::vector<TCPSocket>& origin
){
    v.clear();
    for(auto& socket: origin){
        if(FD_ISSET(socket.get(), &set))
            v.emplace_back(&socket);
    }
}

