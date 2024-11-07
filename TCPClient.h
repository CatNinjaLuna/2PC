/**
 * @file TCPClient.h declaration for a TCP/IP client
 * @author Kevin Lundeen
 * @see Seattle University, CPSC 5042, Spring 2024, ICE 4 professor's solution
 */
#pragma once
#include <string>
#include <iostream>


/**
 * @class TCPClient  attempts to connect to the server specified by the
 *                   constructor arguments. The send_request() method will
 *                   send the given string to the server and get_response()
 *                   will block waiting for a response.
 *
 *                   Failures will be thrown as std::runtime_error.
 */
class TCPClient {
public:
    TCPClient(const std::string &server_host, u_short server_port);
    TCPClient(TCPClient &&other) noexcept;  // we may want this for std::vector building
    virtual ~TCPClient();

    // don't allow any of these:
    TCPClient(const TCPClient &) = delete;
    TCPClient& operator=(const TCPClient &) = delete;
    TCPClient& operator=(TCPClient &&) = delete;

    void send_request(const std::string &request);
    std::string get_response();

private:
    int server;  // socket
};

