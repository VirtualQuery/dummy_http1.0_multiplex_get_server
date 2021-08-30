#include <getopt.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <string>
#pragma once

struct TCPAddr {
    static TCPAddr from_strs(const std::string& ip_str, const std::string& port_str) {
        uint16_t port;
        if (EOF == sscanf(port_str.data(), "%hu", &port)) {
            perror("sscanf error");
        }

        TCPAddr addr;
        addr.addr.sin_family = AF_INET;
        if (0 == inet_pton(AF_INET, ip_str.data(), &addr.addr.sin_addr)) {
            perror("inet_pton error");
        }
        addr.addr.sin_port = htons(port);
        return addr;
    }

    std::string ip_str() const {
        char str[INET_ADDRSTRLEN];
        str[sizeof(str) - 1] = 0;
        inet_ntop(AF_INET, &addr.sin_addr, str, INET_ADDRSTRLEN);
        return str;
    }

    sockaddr_in addr;
};