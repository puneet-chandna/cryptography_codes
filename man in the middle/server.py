# server.py - Diffie-Hellman server implementation

import random
import socket

# Import from common.py
from common import Colors, generate_key, compute_shared_secret, derive_session_key
from common import encrypt_message, decrypt_message, send_data, receive_data

def run_server(port=5000, simulate_mitm=False):
    """Run the Diffie-Hellman server"""
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server_socket.bind(('localhost', port))
        server_socket.listen(1)
        
        print(f"{Colors.GREEN}Server started on port {port}{Colors.ENDC}")
        print(f"{Colors.GREEN}Waiting for client connection...{Colors.ENDC}")
        
        client_socket, client_address = server_socket.accept()
        print(f"{Colors.GREEN}Client connected from {client_address}{Colors.ENDC}")
        
        # Diffie-Hellman parameters
        p = 23  # Small prime for demonstration
        g = 5   # Generator
        
        # Generate server's private key
        server_private = random.randint(1, p-1)
        server_public = generate_key(g, p, server_private)
        
        print(f"{Colors.GREEN}Server generated private key: {server_private}{Colors.ENDC}")
        print(f"{Colors.GREEN}Server calculated public key: {server_public}{Colors.ENDC}")
        
        # Send public parameters and server's public key
        send_data(client_socket, {
            "p": p,
            "g": g,
            "public_key": server_public
        })
        
        # Receive client's public key
        data = receive_data(client_socket)
        if not data or "public_key" not in data:
            print(f"{Colors.RED}Error: Invalid data received from client{Colors.ENDC}")
            return
        
        client_public = data["public_key"]
        print(f"{Colors.GREEN}Received client's public key: {client_public}{Colors.ENDC}")
        
        # Compute shared secret
        shared_secret = compute_shared_secret(client_public, server_private, p)
        print(f"{Colors.GREEN}Computed shared secret: {shared_secret}{Colors.ENDC}")
        
        # Derive session key
        session_key = derive_session_key(shared_secret)
        print(f"{Colors.GREEN}Derived session key: {session_key}{Colors.ENDC}")
        
        # Wait for encrypted messages from client
        while True:
            data = receive_data(client_socket)
            if not data or "encrypted" not in data:
                break
            
            encrypted = data["encrypted"]
            decrypted = decrypt_message(encrypted, session_key)
            
            print(f"{Colors.GREEN}Received encrypted: '{encrypted}'{Colors.ENDC}")
            print(f"{Colors.GREEN}Decrypted message: '{decrypted}'{Colors.ENDC}")
            
            # Send a response back
            response = f"Server received: {decrypted}"
            encrypted_response = encrypt_message(response, session_key)
            
            send_data(client_socket, {
                "encrypted": encrypted_response
            })
            
    except Exception as e:
        print(f"{Colors.RED}Server error: {e}{Colors.ENDC}")
    finally:
        server_socket.close()

if __name__ == "__main__":
    import sys
    port = 5000
    if len(sys.argv) > 1:
        port = int(sys.argv[1])
    run_server(port)