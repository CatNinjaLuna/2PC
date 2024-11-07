/**
 * @file TCPClient.cpp definition for TCPClient
 * @author Kevin Lundeen
 * @see Seattle University, CPSC 5042, Spring 2024, ICE 4 professor's solution
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include "TCPClient.h"
using namespace std;

TCPClient::TCPClient(const string &server_host, const u_short server_port) {
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0)
        throw runtime_error(strerror(errno));

    hostent *answer;
    answer = gethostbyname(server_host.c_str());
    if (answer == nullptr)
        throw runtime_error(strerror(h_errno));

    sockaddr_in to = {};
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = *(in_addr_t *)answer->h_addr;
    to.sin_port = htons(server_port);
    if (connect(server, (struct sockaddr *) &to, sizeof(to)) < 0)
        throw runtime_error(strerror(errno));
}

TCPClient::~TCPClient() {
    if (server != -1) {
        close(server);
        server = -1;
    }
}

void TCPClient::send_request(const string &request) {
    const char *message = request.c_str();
    ssize_t length = strlen(message);
    ssize_t sent = send(server, message, length, 0);
    if (sent < 0)
        throw runtime_error(strerror(errno));
}

string TCPClient::get_response() {
    char buffer[4096];
    ssize_t received = recv(server, buffer, sizeof(buffer), 0);
    if (received < 0)
        throw runtime_error(strerror(errno));
    buffer[received] = '\0';
    return string(buffer);
}

TCPClient::TCPClient(TCPClient &&other) noexcept {
    this->server = other.server;
    other.server = -1;
}

