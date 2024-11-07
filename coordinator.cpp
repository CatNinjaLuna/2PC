/*
 * @file coordinator.cpp
 * @author Carolina Li
 * version: 1.0
 * Date: June/2/2024
 * purpose: This C++ file implements a coordinator for a two-phase commit (2PC) protocol
 * using TCP connections. The main function starts by verifying command-line arguments
 * and opening a log file. It then parses transaction details such as the amount to be
 * transferred, origin and destination hostnames, and ports. The coordinator establishes
 * connections to two participants, representing the origin and destination accounts,
 * and sends a vote request for the transaction amount to each participant.
 * Based on the responses, the coordinator decides to either globally commit or
 * abort the transaction and sends the appropriate messages to both participants.
 * It logs all actions and responses, ensuring the transaction's atomicity and
 * consistency by verifying that both participants acknowledge the final decision.
 * The program handles errors and logs outcomes before terminating successfully.
 */

#include <fstream>
#include <cstring>
#include <string>
#include "TCPClient.h"
using namespace std;


int main(int argc, char *argv[]) {
    if (argc < 9) {
        cerr << "usage: coordinator logFileName amount origin_hostname origin_port" <<
                " origin_account destination_hostname destination_port destination_account" << endl;
        return EXIT_FAILURE;
    }
    ofstream outputLog;
    outputLog.open(argv[1], ios::out | ios::app);
    if (!outputLog.is_open()) {
        cout << "Unable to create/open log file" << endl;
        return EXIT_FAILURE;
    }

    double amount = stod(argv[2]);
    int origin_port = atoi(argv[4]);
    if (origin_port < 1 || origin_port >= 1 << 16) {
        cerr << "invalid origin port " << origin_port << endl;
        return EXIT_FAILURE;
    }

    int destination_port = atoi(argv[7]);
    if (destination_port < 1 || destination_port >= 1 << 16) {
        cerr << "invalid destination port " << destination_port << endl;
        return EXIT_FAILURE;
    }

    // connecting between 2 bank--> 2 instances of client
    TCPClient client(argv[3], (u_short) origin_port);
    cout << "Connected to participant " << argv[3] << ":" << origin_port << endl;
    outputLog << "Connected to participant " << argv[3] << ":" << origin_port << endl;
    TCPClient client2(argv[6], (u_short) destination_port);
    cout << "Connected to participant " << argv[6] << ":" << destination_port << endl;
    outputLog << "Connected to participant " << argv[6] << ":" << destination_port << endl;

    string message = string("VOTE-REQUEST ") + argv[4] + " " + to_string(-amount);

    client.send_request(message);
    cout << "Sending message '" << message << "' " << "to " << argv[3] << ":" << origin_port << endl;
    outputLog << "Sending message '" << message << "' " << "to " << argv[3] << ":" << origin_port << endl;
    string response = client.get_response();


    string message2 = string("VOTE-REQUEST ") + argv[4] + " " + to_string(amount);
    client2.send_request(message2);
    cout << "Sending message '" << message2 << "'" << "to " << argv[6] << ":" << destination_port << endl;
    outputLog << "Sending message '" << message2 << "'" << "to " << argv[6] << ":" << destination_port << endl;

    string response2 = client2.get_response();
    bool committed;
    // check if get a VOTE-COMMIT from both client, yes-> global-commit;no from either-->global-abort
    if (response == "VOTE-COMMIT" && response2 == "VOTE-COMMIT") {
        client.send_request("GLOBAL-COMMIT");
        client2.send_request("GLOBAL-COMMIT");
        committed = true;
    } else {
        client.send_request("GLOBAL-ABORT");
        client2.send_request("GLOBAL-ABORT");
        committed = false;
    }

    // Getting the response from both participants
    response = client.get_response();
    response2 = client2.get_response();

    // check if response equals to ack, yes->committed; else->aborted
    if (response == "ACK" && response2 == "ACK") {
        if (committed) {
            cout <<  "Transaction committed" << endl;
            outputLog <<  "Transaction committed" << endl;
        } else {
            cout << "Transaction aborted" << endl;
            outputLog << "Transaction aborted" << endl;
        }
    } else {
        cout << "Error: ACK not received. This should not occur." << endl;
        outputLog << "Error: ACK not received. This should not occur." << endl;
    }
    outputLog.close();

    return EXIT_SUCCESS;
}

