# server.py
import socket
import pickle
import random
from math import gcd

def is_prime(n, k=5):
    """Miller-Rabin primality test"""
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0:
        return False
    
    # Write n as 2^r * d + 1
    r, d = 0, n - 1
    while d % 2 == 0:
        r += 1
        d //= 2
    
    # Witness loop
    for _ in range(k):
        a = random.randint(2, n - 2)
        x = pow(a, d, n)
        if x == 1 or x == n - 1:
            continue
        for _ in range(r - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True

def generate_prime(bits):
    """Generate a prime number with specified number of bits"""
    while True:
        p = random.getrandbits(bits)
        # Ensure the number is odd and has the right bit length
        p |= (1 << bits - 1) | 1
        if is_prime(p):
            return p

def mod_inverse(e, phi):
    """Calculate the modular multiplicative inverse"""
    def extended_gcd(a, b):
        if a == 0:
            return b, 0, 1
        else:
            gcd, x, y = extended_gcd(b % a, a)
            return gcd, y - (b // a) * x, x
    
    gcd, x, y = extended_gcd(e, phi)
    if gcd != 1:
        raise ValueError("Modular inverse does not exist")
    else:
        return x % phi

def generate_keypair(bits=1024):
    """Generate RSA key pair"""
    # Generate two distinct prime numbers
    p = generate_prime(bits // 2)
    q = generate_prime(bits // 2)
    while p == q:
        q = generate_prime(bits // 2)
    
    n = p * q
    phi = (p - 1) * (q - 1)
    
    # Choose e such that 1 < e < phi and gcd(e, phi) = 1
    e = 65537  # Common choice for e
    while gcd(e, phi) != 1:
        e = random.randrange(2, phi)
    
    # Compute d, the modular multiplicative inverse of e (mod phi)
    d = mod_inverse(e, phi)
    
    # Public key: (n, e), Private key: (n, d)
    return ((n, e), (n, d))

def encrypt(public_key, message):
    """Encrypt message using public key"""
    n, e = public_key
    # Convert message to integer
    message_int = int.from_bytes(message.encode(), 'big')
    if message_int >= n:
        raise ValueError("Message too long for key size")
    # Encrypt using modular exponentiation
    ciphertext = pow(message_int, e, n)
    return ciphertext

def decrypt(private_key, ciphertext):
    """Decrypt ciphertext using private key"""
    n, d = private_key
    # Decrypt using modular exponentiation
    message_int = pow(ciphertext, d, n)
    # Convert integer back to bytes and then to string
    message_bytes = message_int.to_bytes((message_int.bit_length() + 7) // 8, 'big')
    return message_bytes.decode('utf-8', errors='ignore')

def main():
    # Generate RSA keys
    public_key, private_key = generate_keypair()
    print(f"Server started with public key: {public_key}")
    
    # Create a socket server
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('localhost', 8000))
    server_socket.listen(1)
    print("Waiting for client connection...")
    
    # Accept client connection
    client_socket, address = server_socket.accept()
    print(f"Connected to client: {address}")
    
    # Send public key to client
    client_socket.send(pickle.dumps(public_key))
    
    try:
        while True:
            # Receive encrypted message from client
            data = client_socket.recv(4096)
            if not data:
                break
            
            encrypted_message = pickle.loads(data)
            print(f"Received encrypted message: {encrypted_message}")
            
            # Decrypt the message
            decrypted_message = decrypt(private_key, encrypted_message)
            print(f"Decrypted message: {decrypted_message}")
            
            # Encrypt and send a response
            response = f"Server received: {decrypted_message}"
            encrypted_response = encrypt(public_key, response)
            client_socket.send(pickle.dumps(encrypted_response))
    
    except Exception as e:
        print(f"Error: {e}")
    
    finally:
        client_socket.close()
        server_socket.close()
        print("Server shutdown")

if __name__ == "__main__":
    main()