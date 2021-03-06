/* Copyright (c) 2012-19, Robert J. Hansen <rob@hansen.engineering>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "common.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <regex>
#include <vector>
#if WINDOWS | WIN32
#include "winsock.h"
#endif

using std::cerr;
using std::cin;
using std::copy;
using std::cout;
using std::ostream_iterator;
using std::sort;
using std::string;
using std::transform;
using std::unique;
using std::vector;
using std::regex;

string SERVER, PORT;
bool SCORE_HITS { false };

// intera edit
bool SHA1 { false };
bool SHA256 { false };

int main_orig(int argc, char* argv[])
{
#if WINDOWS | WIN32
    WSAData wsad;
    if (0 != WSAStartup(MAKEWORD(2, 0), &wsad)) {
        std::cerr << "Error: could not initialize Winsock.\n\n"
                     "You're running a very old version of Windows.  nsrllookup "
                     "won't work\non this system.\n";
        bomb(-1);
    }
#endif
    vector<string> hashes;
    const regex valid_line{ "^[A-F0-9]{32}",
		std::regex_constants::icase | std::regex_constants::optimize };

    parse_options(argc, argv);

    string line;
    while (cin) {
        getline(cin, line);
        transform(line.begin(), line.end(), line.begin(), ::toupper);
        if (regex_search(line, valid_line)) {
            hashes.emplace_back(string(line.begin(), line.begin() + 32));
        }
    }

    sort(hashes.begin(), hashes.end());
    hashes.erase(unique(hashes.begin(), hashes.end()), hashes.end());

    auto answers = query_server(hashes);
    copy(answers.cbegin(), answers.cend(), ostream_iterator<string>(cout, "\n"));
#if WINDOWS | WIN32
    WSACleanup();
#endif
    return EXIT_SUCCESS;
}

// intera edit
int main(int argc, char* argv[])
{
#if WINDOWS | WIN32
    WSAData wsad;
    if (0 != WSAStartup(MAKEWORD(2, 0), &wsad)) {
        std::cerr << "Error: could not initialize Winsock.\n\n"
                     "You're running a very old version of Windows.  nsrllookup "
                     "won't work\non this system.\n";
        bomb(-1);
    }
#endif
    vector<string> hashes;
    int length;
    regex valid_line;
    parse_options(argc, argv);
    const regex valid_line_md5{"^[A-F0-9]{32}$", std::regex_constants::optimize};
    const regex valid_line_sha1{"^[A-F0-9]{40}$", std::regex_constants::optimize};
    const regex valid_line_sha256{"^[A-F0-9]{64}$", std::regex_constants::optimize};
    if (SHA1) {
      valid_line = valid_line_sha1;
      length = 40;
    }
    else if (SHA256) {
      valid_line = valid_line_sha256;
      length = 64;
    }
    else {
      valid_line = valid_line_md5;
      length = 32;
    }
    string line;
    while (cin) {
        getline(cin, line);
        if (length > line.size()) continue;
        if (line[line.size() - 1] == '\r') line.erase(line.size() - 1);
        transform(line.begin(), line.end(), line.begin(), ::toupper);
        if (regex_match(line, valid_line)) hashes.emplace_back(line);
    }
    auto answers = query_server(hashes);
    copy(answers.cbegin(), answers.cend(), ostream_iterator<string>(cout, "\n"));
#if WINDOWS | WIN32
    WSACleanup();
#endif
    return EXIT_SUCCESS;
}
