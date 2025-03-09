#include <iostream>
#include <string>
#include <vector>
#include <bitset>

const int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};


const int FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};


const int E[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

// S-boxes
const int S_BOX[8][4][16] = {
    // S1
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    // S2-S8 boxes would go here...
};

const int P[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25
};

class DES {
    private:
        std::vector<std::bitset<48>> subkeys;
    
        std::bitset<32> f(std::bitset<32> R, std::bitset<48> k) {
        
        std::bitset<48> expanded;
        for(int i = 0; i < 48; i++) {
            expanded[47-i] = R[32-E[i]];
        }
        
        expanded ^= k;
        
        std::bitset<32> output;
        int pos = 0;
        for(int i = 0; i < 8; i++) {
            int row = expanded[47-6*i]*2 + expanded[47-6*i-5];
            int col = expanded[47-6*i-1]*8 + expanded[47-6*i-2]*4 + 
                     expanded[47-6*i-3]*2 + expanded[47-6*i-4];
            int val = S_BOX[i][row][col];
            
            for(int j = 0; j < 4; j++) {
                output[31-pos-j] = (val >> j) & 1;
            }
            pos += 4;
        }
        
        
        std::bitset<32> temp = output;
        for(int i = 0; i < 32; i++) {
            output[31-i] = temp[32-P[i]];
        }
        
        return output;
    }

public:
    DES(const std::bitset<64>& key) {
        generateSubkeys(key);
    }
    
    void generateSubkeys(std::bitset<64> key) {
        subkeys.resize(16);
    }
    
    std::bitset<64> encrypt(std::bitset<64> plaintext) {
        
        std::bitset<64> state;
        for(int i = 0; i < 64; i++) {
            state[63-i] = plaintext[64-IP[i]];
        }
        
        std::bitset<32> L;
        std::bitset<32> R;

        for(int i = 0; i < 32; i++) {
            L[i] = state[i + 32];
        }
        
        for(int i = 0; i < 32; i++) {
            R[i] = state[i];
        }
        
        for(int i = 0; i < 16; i++) {
            std::bitset<32> temp = R;
            R = L ^ f(R, subkeys[i]);
            L = temp;
        }
        
        // Combine R16L16 (note the swap)
        for(int i = 0; i < 32; i++) {
            state[i + 32] = R[i];  
            state[i] = L[i];       
        }
        
        std::bitset<64> ciphertext;
        for(int i = 0; i < 64; i++) {
            ciphertext[63-i] = state[64-FP[i]];
        }
        
        return ciphertext;
    }
    
    std::bitset<64> decrypt(std::bitset<64> ciphertext) {
        
        std::vector<std::bitset<48>> reversed_subkeys = subkeys;
        for(int i = 0; i < 8; i++) {
            std::swap(reversed_subkeys[i], reversed_subkeys[15-i]);
        }
        
        
        std::vector<std::bitset<48>> temp_subkeys = subkeys;
        
       
        subkeys = reversed_subkeys;
        std::bitset<64> plaintext = encrypt(ciphertext);  
        
        subkeys = temp_subkeys;
        
        return plaintext;
    }
};

int main() {
    try {
        
        std::string key_str = "133457799BBCDFF1";  
        std::bitset<64> key;
        for(int i = 0; i < 16; i++) {
            char c = key_str[i];
            int val = (c >= '0' && c <= '9') ? (c - '0') : (c - 'A' + 10);
            for(int j = 0; j < 4; j++) {
                key[63 - (i*4 + j)] = (val >> j) & 1;
            }
        }

        // Convert ASCII string to bitset for plaintext
        std::string text = "HelloDES";  // 8 characters (64 bits)
        std::bitset<64> plaintext;
        for(int i = 0; i < 8; i++) {
            char c = text[i];
            for(int j = 0; j < 8; j++) {
                plaintext[63 - (i*8 + j)] = (c >> j) & 1;
            }
        }
        
        DES des(key);
        std::bitset<64> ciphertext = des.encrypt(plaintext);
        std::bitset<64> decrypted = des.decrypt(ciphertext);
        

        std::string original_text;
        for(int i = 0; i < 8; i++) {
            char c = 0;
            for(int j = 0; j < 8; j++) {
                c |= plaintext[63 - (i*8 + j)] << j;
            }
            original_text += c;
        }

        std::cout << "Original text: " << original_text << std::endl;
        std::cout << "Original bits: " << plaintext << std::endl;
        std::cout << "Encrypted: " << ciphertext << std::endl;
        std::cout << "Decrypted: " << decrypted << std::endl;
        
        
        std::string decrypted_text;
        for(int i = 0; i < 8; i++) {
            char c = 0;
            for(int j = 0; j < 8; j++) {
                c |= decrypted[63 - (i*8 + j)] << j;
            }
            decrypted_text += c;
        }
        std::cout << "Decrypted text: " << decrypted_text << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}