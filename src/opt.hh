#include "tcp_addr.hh"
#pragma once

struct opt {
public:
struct Params {
    const TCPAddr addr;
    const std::string directory;
};

public:
    static Params parse_cli_params(int argc, char* argv[]) {
        std::string ip;
        std::string port;
        std::string directory;
        while(1) {
            const int opt = getopt(argc, argv, "h:p:d:");
            if (-1 == opt) break;
            switch(opt) {
                case 'h':
                    ip = optarg;
                    break;
                case 'p':
                    port = optarg;
                    break;
                case 'd':
                    directory = optarg;
                    break;
            }
        }
        const auto addr = TCPAddr::from_strs(ip, port);
        return Params {
            addr,
            directory,
        };
    }
};
