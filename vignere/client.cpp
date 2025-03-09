#include <iostream>
#include <string>
#include <cctype>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

// Function to encrypt using Vigenère Cipher
std::string encryptVigenere(const std::string &plainText, const std::string &key) {
    std::string cipherText;
    int keyIndex = 0;
    int keyLength = key.length();

    for (char c : plainText) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            char encryptedChar = ((c - base + toupper(key[keyIndex % keyLength]) - 'A') % 26) + base;
            cipherText += encryptedChar;
            keyIndex++;
        } else {
            cipherText += c;  // Preserve spaces and special characters
        }
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

    std::string key = "SECRETKEY";  // Key for encryption
    std::string cipherText = encryptVigenere(plainText, key);

    int cipherLength = cipherText.size();
    send(sock, &cipherLength, sizeof(cipherLength), 0);
    send(sock, cipherText.c_str(), cipherLength, 0);

    std::cout << "Encrypted message sent: " << cipherText << std::endl;

    close(sock);
    return 0;
}
