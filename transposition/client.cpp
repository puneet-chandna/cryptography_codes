#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

// Function to encrypt using Transposition Cipher
std::string encryptTransposition(const std::string &plainText, int key) {
    std::vector<std::string> rows(key);
    int n = plainText.length();

    // Fill rows in a columnar way
    for (int i = 0; i < n; ++i) {
        rows[i % key] += plainText[i];
    }

    // Concatenate all rows to form the cipher text
    std::string cipherText;
    for (const auto &row : rows) {
        cipherText += row;
    }

    return cipherText;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    std::string plainText;
    std::cout << "Enter the message to encrypt and send: ";
    std::getline(std::cin, plainText);

    int key = 5;  // Columnar key
    std::string cipherText = encryptTransposition(plainText, key);

    int cipherLength = cipherText.size();
    send(sock, &cipherLength, sizeof(cipherLength), 0);
    send(sock, cipherText.c_str(), cipherLength, 0);

    std::cout << "Encrypted message sent: " << cipherText << std::endl;

    close(sock);
    return 0;
}
