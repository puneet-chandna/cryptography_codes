#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

// Function to decrypt using Transposition Cipher
std::string decryptTransposition(const std::string &cipherText, int key) {
    int n = cipherText.length();
    int rows = key;
    int cols = (n + rows - 1) / rows;  // Calculate the number of columns

    // Calculate the number of extra padding spaces
    int paddedSpaces = rows * cols - n;

    std::vector<std::string> grid(rows);
    int idx = 0;

    // Fill the grid row by row, account for padding
    for (int i = 0; i < rows; ++i) {
        int charsInRow = (i < rows - paddedSpaces) ? cols : cols - 1;
        for (int j = 0; j < charsInRow && idx < n; ++j) {
            grid[i] += cipherText[idx++];
        }
    }

    // Reconstruct the plaintext by reading column by column
    std::string plainText;
    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            if (i < grid[j].length()) {
                plainText += grid[j][i];
            }
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

    int key = 5;  // Same columnar key used for decryption
    std::string plainText = decryptTransposition(cipherText, key);
    std::cout << "Decrypted message: " << plainText << std::endl;

    close(new_socket);
    close(server_fd);
    return 0;
}
