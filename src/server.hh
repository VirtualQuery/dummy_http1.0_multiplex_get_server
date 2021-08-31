#include "tcp_socket_master.hh"
#include "tcp_socket_slave.hh"
#include "opt.hh"
#include "http1.0.hh"
#include "daemon.hh"
#include "thread_pool.hh"

#include <stdio.h>
#include <sys/select.h>

#include <set>
#include <vector>
#include <thread>

#pragma once

struct server {
    static void run(const opt::Params& params) {
        daemonize(params.directory.data());

        ThreadPool thread_pool;

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
                const auto slave = tcp_slave_socket::accept(master);
                //tcp_slave_socket::set_nonblocking(slave);
                slave_fd_set.insert(slave);
            }

            std::vector<int> slave_fds_to_be_erased; // if i want to do non-blocking need to make it threadsafe
            for(const int slave_fd: slave_fd_set) {
                if (FD_ISSET(slave_fd, &socket_fds)) {
                    thread_pool.add_job([slave_fd](){
                        const auto buf = tcp_slave_socket::recv(slave_fd);
                        if (0 != buf.size()) {
                            const auto path = http_1dot0::get_request_file_path(buf);
                            tcp_slave_socket::send(slave_fd, http_1dot0::get_response(path));
                        }
                        tcp_slave_socket::kill(slave_fd); // just do blocking and kill for now

                        // else if (errno != EAGAIN) { // for non-blocking io, no data available
                        //     tcp_slave_socket::kill(slave_fd);
                        //     slave_fds_to_be_erased.push_back(slave_fd); // instead of below
                        // }
                    });
                    slave_fds_to_be_erased.push_back(slave_fd);
                }
            }

            for(const int slave_fd: slave_fds_to_be_erased) {
                slave_fd_set.erase(slave_fd);
            }
        }
    }
};