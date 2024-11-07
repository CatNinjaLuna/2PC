P2: 2PC Using TCP
This project implements a simple Two-Phase Commit (2PC) protocol using TCP
connections to ensure atomicity and consistency in distributed transactions
between multiple participants. The system comprises a coordinator and multiple
participants, each managing a set of accounts. The project includes
implementations for TCP clients and servers, along with specific classes
for handling 2PC messages and state transitions.

The components is listed below:
1. TCPClient
A class for establishing TCP connections with a server, sending requests,
and receiving responses.
Constructor: Initializes the TCP client with a specified server host and port.
Destructor: Closes the TCP connection.
send_request: Sends a request message to the server.
get_response: Receives a response message from the server.

2. TCPServer
A class for setting up a TCP server that listens for incoming connections and processes requests.
Constructor: Initializes the TCP server on a specified port.
Destructor: Closes the server connection.
serve: Listens for incoming client connections and processes requests in a loop.
respond: Sends a response message to the client.

3. P2Server
A derived class from TCPServer that handles 2PC messages and state transitions for a participant in the protocol.
Constructor: Initializes the server, reads account information from a file, and opens a log file.
Destructor: Closes the log file.
start_client: Handles the initial client connection.
process: Processes incoming 2PC messages and manages state transitions (INIT, ABORT, READY, COMMIT).

4. Coordinator
A coordinator program that manages the 2PC protocol, communicating with multiple
participants to commit or abort transactions based on their votes.

Usage of participants and Coordinator:
Use this command to run a participant: ./participant <port> <accountFileName> <logFileName>
Use this command to run a coordinator: ./coordinator <logFileName> <amount> <origin_hostname> <origin_port> <origin_account> <destination_hostname> <destination_port> <destination_account>

Error Handling:
The project includes basic error handling for invalid ports, file I/O errors,
and network errors. Each component logs errors and transaction details to the specified log files.