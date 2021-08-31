#include "tcp_addr.hh"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <string>

#pragma once

struct TCPMasterSocket {
    int bind(const TCPAddr& addr) {
        const auto ret = ::bind(fd, (sockaddr*)&addr.addr, sizeof(sockaddr_in));
        if (-1 == ret) {
            perror("bind failed");
        }
        return ret;
    }

    TCPMasterSocket() {
        const auto s = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (-1 == s) {
            perror("socket failed");
        }
        fd = s;
    }

    int listen(int backlog) {
        const auto ret = ::listen(fd, backlog);
        if (-1 == ret) {
            perror("listen failed");
        }
        return ret;
    }

    int file_descriptor() const {
        return fd;
    }

private:
    int fd;
};

// change signatures to non-static ones?
// will set still maintain order?
// or maybe just cast inbetween keeping in set?
// wish i could just extend int or something

// probably should just define "less" and hope that it will be as fast as ints
struct tcp_slave_socket {
    static void set_nonblocking(int slave_fd) {
        // actually to hell with ioctl for now
        const auto mask = fcntl(slave_fd, F_GETFL, 0);
        if (-1 == fcntl(slave_fd, mask | O_NONBLOCK)) { // is it ok when it fails?
            perror("fcntl error");
        }
    }

    static int accept(const TCPMasterSocket& master) {
        const auto ret = ::accept(master.file_descriptor(), NULL, NULL); // use addr here?
        if (-1 == ret) {
            perror("accept error");
        }
        return ret;
    }

    static std::string recv(int slave_fd) {
        constexpr size_t MAX_SIZE = 3000;
        char buf[MAX_SIZE + 1];
        const auto n_bytes_recv = ::recv(slave_fd, buf, MAX_SIZE, MSG_NOSIGNAL); // do i really need MSG_NOSIGNAL everywhere?
        if (-1 == n_bytes_recv) {
            perror("recv error");
            return "";
        }
        buf[n_bytes_recv] = 0;
        return buf;
    }

    static int send(int slave_fd, const std::string& buf) {
        const auto ret = ::send(slave_fd, buf.data(), buf.size(), MSG_NOSIGNAL);
        if (-1 == ret) {
            perror("send error");
        }
        return ret;
    }

    static void kill(int slave_fd) {
        if (-1 == shutdown(slave_fd, SHUT_RDWR) ) {
            perror("shutdown error");
        }
        if (-1 == close(slave_fd) ) {
            perror("close error");
        }
    }
};