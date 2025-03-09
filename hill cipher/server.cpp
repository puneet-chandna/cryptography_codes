#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

const int PORT = 8080;

// Function to find the modulo 26 of a number
int mod26(int x) {
    return (x % 26 + 26) % 26;
}

// Function to perform matrix multiplication modulo 26
vector<vector<int>> multiplyMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> result(A.size(), vector<int>(B[0].size(), 0));
    for (int i = 0; i < A.size(); ++i) {
        for (int j = 0; j < B[0].size(); ++j) {
            for (int k = 0; k < A[0].size(); ++k) {
                result[i][j] = mod26(result[i][j] + A[i][k] * B[k][j]);
            }
        }
    }
    return result;
}

// Hill Cipher Encryption Function
string hillCipherEncrypt(const string& plaintext, const vector<vector<int>>& keyMatrix) {
    string ciphertext = "";
    int n = keyMatrix.size();
    vector<int> plaintextVec;
    for (char c : plaintext) {
        if (isalpha(c)) {
            plaintextVec.push_back(tolower(c) - 'a');
        }
    }

    while (plaintextVec.size() % n != 0) {
        plaintextVec.push_back(0); // padding with 'a' (0)
    }

    for (size_t i = 0; i < plaintextVec.size(); i += n) {
        vector<vector<int>> block(n, vector<int>(1, 0));
        for (int j = 0; j < n; ++j) {
            block[j][0] = plaintextVec[i + j];
        }

        vector<vector<int>> encryptedBlock = multiplyMatrices(keyMatrix, block);

        for (int j = 0; j < n; ++j) {
            ciphertext += (char)(encryptedBlock[j][0] + 'a');
        }
    }

    return ciphertext;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    cout << "Server is running and waiting for connections..." << endl;

    // Keep the server running to accept multiple client connections
    while (true) {
        // Accept incoming connections
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        cout << "Connection accepted. Waiting for data..." << endl;

        char buffer[1024] = {0};
        string plaintext;

        // Read the plaintext message from the client
        int valread = read(new_socket, buffer, 1024);
        if (valread < 0) {
            perror("Failed to read data from client");
            close(new_socket);
            continue;
        }

        plaintext = string(buffer, valread);

        // Print the incoming message from the client
        cout << "Received message: " << plaintext << endl;

        // Example 2x2 key matrix for encryption
        vector<vector<int>> keyMatrix = {
            {6, 24},
            {1, 16}
        };

        // Encrypt the plaintext
        string ciphertext = hillCipherEncrypt(plaintext, keyMatrix);

        // Send the ciphertext back to the client
        send(new_socket, ciphertext.c_str(), ciphertext.length(), 0);

        cout << "Encrypted message sent: " << ciphertext << endl;

        // Close the client connection after processing the message
        close(new_socket);
    }

    // Close the server socket (this line will never be reached in this infinite loop)
    close(server_fd);

    return 0;
}