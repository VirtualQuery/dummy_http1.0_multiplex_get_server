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

        path = path.substr(1); // strip '/'

        // trim cgi params:
        const auto pos = path.find('?');
        if (std::string::npos != pos) {
            path = path.substr(0, pos);
        }

        return path;
    }

    template<const size_t MTU>
    static std::string get_response(const std::string& file_path) {
        std::ifstream fin(file_path);
        if (!fin.good()) {
            return get_response_not_found();
        }
        std::string s(std::istreambuf_iterator<char>(fin), {});
        return get_response_ok<MTU>(s);
    }

    template<const size_t MTU>
    static std::string get_response_ok(const std::string& content) {
        char buf[MTU];
        if (0 > snprintf(
            buf,
            MTU - 1,
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