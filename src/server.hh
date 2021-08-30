#include "tcp_socket.hh"
#include "opt.hh"
#include "http1.0.hh"
#include "daemon.hh"

#include <stdio.h>
#include <sys/select.h>

#include <set>
#include <vector>
#include <thread>

#pragma once

struct server {
    static void run(const opt::Params& params) {
        daemonize(params.directory.data());
        TCPMasterSocket master;
        std::set<int> slave_fd_set;

        master.bind(params.addr);
        master.listen(100);

        fd_set socket_fds;
        while(true) {
            FD_ZERO(&socket_fds);
            FD_SET(master.file_descriptor(), &socket_fds);
            for(const int slave_fd: slave_fd_set) {
                FD_SET(slave_fd, &socket_fds);
            }
            const auto max_slave_fd = !slave_fd_set.empty() ? *slave_fd_set.rbegin() : 0;
            const auto max_fd = std::max(max_slave_fd, master.file_descriptor());
            if (-1 == select(max_fd + 1, &socket_fds, NULL, NULL, 0) ) {
                perror("select failed");
            }

            if (FD_ISSET(master.file_descriptor(), &socket_fds)) {
                puts("accept");
                const auto slave = tcp_slave_socket::accept(master);
                slave_fd_set.insert(slave);
            }

            std::vector<int> slave_fds_to_be_erased;
            std::vector<std::thread> threads;
            for(const int slave_fd: slave_fd_set) {
                if (FD_ISSET(slave_fd, &socket_fds)) {
                    puts("fd set");
                    threads.push_back(std::thread([=](){ // yes, multithreading... should add a threadpool, at least, lol
                        const auto buf = tcp_slave_socket::recv(slave_fd);
                        if (0 != buf.size()) {
                            const auto path = http_1dot0::get_request_file_path(buf);
                            tcp_slave_socket::send(slave_fd, http_1dot0::get_response(path));
                            puts("sent");
                        }
                        else if (errno != EAGAIN) {
                            if (-1 == shutdown(slave_fd, SHUT_RDWR) ) {
                                perror("shutdown failed");
                            }
                            if (-1 == close(slave_fd) ) {
                                perror("close failed");
                            }
                        }
                    }));
                    slave_fds_to_be_erased.push_back(slave_fd);
                }
            }
            for(auto& thread: threads) {
                thread.join();
            }

            for(const int slave_fd: slave_fds_to_be_erased) {
                slave_fd_set.erase(slave_fd);
            }
        }
    }
};