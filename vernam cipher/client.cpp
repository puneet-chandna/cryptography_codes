#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void encryptMessage(const std::string &plainText, const std::string &key, std::string &cipherText) {
    cipherText = "";
    for (size_t i = 0; i < plainText.size(); ++i) {
        cipherText += plainText[i] ^ key[i];
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Connect to the server
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    // Get message from user
    std::string plainText;
    std::cout << "Enter the message to encrypt and send: ";
    std::getline(std::cin, plainText);

    // Generate a key of the same length as the message
    std::string key = "SomeRandomKey12345"; // Ensure key is long enough
    key = key.substr(0, plainText.size());

    // Encrypt the message
    std::string cipherText;
    encryptMessage(plainText, key, cipherText);

    // Send ciphertext length
    int cipherLength = cipherText.size();
    send(sock, &cipherLength, sizeof(cipherLength), 0);

    // Send ciphertext
    send(sock, cipherText.c_str(), cipherLength, 0);

    // Send key
    send(sock, key.c_str(), cipherLength, 0);

    std::cout << "Encrypted message sent: " << cipherText << std::endl;

    close(sock);
    return 0;
}
