#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
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

std::string preprocessMessage(const std::string &message) {
    std::string processed = "";
    for (char ch : message) {
        if (ch != ' ') {
            processed += toupper(ch);
        }
    }
    if (processed.length() % 2 != 0) {
        processed += 'X'; // Padding for odd-length plaintext
    }
    return processed;
}

std::string encrypt(const std::string &plaintext, const std::vector<std::string> &matrix) {
    std::string ciphertext = "";
    for (size_t i = 0; i < plaintext.length(); i += 2) {
        char a = plaintext[i], b = plaintext[i + 1];
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
            ciphertext += matrix[row1][(col1 + 1) % 5];
            ciphertext += matrix[row2][(col2 + 1) % 5];
        } else if (col1 == col2) {
            ciphertext += matrix[(row1 + 1) % 5][col1];
            ciphertext += matrix[(row2 + 1) % 5][col2];
        } else {
            ciphertext += matrix[row1][col2];
            ciphertext += matrix[row2][col1];
        }
    }
    return ciphertext;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    std::cout << "Enter the encryption key: ";
    std::string key;
    std::cin >> key;

    std::cin.ignore(); // Clear input buffer

    std::cout << "Enter the message to encrypt and send: ";
    std::string message;
    std::getline(std::cin, message);

    std::string processedMessage = preprocessMessage(message);
    std::vector<std::string> keyMatrix = constructKeyMatrix(key);
    std::string encryptedMessage = encrypt(processedMessage, keyMatrix);

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
