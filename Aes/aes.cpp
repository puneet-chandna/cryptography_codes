#include <iostream>
#include <iomanip>
#include <cstdint>
#include <algorithm>  // for std::swap
#include <string>

using namespace std;

typedef uint8_t State[4][4];

// --- AES S-boxes and Rcon values ---
const uint8_t SBOX[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
    0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
    0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
    0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
    0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
    0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
    0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
    0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
    0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
    0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
    0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
    0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
    0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
    0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
    0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
    0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
    0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

const uint8_t INV_SBOX[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
    0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
    0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D,
    0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2,
    0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA,
    0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A,
    0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
    0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA,
    0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85,
    0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
    0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20,
    0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31,
    0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
    0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0,
    0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26,
    0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

const uint32_t Rcon[10] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000
};
hii bhootni k


// --- Helper Functions for AES ---
uint32_t RotWord(uint32_t word) {
    return (word << 8) | (word >> 24);
}

uint32_t SubWord(uint32_t word) {
    return (SBOX[(word >> 24) & 0xFF] << 24) |
           (SBOX[(word >> 16) & 0xFF] << 16) |
           (SBOX[(word >> 8) & 0xFF] << 8) |
           (SBOX[word & 0xFF]);
}

void KeyExpansion(const uint8_t key[16], uint32_t w[44]) {
    for (int i = 0; i < 4; i++) {
        w[i] = ((uint32_t)key[4*i] << 24) |
               ((uint32_t)key[4*i+1] << 16) |
               ((uint32_t)key[4*i+2] << 8) |
               ((uint32_t)key[4*i+3]);
    }

    for (int i = 4; i < 44; i++) {
        uint32_t temp = w[i - 1];
        if (i % 4 == 0) {
            temp = SubWord(RotWord(temp)) ^ Rcon[(i/4) - 1];
        }
        w[i] = w[i - 4] ^ temp;
    }
}

void SubBytes(State state) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state[i][j] = SBOX[state[i][j]];
}

void InvSubBytes(State state) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state[i][j] = INV_SBOX[state[i][j]];
}

void ShiftRows(State state) {
    // Row 0: no shift
    // Row 1: left shift by 1
    uint8_t temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;
    
    // Row 2: left shift by 2
    swap(state[2][0], state[2][2]);
    swap(state[2][1], state[2][3]);
    
    // Row 3: left shift by 3 (or right shift by 1)
    temp = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp;
}

void InvShiftRows(State state) {
    // Row 0: no shift
    // Row 1: right shift by 1
    uint8_t temp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = temp;

    swap(state[2][0], state[2][2]);
    swap(state[2][1], state[2][3]);
    
    // Row 3: right shift by 3 (or left shift by 1)
    temp = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = temp;
}

uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1)
            p ^= a;
        uint8_t carry = a & 0x80;
        a <<= 1;
        if (carry)
            a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

void MixColumns(State state) {
    for (int i = 0; i < 4; i++) {
        uint8_t a[4], b[4];
        for (int j = 0; j < 4; j++)
            a[j] = state[j][i];
        
        b[0] = gmul(a[0], 0x02) ^ gmul(a[1], 0x03) ^ a[2] ^ a[3];
        b[1] = a[0] ^ gmul(a[1], 0x02) ^ gmul(a[2], 0x03) ^ a[3];
        b[2] = a[0] ^ a[1] ^ gmul(a[2], 0x02) ^ gmul(a[3], 0x03);
        b[3] = gmul(a[0], 0x03) ^ a[1] ^ a[2] ^ gmul(a[3], 0x02);
        
        for (int j = 0; j < 4; j++)
            state[j][i] = b[j];
    }
}

void InvMixColumns(State state) {
    for (int i = 0; i < 4; i++) {
        uint8_t a[4], b[4];
        for (int j = 0; j < 4; j++)
            a[j] = state[j][i];
        
        b[0] = gmul(a[0], 0x0e) ^ gmul(a[1], 0x0b) ^ gmul(a[2], 0x0d) ^ gmul(a[3], 0x09);
        b[1] = gmul(a[0], 0x09) ^ gmul(a[1], 0x0e) ^ gmul(a[2], 0x0b) ^ gmul(a[3], 0x0d);
        b[2] = gmul(a[0], 0x0d) ^ gmul(a[1], 0x09) ^ gmul(a[2], 0x0e) ^ gmul(a[3], 0x0b);
        b[3] = gmul(a[0], 0x0b) ^ gmul(a[1], 0x0d) ^ gmul(a[2], 0x09) ^ gmul(a[3], 0x0e);
        
        for (int j = 0; j < 4; j++)
            state[j][i] = b[j];
    }
}

void AddRoundKey(State state, const uint32_t w[44], int round) {
    for (int i = 0; i < 4; i++) {
        uint32_t word = w[round * 4 + i];
        state[0][i] ^= (word >> 24) & 0xFF;
        state[1][i] ^= (word >> 16) & 0xFF;
        state[2][i] ^= (word >> 8) & 0xFF;
        state[3][i] ^= word & 0xFF;
    }
}

void AESEncrypt(uint8_t in[16], uint8_t out[16], const uint8_t key[16]) {
    State state;
    // Copy input into state in column-major order.
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            state[r][c] = in[c*4 + r];
    
    uint32_t w[44];
    KeyExpansion(key, w);
    
    // Initial round key addition.
    AddRoundKey(state, w, 0);
    
    // 9 main rounds.
    for (int round = 1; round < 10; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, w, round);
    }
    
    // Final round (without MixColumns).
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, w, 10);
    
    // Copy the state to the output array.
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            out[c*4 + r] = state[r][c];
}

void AESDecrypt(uint8_t in[16], uint8_t out[16], const uint8_t key[16]) {
    State state;
    // Copy input into state.
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            state[r][c] = in[c*4 + r];
    
    uint32_t w[44];
    KeyExpansion(key, w);
    
    // Initial round key addition using the final round key.
    AddRoundKey(state, w, 10);
    
    // 9 main rounds.
    for (int round = 9; round >= 1; round--) {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(state, w, round);
        InvMixColumns(state);
    }
    
    // Final round.
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state, w, 0);
    
    // Copy the state to the output array.
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            out[c*4 + r] = state[r][c];
}

// --- Base64 Encoding Function ---
// This function converts a byte array into a Base64-encoded string.
std::string base64_encode(const uint8_t* data, size_t len) {
    static const char* base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    std::string encoded;
    int val = 0, valb = -6;
    for (size_t i = 0; i < len; i++) {
        val = (val << 8) + data[i];
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (encoded.size() % 4)
        encoded.push_back('=');
    return encoded;
}

int main() {
    // Define a 16-character plaintext message.
    // (Make sure the message length is exactly 16 bytes.)
    uint8_t original[16] = {
        'A', 't', 't', 'a', 'c', 'k', ' ', 'a',
        't', ' ', 'd', 'a', 'w', 'n', '!', '!'
    };

    // Use the same 16-byte key (given here in hexadecimal).
    uint8_t key[16] = {
         0x2B, 0x7E, 0x15, 0x16,
         0x28, 0xAE, 0xD2, 0xA6,
         0xAB, 0xF7, 0x15, 0x88,
         0x09, 0xCF, 0x4F, 0x3C
    };

    uint8_t encrypted[16];
    uint8_t decrypted[16];

    // Encrypt the data.
    AESEncrypt(original, encrypted, key);
    // Decrypt the data.
    AESDecrypt(encrypted, decrypted, key);

    // Convert arrays to strings.
    // For original and decrypted data, we know they are plain text.
    std::string origText(reinterpret_cast<char*>(original), 16);
    std::string decrText(reinterpret_cast<char*>(decrypted), 16);
    // For encrypted data, we convert to Base64 so the output is printable.
    std::string encryptedBase64 = base64_encode(encrypted, 16);

    cout << "Original Data: " << origText << endl;
    cout << "Encrypted Data (Base64): " << encryptedBase64 << endl;
    cout << "Decrypted Data: " << decrText << endl;

    return 0;
}
