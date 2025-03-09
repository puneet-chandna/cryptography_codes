#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

std::vector<std::string> constructKeyMatrix(const std::string &key) {
    std::string uniqueKey = "";
    std::vector<bool> present(26, false);

    for (char ch : key) {
        ch = toupper(ch);
        if (ch == 'J') ch = 'I';
        if (!present[ch - 'A']) {
            uniqueKey += ch;
            present[ch - 'A'] = true;
        }
    }

    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        if (ch == 'J') continue;
        if (!present[ch - 'A']) {
            uniqueKey += ch;
            present[ch - 'A'] = true;
        }
    }

    std::vector<std::string> matrix(5, "");
    for (int i = 0; i < 25; ++i) {
        matrix[i / 5] += uniqueKey[i];
    }
    return matrix;
}

std::string decrypt(const std::string &ciphertext, const std::vector<std::string> &matrix) {
    std::string plaintext = "";
    for (size_t i = 0; i < ciphertext.length(); i += 2) {
        char a = ciphertext[i], b = ciphertext[i + 1];
        int row1, col1, row2, col2;

        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (matrix[row][col] == a) {
                    row1 = row;
                    col1 = col;
                }
                if (matrix[row][col] == b) {
                    row2 = row;
                    col2 = col;
                }
            }
        }

        if (row1 == row2) {
            plaintext += matrix[row1][(col1 + 4) % 5];
            plaintext += matrix[row2][(col2 + 4) % 5];
        } else if (col1 == col2) {
            plaintext += matrix[(row1 + 4) % 5][col1];
            plaintext += matrix[(row2 + 4) % 5][col2];
        } else {
            plaintext += matrix[row1][col2];
            plaintext += matrix[row2][col1];
        }
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

    std::string key = "PUNEET"; // Same key as client
    std::vector<std::string> matrix = constructKeyMatrix(key);
    std::string decryptedMessage = decrypt(encryptedMessage, matrix);

    std::cout << "Decrypted message: " << decryptedMessage << std::endl;

    close(newSocket);
    close(serverSocket);
    return 0;
}
