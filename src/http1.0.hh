#include <cstring>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#pragma once

struct http_1dot0 {
    static std::string get_request_file_path(const std::string& get_request) { // not very clever, but should work for now
        std::stringstream ss(get_request);
        std::string GET, path;
        ss >> GET >> path;
        return path.substr(1); // strip '/'
    }

    static std::string get_response(const std::string& file_path) {
        std::ifstream fin(file_path);
        if (!fin.good()) {
            return get_response_not_found();
        }
        std::string s(std::istreambuf_iterator<char>(fin), {});
        return get_response_ok(s);
    }

    static std::string get_response_ok(const std::string& content) {
        char buf[5000];
        if (0 > snprintf(
            buf,
            5000,
            "HTTP/1.0 200 OK\r\n"
            "Content-length: %zu\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "%s",
            content.size(), content.data()
        ) ) {
            perror("snprintf error");
        }
        return buf;
    }

    static std::string get_response_not_found() {
        return
            "HTTP/1.0 404 NOT FOUND\r\n"
            "Content-Type: text/html\r\n"
            "\r\n";
    }
};