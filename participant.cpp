/**
 * @file participant.cpp
 * @author Carolina Li
 * version: 1.0
 * Date: June/2/2024
 * purpose: This C++ file defines a transaction processing server class, P2Server,
 * which extends a TCPServer to handle two-phase commit (2PC) protocols for
 * distributed transactions. The server manages multiple Account objects,
 * representing accounts with balances, and processes incoming requests to
 * either commit or abort transactions based on the availability of funds
 * and the current state of the 2PC protocol. The P2Server initializes by
 * loading account data from a file and logs all transactions to a log file.
 * It transitions through various states (INIT, ABORT, READY, COMMIT)
 * in response to coordinator messages, ensuring atomicity and consistency
 * of transactions by committing or aborting changes to account balances as instructed.
 * The main function initializes the server with specified parameters and starts the transaction service.
 */
#include "TCPServer.h"
#include <cstring>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

// states for the 2PC participant FSM
enum State{
    INIT,
    ABORT,
    READY,
    COMMIT
};

class Account {

public:
    // constructor for account
    Account(string accountName, double amount) {
        this->accountName = accountName;
        this->amount = amount;
    }

    // getters
    string getAccountName() const{
        return accountName;
    }

    double getAmount() const{
        return amount;
    }

    // add or minus amount based on transaction
    void changeAmount(double transAmount) {
        amount += transAmount;
    }

private:
    double amount;
    string accountName;
};



class P2Server : public TCPServer {
public:
    explicit P2Server(u_short listening_port, string accountFileName, string logFileName) : TCPServer(listening_port) {
        state = INIT;
        this->accountFileName = accountFileName;
        // open the accountFileName
        ifstream accountFile;
        accountFile.open(accountFileName);
        if (!accountFile.is_open()) {
            cout << "Unable to open account file" << endl;
            exit(EXIT_FAILURE);
        }
        // process rows inside accountFile
        string line;

        while(getline(accountFile, line)) {
            // balance
            size_t loc = line.find(' '); // loc of space
            double balance;
            balance = stod(line.substr(0,loc));
            // get the rest of string into an account
            string accountName = line.substr(loc + 1);
            Account account(accountName, balance);
            accountVec.push_back(account);

        }
        accountFile.close();

        // open the logFile-----> pick up next time
        outputLog.open(logFileName);
        if (!outputLog.is_open()) {
            cout << "Unable to create/open log file" << endl;
            exit(EXIT_FAILURE);
        }

        // write to console and write to outputLog
        cout << "Transaction service on port " << listening_port << " (Ctrl-C to stop)" << endl;
        outputLog << "Transaction service on port " << listening_port << " (Ctrl-C to stop)" << endl;
    }

    // destructor for P2Server
    ~P2Server() {
        outputLog.close();
    }

private:
    vector<Account> accountVec;
    ofstream outputLog;
    State state;
    double amount;
    string accountName;
    string accountFileName;
protected:
    void start_client(const std::string &their_host, u_short their_port) override {
        cout << "Accepting coordinator connection. State: INIT" << endl;
    }

    bool process(const std::string &request) override {
        if (state == INIT) {
            cout << "Accepting coordinator connection. State: INIT" << endl;
            size_t loc = request.find(' '); // loc of space

            // rest of string
            string nameMessage = request.substr(0, loc);
            if (nameMessage != "VOTE-REQUEST") {
                cout << "GOT " << nameMessage << ", replying VOTE-ABORT, State: ABORT" << endl;
                outputLog << "GOT " << nameMessage << ", replying VOTE-ABORT, State: ABORT" << endl;
                cout << "Releasing hold from account" << endl;
                outputLog << "Releasing hold from account" << endl;
                return false; //cause participant to exit
            }
            string trans = request.substr(loc + 1);
            loc = trans.find(' '); // loc of space
            amount = stod(trans.substr(0, loc));

            // get the rest of string into an account
            accountName = trans.substr(loc + 1);
            cout << "Holding " << (-1 * amount) << " from account " << accountName << endl;

            // Verifies that account exist and account has sufficient funds
            vector<Account>::iterator it = accountVec.begin();
            for (; it < accountVec.end(); it++) {
                // is there an account match
                if (it->getAccountName() == accountName) {
                    break;
                }
            }
            // the account exist but there is no sufficient money
            bool commit = true;
            if (it == accountVec.end() || (amount < 0 && it->getAmount() < (amount * -1))) {
                cout << "Got VOTE-REQUEST, replying VOTE-ABORT. State: ABORT" << endl;
                outputLog << "Got VOTE-REQUEST, replying VOTE-ABORT. State: ABORT" << endl;
                commit = false;
            }
            if (!commit) {
                cout << "Releasing hold from account" << endl;
                outputLog << "Releasing hold from account" << endl;
                respond("VOTE-ABORT");
                // cout << "Closing server connection..." << endl;
                // returning false is a signal to shut down server
                return false;
            } else {
                respond("VOTE-COMMIT");
            }
            //
            state = READY;
            return true;
        } else if (state == READY) {
            if (request == "GLOBAL-ABORT") {
                cout << "GOT " << request << ", replying ACK, State: ABORT" << endl;
                outputLog << "GOT " << request << ", replying ACK, State: ABORT" << endl;
                cout << "Releasing hold from account " << accountName<< endl;
                outputLog << "Releasing hold from account " << accountName << endl;
                state = INIT;

            } else if (request == "GLOBAL-COMMIT") {
                // change balance of account
                vector<Account>::iterator it = accountVec.begin();
                for (; it < accountVec.end(); it++) {
                    // is there an account match
                    if (it->getAccountName() == accountName) {
                        break;
                    }
                } // if there is a match
                it->changeAmount(amount);

                cout << "Got " << request << ", replying ACK. State: COMMIT" << endl;
                outputLog << "Got " << request << ", replying ACK. State: COMMIT" << endl;

                cout << "Committing " << amount << " from account " << accountName << endl;
                outputLog << "Committing " << amount << " from account " << accountName << endl;
                // open account, write back to file
                ofstream accountFile;
                accountFile.open(accountFileName);
                if (!accountFile.is_open()) {
                    cout << "Unable to open account file" << endl;
                    exit(EXIT_FAILURE);
                }
                // process rows inside accountFile
                string line;

                it = accountVec.begin();
                for (; it < accountVec.end(); it++) {
                    accountFile << it->getAmount() << " " << it->getAccountName() << endl;
                }
                accountFile.close();
                state = INIT;
            } else { // error handling
                // reply back
                cout << "GOT " << request << ", replying ACK, State: INIT" << endl;
                outputLog << "GOT " << request << ", replying ACK, State: INIT" << endl;
                cout << "Releasing hold from account " << accountName << endl;
                outputLog << "Releasing hold from account " << accountName << endl;
                state = INIT;
            }
        }
        return false;
    }
};

int main(int argc, char *argv[]) {

    if (argc < 4) {
        cerr << "usage: participant port accountFileName logFileName" << endl;
        return EXIT_FAILURE;
    }
    int port = atoi(argv[1]);
    if (port < 1 || port >= 1 << 16) {
        cerr << "invalid port " << port << endl;
        return EXIT_FAILURE;
    }
    P2Server server((u_short) port, argv[2], argv[3]);
    server.serve();
    return EXIT_SUCCESS;
}

