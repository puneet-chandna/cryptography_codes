#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

void decryptMessage(const std::string &cipherText, const std::string &key, std::string &plainText) {
    plainText = "";
    for (size_t i = 0; i < cipherText.size(); ++i) {
        plainText += cipherText[i] ^ key[i];
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char keyBuffer[1024] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
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

    // Read ciphertext length
    int cipherLength;
    read(new_socket, &cipherLength, sizeof(cipherLength));

    // Read ciphertext
    read(new_socket, buffer, cipherLength);
    std::string cipherText(buffer, cipherLength);

    // Read key
    read(new_socket, keyBuffer, cipherLength);
    std::string key(keyBuffer, cipherLength);

    // Print received ciphertext
    std::cout << "Received encrypted message: " << cipherText << std::endl;

    // Decrypt and print plaintext
    std::string plainText;
    decryptMessage(cipherText, key, plainText);
    std::cout << "Decrypted message: " << plainText << std::endl;

    close(new_socket);
    close(server_fd);
    return 0;
}
