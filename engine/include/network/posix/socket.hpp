#ifndef MODERNBOY_NETWORK_WRAPPER_HPP
#define MODERNBOY_NETWORK_WRAPPER_HPP

#include <cstdint>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>

namespace ModernBoy{
    namespace POSIX{
        class IPv4Address{
        public:
            IPv4Address(){}
            IPv4Address(uint32_t address, uint16_t port);
            IPv4Address(const sockaddr& address);

            const sockaddr* get() const{
                return reinterpret_cast<const sockaddr*>(&addr); }
            sockaddr* get(){
                return reinterpret_cast<sockaddr*>(&addr); }
            constexpr socklen_t size() const{
                return sizeof(sockaddr); }

        private:
            sockaddr_in addr;
        };
        class UDPSocket{
        public:
            UDPSocket();
            ~UDPSocket();
            UDPSocket(const UDPSocket& other)=delete;
            UDPSocket(UDPSocket&& other);
            UDPSocket& operator=(const UDPSocket& other)=delete;
            UDPSocket& operator=(UDPSocket&& other);

            bool bind(const IPv4Address& toAddr);
            bool sentTo(const void* data, size_t len,
                const IPv4Address& toAddr);
            bool recvFrom(void* buf, size_t len,
                IPv4Address& fromAddr);

            bool setMode(bool nonBlock);

            bool operator==(const UDPSocket& other);


        private:
            UDPSocket(int socket)
            :socket(socket){}
            void moveFrom(UDPSocket&& other);

            int socket;
        };
        class TCPSocket{
        public:
            TCPSocket();
            ~TCPSocket();
            TCPSocket(const TCPSocket& other)=delete;
            TCPSocket(TCPSocket&& other);
            TCPSocket& operator=(const TCPSocket& other)=delete;
            TCPSocket& operator=(TCPSocket&& other);

            bool connect(const IPv4Address& toAddr) const;
            bool bind(const IPv4Address& toAddr) const;
            bool listen(int backlog=32);
            bool accept(IPv4Address& fromAddr,
                TCPSocket& outSocket) const;
            bool send(const void* data, size_t len) const;
            bool receive(void* buf, size_t len) const;

            static int select(
                std::vector<TCPSocket>* readSet,
                std::vector<TCPSocket*>* readable,
                std::vector<TCPSocket>* writeSet,
                std::vector<TCPSocket*>* writable,
                std::vector<TCPSocket>* exceptSet,
                std::vector<TCPSocket*>* occured
            );

            bool setMode(bool nonBlock);
            int get() const{ return socket; }
            bool listenSocket() const{ return isListenSocket; }

            bool operator==(const TCPSocket& other);

        private:
            void moveFrom(TCPSocket&& other);
            TCPSocket(int socket)
            :socket(socket){}

            int socket = -1;
            bool isListenSocket = false;
        };
    }
} // namespace ModernBoy

#endif // MODERNBOY_NETWORK_WRAPPER_HPP