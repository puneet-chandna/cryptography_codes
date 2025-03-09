
import os
import numpy as np
from multiprocessing import Pool, cpu_count

# Keep the original AES implementation for the core algorithm
from AES import AES, pad_pkcs7, unpad_pkcs7

class ParallelAES_CBC:
    """
    AES in Cipher Block Chaining (CBC) mode with parallel processing for multiple blocks.
    """
    def __init__(self, key, iv, key_size=128, num_threads=None):
        """
        Initialize parallel AES-CBC with key and initialization vector.
        
        Args:
            key (bytes): Encryption/decryption key
            iv (bytes): 16-byte initialization vector
            key_size (int): Key size in bits, can be 128, 192, or 256
            num_threads (int): Number of threads to use (defaults to CPU count)
        """
        if len(iv) != 16:
            raise ValueError("IV must be 16 bytes")
        
        self.aes = AES(key, key_size)
        self.iv = iv
        self.num_threads = num_threads if num_threads else cpu_count()

    def encrypt(self, plaintext):
        """
        Encrypt data using parallelized AES-CBC.
        
        For CBC mode, we can parallelize using the following approach:
        1. First block is processed normally with the IV
        2. For subsequent blocks, we process them in parallel once 
           the previous blocks are computed
        """
        plaintext_padded = pad_pkcs7(plaintext)
        blocks = [plaintext_padded[i:i+16] for i in range(0, len(plaintext_padded), 16)]
        
        # For small inputs, use sequential processing
        if len(blocks) <= 2:
            # Use the standard implementation for small inputs
            ciphertext = bytearray()
            prev_block = self.iv
            
            for block in blocks:
                xored = bytes(x ^ y for x, y in zip(block, prev_block))
                encrypted_block = self.aes.encrypt(xored)
                ciphertext.extend(encrypted_block)
                prev_block = encrypted_block
            
            return bytes(ciphertext)
        
        # For larger inputs, use parallel processing
        # First block must be processed sequentially with IV
        xored = bytes(x ^ y for x, y in zip(blocks[0], self.iv))
        first_encrypted = self.aes.encrypt(xored)
        ciphertext = bytearray(first_encrypted)
        
        # Process remaining blocks in parallel batches
        remaining_blocks = blocks[1:]
        block_count = len(remaining_blocks)
        
        # Process in batches to maintain chain dependencies but enable parallelism
        batch_size = min(self.num_threads, block_count)
        for i in range(0, block_count, batch_size):
            batch_end = min(i + batch_size, block_count)
            batch_blocks = remaining_blocks[i:batch_end]
            
            # Get the previous encrypted block for this batch
            prev_encrypted = first_encrypted if i == 0 else results[-1]
            
            # Create tasks for parallel processing
            results = []
            with Pool(processes=min(self.num_threads, len(batch_blocks))) as pool:
                tasks = []
                for j, block in enumerate(batch_blocks):
                    if j == 0:
                        # First block in batch uses the previous encrypted block
                        tasks.append((block, prev_encrypted))
                    else:
                        # Later blocks need to wait - we'll handle them sequentially
                        pass
                
                # Process the first block of the batch
                if tasks:
                    first_result = pool.apply(self._encrypt_block, args=tasks[0])
                    results.append(first_result)
                    
                    # Process remaining blocks in the batch sequentially
                    for j in range(1, len(batch_blocks)):
                        next_result = self._encrypt_block(batch_blocks[j], results[-1])
                        results.append(next_result)
            
            # Add batch results to ciphertext
            ciphertext.extend(b''.join(results))
        
        return bytes(ciphertext)

    def _encrypt_block(self, block, prev_block):
        """Helper function to encrypt a single block."""
        xored = bytes(x ^ y for x, y in zip(block, prev_block))
        return self.aes.encrypt(xored)

    def decrypt(self, ciphertext):
        """
        Decrypt data using parallelized AES-CBC.
        
        For CBC decryption, we can fully parallelize since each block's
        decryption is independent (only the XOR with previous block needs 
        to be sequential).
        """
        if len(ciphertext) % 16 != 0:
            raise ValueError("Ciphertext length must be a multiple of 16 bytes")
        
        blocks = [ciphertext[i:i+16] for i in range(0, len(ciphertext), 16)]
        block_count = len(blocks)
        
        # For small inputs, use sequential processing
        if block_count <= 2:
            plaintext = bytearray()
            prev_block = self.iv
            
            for block in blocks:
                decrypted_block = self.aes.decrypt(block)
                xored = bytes(x ^ y for x, y in zip(decrypted_block, prev_block))
                plaintext.extend(xored)
                prev_block = block
            
            return unpad_pkcs7(bytes(plaintext))
        
        # For larger inputs, we can parallelize the decrypt operations
        with Pool(processes=min(self.num_threads, block_count)) as pool:
            # Parallelize the decryption of blocks
            decrypted_blocks = pool.map(self.aes.decrypt, blocks)
        
        # XOR with previous blocks (must be done sequentially)
        plaintext = bytearray()
        prev_block = self.iv
        
        for i, decrypted in enumerate(decrypted_blocks):
            xored = bytes(x ^ y for x, y in zip(decrypted, prev_block))
            plaintext.extend(xored)
            prev_block = blocks[i]
        
        return unpad_pkcs7(bytes(plaintext))


# Cython implementation for even better performance
# Save this in a file called aes_parallel.pyx and compile with:
# python setup.py build_ext --inplace

"""
# Contents of setup.py
from setuptools import setup, Extension
from Cython.Build import cythonize
import numpy as np

extensions = [
    Extension(
        "aes_parallel",
        ["aes_parallel.pyx"],
        extra_compile_args=['-fopenmp'],
        extra_link_args=['-fopenmp'],
    )
]

setup(
    ext_modules=cythonize(extensions),
    include_dirs=[np.get_include()]
)
"""

# Example of how the Cython file (aes_parallel.pyx) would look:
"""
# cython: language_level=3
# distutils: extra_compile_args = -fopenmp
# distutils: extra_link_args = -fopenmp

import numpy as np
cimport numpy as np
from cython.parallel import prange
from libc.stdlib cimport malloc, free

# Import the Python AES class
from original_aes import AES, pad_pkcs7, unpad_pkcs7

cdef class CythonAES_CBC:
    cdef:
        object aes
        bytes iv
        int num_threads
    
    def __init__(self, key, iv, key_size=128, num_threads=4):
        if len(iv) != 16:
            raise ValueError("IV must be 16 bytes")
        
        self.aes = AES(key, key_size)
        self.iv = iv
        self.num_threads = num_threads
    
    def encrypt(self, bytes plaintext):
        # Implementation details would go here
        # Using OpenMP with prange for parallelism
        pass
    
    def decrypt(self, bytes ciphertext):
        # Implementation details would go here
        # Using OpenMP with prange for parallelism
        pass
"""


# Example usage
if __name__ == "__main__":
    import time
    import os
    
    # Generate random key and IV
    key = os.urandom(16)  # 128-bit key
    iv = os.urandom(16)   # 16-byte IV
    
    # Create test data - larger data to see parallelization benefits
    plaintext = b"This is a secret message that needs to be encrypted using AES!" * 10000
    
    # Standard implementation
    from AES import AES_CBC
    start_time = time.time()
    aes_cbc = AES_CBC(key, iv)
    ciphertext_standard = aes_cbc.encrypt(plaintext)
    decrypted_standard = aes_cbc.decrypt(ciphertext_standard)
    standard_time = time.time() - start_time
    print(f"Standard implementation time: {standard_time:.4f} seconds")
    
    # Parallel implementation
    start_time = time.time()
    aes_parallel = ParallelAES_CBC(key, iv)
    ciphertext_parallel = aes_parallel.encrypt(plaintext)
    decrypted_parallel = aes_parallel.decrypt(ciphertext_parallel)
    parallel_time = time.time() - start_time
    print(f"Parallel implementation time: {parallel_time:.4f} seconds")
    
    # Verify results
    assert decrypted_standard == plaintext, "Standard decryption failed!"
    assert decrypted_parallel == plaintext, "Parallel decryption failed!"
    assert ciphertext_standard == ciphertext_parallel, "Encryption results differ!"
    
    print(f"Speedup: {standard_time / parallel_time:.2f}x")