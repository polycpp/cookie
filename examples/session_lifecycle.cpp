// Demonstrates the sign-in → refresh → sign-out cookie lifecycle.
// Run with `signin`, `refresh`, or `signout` to print the Set-Cookie
// header that would be returned to the client.

#include <iostream>
#include <string>
#include <polycpp/cookie/cookie.hpp>

namespace cookie = polycpp::cookie;

namespace {

cookie::SerializeOptions sessionAttrs() {
    // Same attributes across all lifecycle phases — matching `Path` is
    // critical so the deletion targets the right cookie.
    return cookie::SerializeOptions{
        .path     = "/",
        .httpOnly = true,
        .secure   = true,
        .sameSite = "lax",
    };
}

std::string signIn() {
    auto opts = sessionAttrs();
    opts.maxAge = 60 * 60 * 2;
    return cookie::serialize("sid", "fresh-session-id", opts);
}

std::string refresh() {
    auto opts = sessionAttrs();
    opts.maxAge = 60 * 60 * 2;  // slide the expiry forward
    return cookie::serialize("sid", "still-same-session-id", opts);
}

std::string signOut() {
    auto opts = sessionAttrs();
    opts.maxAge = 0;            // tell the browser to drop it
    return cookie::serialize("sid", "", opts);
}

void usage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " (signin|refresh|signout)\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) { usage(argv[0]); return 2; }
    const std::string cmd = argv[1];

    if (cmd == "signin")  { std::cout << "Set-Cookie: " << signIn()  << '\n'; }
    else if (cmd == "refresh") { std::cout << "Set-Cookie: " << refresh() << '\n'; }
    else if (cmd == "signout") { std::cout << "Set-Cookie: " << signOut() << '\n'; }
    else { usage(argv[0]); return 2; }

    return 0;
}
