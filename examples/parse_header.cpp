// Read a Cookie header value on stdin, print each pair on its own line.
//
//   $ echo 'a=1; b=2' | ./parse_header
//   a=1
//   b=2

#include <iostream>
#include <string>
#include <polycpp/cookie/cookie.hpp>

int main() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return 0;
    }
    for (const auto& [name, value] : polycpp::cookie::parse(line)) {
        std::cout << name << '=' << value << '\n';
    }
}
