#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

std::string decryptRailfence(const std::string &ciphertext, int key) {
    if (key <= 1) return ciphertext;

    std::vector<int> pos(ciphertext.size());
    std::vector<int> rowCounter(key, 0);

    // Determine the zigzag pattern
    int dir = 1, row = 0;
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        pos[i] = row;
        rowCounter[row]++;
        row += dir;
        if (row == 0 || row == key - 1)
            dir = -dir;
    }

    // Fill the rails with ciphertext
    std::vector<std::string> rail(key);
    size_t index = 0;
    for (int r = 0; r < key; ++r) {
        rail[r] = ciphertext.substr(index, rowCounter[r]);
        index += rowCounter[r];
    }

    // Read off the plaintext
    std::string plaintext;
    row = 0;
    dir = 1;
    std::vector<size_t> currentPos(key, 0);
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        plaintext += rail[row][currentPos[row]++];
        row += dir;
        if (row == 0 || row == key - 1)
            dir = -dir;
    }
    return plaintext;
}

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024] = {0};

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed!" << std::endl;
        return -1;
    }

    if (listen(serverSocket, 1) < 0) {
        std::cerr << "Listen failed!" << std::endl;
        return -1;
    }

    std::cout << "Server is waiting for a connection..." << std::endl;
    newSocket = accept(serverSocket, NULL, NULL);
    if (newSocket < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return -1;
    }

    int valread = read(newSocket, buffer, 1024);
    std::string encryptedMessage(buffer, valread);
    std::cout << "Received encrypted message: " << encryptedMessage << std::endl;

    std::cout << "Enter the decryption key (number of rails): ";
    int key;
    std::cin >> key;

    std::string decryptedMessage = decryptRailfence(encryptedMessage, key);
    std::cout << "Decrypted message: " << decryptedMessage << std::endl;

    close(newSocket);
    close(serverSocket);
    return 0;
}
