#include "tcp_socket_master.hh"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#pragma once

// maybe i should have kept functions static
struct TCPSlaveSocket {
    void set_nonblocking() { // dead code
        // actually to hell with ioctl for now
        const auto mask = fcntl(slave_fd, F_GETFL, 0);
        if (-1 == fcntl(slave_fd, mask | O_NONBLOCK)) { // is it ok when it fails?
            perror("fcntl error");
        }
    }

    TCPSlaveSocket(int _slave_fd_):
        slave_fd(_slave_fd_)
    {}

    static TCPSlaveSocket accept(const TCPMasterSocket& master) {
        const auto ret = ::accept(master.file_descriptor(), NULL, NULL); // use addr here?
        if (-1 == ret) {
            perror("accept error");
        }
        return TCPSlaveSocket{ ret };
    }

    explicit operator int() const {
        return slave_fd;
    }

    std::string recv() const {
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

    int send(const std::string& buf) const {
        const auto ret = ::send(slave_fd, buf.data(), buf.size(), MSG_NOSIGNAL);
        if (-1 == ret) {
            perror("send error");
        }
        return ret;
    }

    void die() {
        if (-1 == shutdown(slave_fd, SHUT_RDWR) ) {
            perror("shutdown error");
        }
        if (-1 == close(slave_fd) ) {
            perror("close error");
        }
    }

private:
    int slave_fd;
};
