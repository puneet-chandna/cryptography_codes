

class AES:
    
    # S-box and Inverse S-box (S is for Substitution)
    sbox = [
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    ]

    inv_sbox = [
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
    ]

    # Rijndael Rcon table for key expansion
    rcon = [
        0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
        0x80, 0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D, 0x9A,
        0x2F, 0x5E, 0xBC, 0x63, 0xC6, 0x97, 0x35, 0x6A,
        0xD4, 0xB3, 0x7D, 0xFA, 0xEF, 0xC5, 0x91, 0x39
    ]

    def __init__(self, key, key_size=128):
        """
        Initialize AES with the given key and key size.
        
        Args:
            key (bytes): The encryption/decryption key
            key_size (int): Key size in bits, can be 128, 192, or 256
        """
        self.key = key
        self.key_size = key_size
        
        # AES parameters based on key size
        if key_size == 128:
            self.rounds = 10
            self.key_words = 4
        elif key_size == 192:
            self.rounds = 12
            self.key_words = 6
        elif key_size == 256:
            self.rounds = 14
            self.key_words = 8
        else:
            raise ValueError("Key size must be 128, 192, or 256 bits")
        
        # Ensure key length matches key_size
        if len(key) * 8 != key_size:
            raise ValueError(f"Key length should be {key_size // 8} bytes")
        
        # Generate round keys
        self.round_keys = self._key_expansion(key)

    def encrypt(self, plaintext):
        """
        Encrypt a 16-byte block of plaintext using AES.
        
        Args:
            plaintext (bytes): 16-byte plaintext block
        
        Returns:
            bytes: 16-byte ciphertext block
        """
        if len(plaintext) != 16:
            raise ValueError("Plaintext block must be 16 bytes")
        
        # Convert plaintext to state matrix (4x4 array of bytes)
        state = [list(plaintext[i:i+4]) for i in range(0, 16, 4)]
        state = [list(row) for row in zip(*state)]  # Transpose
        
        # Initial round key addition
        state = self._add_round_key(state, self.round_keys[:4])
        
        # Main rounds
        for round_num in range(1, self.rounds):
            state = self._sub_bytes(state)
            state = self._shift_rows(state)
            state = self._mix_columns(state)
            state = self._add_round_key(state, self.round_keys[4*round_num:4*(round_num+1)])
        
        # Final round (no mix columns)
        state = self._sub_bytes(state)
        state = self._shift_rows(state)
        state = self._add_round_key(state, self.round_keys[4*self.rounds:4*(self.rounds+1)])
        
        # Convert state matrix back to bytes
        state = [list(row) for row in zip(*state)]  # Transpose back
        return bytes(sum(state, []))

    def decrypt(self, ciphertext):
        """
        Decrypt a 16-byte block of ciphertext using AES.
        
        Args:
            ciphertext (bytes): 16-byte ciphertext block
        
        Returns:
            bytes: 16-byte plaintext block
        """
        if len(ciphertext) != 16:
            raise ValueError("Ciphertext block must be 16 bytes")
        
        # Convert ciphertext to state matrix (4x4 array of bytes)
        state = [list(ciphertext[i:i+4]) for i in range(0, 16, 4)]
        state = [list(row) for row in zip(*state)]  # Transpose
        
        # Initial round key addition
        state = self._add_round_key(state, self.round_keys[4*self.rounds:4*(self.rounds+1)])
        
        # Main rounds
        for round_num in range(self.rounds-1, 0, -1):
            state = self._inv_shift_rows(state)
            state = self._inv_sub_bytes(state)
            state = self._add_round_key(state, self.round_keys[4*round_num:4*(round_num+1)])
            state = self._inv_mix_columns(state)
        
        # Final round (no mix columns)
        state = self._inv_shift_rows(state)
        state = self._inv_sub_bytes(state)
        state = self._add_round_key(state, self.round_keys[:4])
        
        # Convert state matrix back to bytes
        state = [list(row) for row in zip(*state)]  # Transpose back
        return bytes(sum(state, []))

    def _sub_bytes(self, state):
        """Apply S-box substitution to each byte of the state."""
        return [[self.sbox[byte] for byte in row] for row in state]

    def _inv_sub_bytes(self, state):
        """Apply inverse S-box substitution to each byte of the state."""
        return [[self.inv_sbox[byte] for byte in row] for row in state]

    def _shift_rows(self, state):
        """Shift rows of state matrix: no shift for row 0, shift left 1 for row 1, etc."""
        return [
            state[0],
            state[1][1:] + state[1][:1],
            state[2][2:] + state[2][:2],
            state[3][3:] + state[3][:3]
        ]

    def _inv_shift_rows(self, state):
        """Inverse of _shift_rows: shift rows of state matrix to the right."""
        return [
            state[0],
            state[1][-1:] + state[1][:-1],
            state[2][-2:] + state[2][:-2],
            state[3][-3:] + state[3][:-3]
        ]

    def _mix_columns(self, state):
        """Mix columns using Galois Field multiplication."""
        def galois_mult(a, b):
            p = 0
            for _ in range(8):
                if b & 1:
                    p ^= a
                high_bit = a & 0x80
                a <<= 1
                if high_bit:
                    a ^= 0x1B  # XOR with irreducible polynomial x^8 + x^4 + x^3 + x + 1
                b >>= 1
            return p & 0xFF

        new_state = [[] for _ in range(4)]
        
        for i in range(4):
            for j in range(4):
                new_state[i].append(
                    galois_mult(0x02, state[i][0]) ^
                    galois_mult(0x03, state[i][1]) ^
                    state[i][2] ^
                    state[i][3]
                )
                new_state[i].append(
                    state[i][0] ^
                    galois_mult(0x02, state[i][1]) ^
                    galois_mult(0x03, state[i][2]) ^
                    state[i][3]
                )
                new_state[i].append(
                    state[i][0] ^
                    state[i][1] ^
                    galois_mult(0x02, state[i][2]) ^
                    galois_mult(0x03, state[i][3])
                )
                new_state[i].append(
                    galois_mult(0x03, state[i][0]) ^
                    state[i][1] ^
                    state[i][2] ^
                    galois_mult(0x02, state[i][3])
                )
        
        return new_state

    def _inv_mix_columns(self, state):
        """Inverse of _mix_columns."""
        def galois_mult(a, b):
            p = 0
            for _ in range(8):
                if b & 1:
                    p ^= a
                high_bit = a & 0x80
                a <<= 1
                if high_bit:
                    a ^= 0x1B
                b >>= 1
            return p & 0xFF

        new_state = [[] for _ in range(4)]
        
        for i in range(4):
            for j in range(4):
                new_state[i].append(
                    galois_mult(0x0E, state[i][0]) ^
                    galois_mult(0x0B, state[i][1]) ^
                    galois_mult(0x0D, state[i][2]) ^
                    galois_mult(0x09, state[i][3])
                )
                new_state[i].append(
                    galois_mult(0x09, state[i][0]) ^
                    galois_mult(0x0E, state[i][1]) ^
                    galois_mult(0x0B, state[i][2]) ^
                    galois_mult(0x0D, state[i][3])
                )
                new_state[i].append(
                    galois_mult(0x0D, state[i][0]) ^
                    galois_mult(0x09, state[i][1]) ^
                    galois_mult(0x0E, state[i][2]) ^
                    galois_mult(0x0B, state[i][3])
                )
                new_state[i].append(
                    galois_mult(0x0B, state[i][0]) ^
                    galois_mult(0x0D, state[i][1]) ^
                    galois_mult(0x09, state[i][2]) ^
                    galois_mult(0x0E, state[i][3])
                )
        
        return new_state

    def _add_round_key(self, state, round_key):
        """XOR state with round key."""
        new_state = []
        for i in range(4):
            row = []
            for j in range(4):
                row.append(state[i][j] ^ round_key[j][i])
            new_state.append(row)
        return new_state

    def _key_expansion(self, key):
        """
        Expand the key into round keys.
        
        Args:
            key (bytes): The encryption/decryption key
        
        Returns:
            list: Round keys as a list of 4x4 word matrices
        """
        # Convert key to a list of words (4 bytes each)
        key_words = []
        for i in range(0, len(key), 4):
            word = [key[i], key[i+1], key[i+2], key[i+3]]
            key_words.append(word)
        
        # Expand key to get round keys
        for i in range(self.key_words, 4 * (self.rounds + 1)):
            temp = key_words[i-1].copy()
            
            if i % self.key_words == 0:
                # Rotate word
                temp = temp[1:] + temp[:1]
                # SubBytes
                temp = [self.sbox[b] for b in temp]
                # XOR with Rcon
                temp[0] ^= self.rcon[i // self.key_words]
            elif self.key_words > 6 and i % self.key_words == 4:
                # Additional SubBytes for 256-bit keys
                temp = [self.sbox[b] for b in temp]
            
            # XOR with word self.key_words positions earlier
            word = []
            for j in range(4):
                word.append(key_words[i-self.key_words][j] ^ temp[j])
            
            key_words.append(word)
        
        # Convert to 4x4 matrices for each round
        round_keys = []
        for i in range(0, len(key_words), 4):
            round_key = [[] for _ in range(4)]
            for j in range(4):
                if i+j < len(key_words):
                    for k in range(4):
                        round_key[k].append(key_words[i+j][k])
            if len(round_key[0]) == 4:  # Only add complete round keys
                round_keys.extend(round_key)
        
        return round_keys


# Utility functions for padding and mode of operation support

def pad_pkcs7(data, block_size=16):
    """
    Pad data using PKCS#7 padding.
    
    Args:
        data (bytes): Data to pad
        block_size (int): Block size in bytes (default: 16 for AES)
    
    Returns:
        bytes: Padded data
    """
    padding_length = block_size - (len(data) % block_size)
    padding = bytes([padding_length] * padding_length)
    return data + padding

def unpad_pkcs7(data):
    """
    Remove PKCS#7 padding.
    
    Args:
        data (bytes): Padded data
    
    Returns:
        bytes: Data with padding removed
    """
    padding_length = data[-1]
    if padding_length > len(data):
        raise ValueError("Invalid padding")
    for i in range(1, padding_length + 1):
        if data[-i] != padding_length:
            raise ValueError("Invalid padding")
    return data[:-padding_length]

class AES_CBC:
    """
    AES in Cipher Block Chaining (CBC) mode.
    """
    def __init__(self, key, iv, key_size=128):
        """
        Initialize AES-CBC with key and initialization vector.
        
        Args:
            key (bytes): Encryption/decryption key
            iv (bytes): 16-byte initialization vector
            key_size (int): Key size in bits, can be 128, 192, or 256
        """
        if len(iv) != 16:
            raise ValueError("IV must be 16 bytes")
        
        self.aes = AES(key, key_size)
        self.iv = iv

    def encrypt(self, plaintext):
        """
        Encrypt data using AES-CBC.
        
        Args:
            plaintext (bytes): Data to encrypt
        
        Returns:
            bytes: Encrypted data
        """
        plaintext = pad_pkcs7(plaintext)
        blocks = [plaintext[i:i+16] for i in range(0, len(plaintext), 16)]
        
        ciphertext = bytearray()
        prev_block = self.iv
        
        for block in blocks:
            # XOR with previous ciphertext block (or IV for first block)
            xored = bytes(x ^ y for x, y in zip(block, prev_block))
            # Encrypt
            encrypted_block = self.aes.encrypt(xored)
            # Add to result
            ciphertext.extend(encrypted_block)
            # Update previous block
            prev_block = encrypted_block
        
        return bytes(ciphertext)

    def decrypt(self, ciphertext):
        """
        Decrypt data using AES-CBC.
        
        Args:
            ciphertext (bytes): Data to decrypt
        
        Returns:
            bytes: Decrypted data
        """
        if len(ciphertext) % 16 != 0:
            raise ValueError("Ciphertext length must be a multiple of 16 bytes")
        
        blocks = [ciphertext[i:i+16] for i in range(0, len(ciphertext), 16)]
        
        plaintext = bytearray()
        prev_block = self.iv
        
        for block in blocks:
            # Decrypt
            decrypted_block = self.aes.decrypt(block)
            # XOR with previous ciphertext block (or IV for first block)
            xored = bytes(x ^ y for x, y in zip(decrypted_block, prev_block))
            # Add to result
            plaintext.extend(xored)
            # Update previous block
            prev_block = block
        
        return unpad_pkcs7(bytes(plaintext))


# Example usage
if __name__ == "__main__":
    import os
    
    # Generate random key and IV
    key = os.urandom(16)  # 128-bit key
    iv = os.urandom(16)   # 16-byte IV
    
    # Create AES-CBC instance
    aes_cbc = AES_CBC(key, iv)
    
    # Example plaintext
    plaintext = b"This is a secret message that needs to be encrypted using AES!"
    
    # Encrypt
    ciphertext = aes_cbc.encrypt(plaintext)
    print("Ciphertext:", ciphertext.hex())
    
    # Decrypt
    decrypted = aes_cbc.decrypt(ciphertext)
    print("Decrypted:", decrypted.decode())
    
    # Verify
    assert decrypted == plaintext, "Decryption failed!"
    print("Encryption and decryption successful!")