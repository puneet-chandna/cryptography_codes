#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

const int PORT = 8080;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    string message;
    cout << "Enter message to encrypt: ";
    getline(cin, message);

    // Send message to server
    send(sock, message.c_str(), message.length(), 0);

    char buffer[1024] = {0};

    // Receive the encrypted message from the server
    int valread = read(sock, buffer, 1024);
    cout << "Encrypted message: " << string(buffer, valread) << endl;

    close(sock);
    return 0;
}
