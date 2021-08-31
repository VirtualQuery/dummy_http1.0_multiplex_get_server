#include "tcp_addr.hh"

#include <sys/socket.h>
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
