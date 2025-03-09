#include <iostream>
#include <string>
#include <cctype>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

// Function to decrypt using Vigenère Cipher
std::string decryptVigenere(const std::string &cipherText, const std::string &key) {
    std::string plainText;
    int keyIndex = 0;
    int keyLength = key.length();

    for (char c : cipherText) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            char decryptedChar = ((c - base - (toupper(key[keyIndex % keyLength]) - 'A') + 26) % 26) + base;
            plainText += decryptedChar;
            keyIndex++;
        } else {
            plainText += c;  // Preserve spaces and special characters
        }
    }

    return plainText;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is waiting for a connection...\n";

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    int cipherLength;
    read(new_socket, &cipherLength, sizeof(cipherLength));

    read(new_socket, buffer, cipherLength);
    std::string cipherText(buffer, cipherLength);

    std::cout << "Received encrypted message: " << cipherText << std::endl;

    std::string key = "SECRETKEY";  // Same key used for decryption
    std::string plainText = decryptVigenere(cipherText, key);
    std::cout << "Decrypted message: " << plainText << std::endl;

    close(new_socket);
    close(server_fd);
    return 0;
}
