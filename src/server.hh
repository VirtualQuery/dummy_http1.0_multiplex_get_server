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
    template<const size_t MTU>
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
                const auto slave = TCPSlaveSocket<MTU>::accept(master);
                slave_fd_set.insert(static_cast<int>(slave));
            }

            for(const auto it = slave_fd_set.begin(); it != slave_fd_set.end(); /* empty */) {
                const auto slave_fd = *it;
                if (FD_ISSET(slave_fd, &socket_fds)) {
                    thread_pool.add_job([slave_fd](){
                        TCPSlaveSocket<MTU> slave(slave_fd);
                        const auto buf = slave.recv();
                        if (0 != buf.size()) {
                            const auto path = http_1dot0::get_request_file_path(buf);
                            slave.send(http_1dot0::get_response<MTU>(path));
                        }
                        slave.die();
                    });
                    it = slave_fd_set.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }
};