#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

std::string encryptRailfence(const std::string &plaintext, int key) {
    if (key <= 1) return plaintext;

    std::vector<std::string> rail(key);
    int dir = 1, row = 0;

    for (char ch : plaintext) {
        rail[row] += ch;
        row += dir;

        if (row == 0 || row == key - 1)
            dir = -dir; // Change direction
    }

    std::string ciphertext;
    for (const auto &r : rail) {
        ciphertext += r;
    }
    return ciphertext;
}

std::string preprocessMessage(const std::string &message) {
    std::string processed;
    for (char ch : message) {
        if (ch != ' ') {
            processed += toupper(ch);
        }
    }
    return processed;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    std::cout << "Enter the encryption key (number of rails): ";
    int key;
    std::cin >> key;

    std::cin.ignore(); // Clear input buffer

    std::cout << "Enter the message to encrypt and send: ";
    std::string message;
    std::getline(std::cin, message);

    std::string processedMessage = preprocessMessage(message);
    std::string encryptedMessage = encryptRailfence(processedMessage, key);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error!" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported!" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return -1;
    }

    send(sock, encryptedMessage.c_str(), encryptedMessage.size(), 0);
    std::cout << "Encrypted message sent: " << encryptedMessage << std::endl;

    close(sock);
    return 0;
}
