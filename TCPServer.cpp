/**
 * @file TCPServer.cpp definition for a base class TCP/IP server
 * @author Kevin Lundeen
 * @see Seattle University, CPSC 5042, Spring 2024, ICE 4 professor's solution
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include "TCPServer.h"
using namespace std;


TCPServer::TCPServer(u_short port) {
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0)
        throw runtime_error(strerror(errno));
    if (port != 0) {
        sockaddr_in me = {};
        me.sin_family = AF_INET;
        me.sin_port = htons(port);
        me.sin_addr.s_addr = inet_addr("0.0.0.0");
        if (bind(server, (sockaddr *)&me, sizeof(me)) < 0)
            throw runtime_error(strerror(errno));
    }
    listen(server, 1);
    client = -1;
}

TCPServer::~TCPServer() {
    close(server);
}

void TCPServer::serve() {
    sockaddr_in them = {};
    socklen_t them_len = sizeof(them);
    client = accept(server, (sockaddr *) &them, &them_len);
    if (client < 0)
        throw runtime_error(strerror(errno));

    char *their_host = inet_ntoa(them.sin_addr);
    u_short their_port = ntohs(them.sin_port);
    start_client(their_host, their_port);

    while (true) {
        char buffer[1024];
        ssize_t received = recv(client, buffer, sizeof(buffer), 0);
        if (received < 0)
            throw runtime_error(strerror(errno));
        buffer[received] = '\0';  // null-terminate c-style string
        string request(buffer);
        if (!process(request)) {
            return;
        }
    }
}

void TCPServer::respond(const string &response) {
    ssize_t sent = send(client, response.c_str(), response.length(), 0);
    if (sent < 0)
        throw runtime_error(strerror(errno));
}

