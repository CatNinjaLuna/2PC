/**
 * @file TCPServer.h declaration for a base class TCP/IP server
 * @author Kevin Lundeen
 * @see Seattle University, CPSC 5042, Spring 2024, ICE 4 professor's solution
 */
#pragma once
#include <iostream>

/**
 * @class TCPServer this class is intended to only be used as a base class for
 *                  an application-defined server. It is a simple server that only
 *                  accepts a _single_ client and then after conversing with that client,
 *                  exits.
 *                  The application's subclass is intended to implement overrides of
 *                  two protected methods: start_client() and process():
 *                    o  The start_client() method is called after the clients connection has
 *                       been established.
 *                    o  The process() method is called after every recv() from the client.
 *                    o  The respond() method is available for replies to be sent to the
 *                       client as desired by the subclass.
 *
 *                  Construction creates the server and initializes the socket. The serve()
 *                  method actually starts listening (and will block internally waiting for
 *                  the client). It also blocks in serve() waiting for more recv() calls to
 *                  finish. The server will exit the inter serve() loop if the process() method
 *                  returns false.
 *
 *                  Failures will be thrown as std::runtime_error.
 */
class TCPServer {
public:
    explicit TCPServer(u_short listening_port);
    virtual ~TCPServer();

    // don't allow anything other than explicit construction:
    TCPServer(const TCPServer&) = delete;
    TCPServer& operator=(const TCPServer&) = delete;

    virtual void serve();

protected:
    virtual void start_client(const std::string &their_host, u_short their_port) {}
    virtual bool process(const std::string &incoming_stream_piece) {return false;}
    virtual void respond(const std::string &response);

private:
    int server;  // socket for listening
    int client;  // sockets for a single client
};

