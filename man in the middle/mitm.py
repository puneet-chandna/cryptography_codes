# mitm.py - Man-in-the-Middle attack implementation

import random
import hashlib
import socket
import json
import time

# Import from common.py
from common import Colors, generate_key, compute_shared_secret, derive_session_key
from common import encrypt_message, decrypt_message, send_data, receive_data

def run_mitm_attack(client_port=5000, server_port=5001):
    """Run a Man-in-the-Middle attack on Diffie-Hellman key exchange"""
    # Socket to listen for client connections
    mitm_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    mitm_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    # Socket to connect to the real server
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        # Bind to the client-facing port
        mitm_socket.bind(('localhost', client_port))
        mitm_socket.listen(1)
        
        print(f"{Colors.RED}MITM attack started - listening on port {client_port}{Colors.ENDC}")
        print(f"{Colors.RED}Will forward to server on port {server_port}{Colors.ENDC}")
        
        # Wait for client connection
        client_socket, client_address = mitm_socket.accept()
        print(f"{Colors.RED}Client connected from {client_address}{Colors.ENDC}")
        
        # Connect to the real server
        print(f"{Colors.RED}Connecting to real server on port {server_port}{Colors.ENDC}")
        server_socket.connect(('localhost', server_port))
        
        # Diffie-Hellman parameters - will be received from real server
        p = None
        g = None
        
        # Generate MITM private keys
        mitm_private_for_client = random.randint(1, 22)  # p-1 (assuming p=23)
        mitm_private_for_server = random.randint(1, 22)  # p-1 (assuming p=23)
        
        print(f"{Colors.RED}MITM generated private key for client: {mitm_private_for_client}{Colors.ENDC}")
        print(f"{Colors.RED}MITM generated private key for server: {mitm_private_for_server}{Colors.ENDC}")
        
        # Receive DH parameters and server's public key
        data = receive_data(server_socket)
        if not data or "p" not in data or "g" not in data or "public_key" not in data:
            print(f"{Colors.RED}Error: Invalid data received from server{Colors.ENDC}")
            return
        
        p = data["p"]
        g = data["g"]
        server_public = data["public_key"]
        
        print(f"{Colors.RED}Received parameters from server: p={p}, g={g}{Colors.ENDC}")
        print(f"{Colors.RED}Received server's public key: {server_public}{Colors.ENDC}")
        
        # Generate MITM's public key for client (to impersonate server)
        mitm_public_for_client = generate_key(g, p, mitm_private_for_client)
        print(f"{Colors.RED}Generated public key for client: {mitm_public_for_client}{Colors.ENDC}")
        
        # Forward modified parameters to client (replace server's public key with MITM's)
        send_data(client_socket, {
            "p": p,
            "g": g,
            "public_key": mitm_public_for_client  # Send MITM's key instead
        })
        
        # Receive client's public key
        data = receive_data(client_socket)
        if not data or "public_key" not in data:
            print(f"{Colors.RED}Error: Invalid data received from client{Colors.ENDC}")
            return
        
        client_public = data["public_key"]
        print(f"{Colors.RED}Received client's public key: {client_public}{Colors.ENDC}")
        
        # Generate MITM's public key for server (to impersonate client)
        mitm_public_for_server = generate_key(g, p, mitm_private_for_server)
        print(f"{Colors.RED}Generated public key for server: {mitm_public_for_server}{Colors.ENDC}")
        
        # Forward modified client public key to server
        send_data(server_socket, {
            "public_key": mitm_public_for_server  # Send MITM's key instead
        })
        
        # Compute shared secrets with both client and server
        client_shared = compute_shared_secret(client_public, mitm_private_for_client, p)
        server_shared = compute_shared_secret(server_public, mitm_private_for_server, p)
        
        print(f"{Colors.RED}Computed shared secret with client: {client_shared}{Colors.ENDC}")
        print(f"{Colors.RED}Computed shared secret with server: {server_shared}{Colors.ENDC}")
        
        # Derive session keys
        client_session_key = derive_session_key(client_shared)
        server_session_key = derive_session_key(server_shared)
        
        print(f"{Colors.RED}Derived session key with client: {client_session_key}{Colors.ENDC}")
        print(f"{Colors.RED}Derived session key with server: {server_session_key}{Colors.ENDC}")
        
        # Intercept messages
        while True:
            # Receive encrypted message from client
            data = receive_data(client_socket)
            if not data or "encrypted" not in data:
                break
            
            encrypted_from_client = data["encrypted"]
            decrypted = decrypt_message(encrypted_from_client, client_session_key)
            
            print(f"{Colors.RED}Intercepted from client (encrypted): '{encrypted_from_client}'{Colors.ENDC}")
            print(f"{Colors.RED}Decrypted client message: '{decrypted}'{Colors.ENDC}")
            
            # Modify the message
            modified = decrypted.replace("secret", "HACKED")
            print(f"{Colors.RED}Modified message: '{modified}'{Colors.ENDC}")
            
            # Re-encrypt with server session key and forward
            encrypted_for_server = encrypt_message(modified, server_session_key)
            print(f"{Colors.RED}Re-encrypted for server: '{encrypted_for_server}'{Colors.ENDC}")
            
            send_data(server_socket, {
                "encrypted": encrypted_for_server
            })
            
            # Receive response from server
            data = receive_data(server_socket)
            if not data or "encrypted" not in data:
                break
            
            encrypted_from_server = data["encrypted"]
            decrypted_server = decrypt_message(encrypted_from_server, server_session_key)
            
            print(f"{Colors.RED}Intercepted from server (encrypted): '{encrypted_from_server}'{Colors.ENDC}")
            print(f"{Colors.RED}Decrypted server message: '{decrypted_server}'{Colors.ENDC}")
            
            # Modify the response
            modified_response = decrypted_server.replace("received", "INTERCEPTED")
            print(f"{Colors.RED}Modified response: '{modified_response}'{Colors.ENDC}")
            
            # Re-encrypt with client session key and forward
            encrypted_for_client = encrypt_message(modified_response, client_session_key)
            print(f"{Colors.RED}Re-encrypted for client: '{encrypted_for_client}'{Colors.ENDC}")
            
            send_data(client_socket, {
                "encrypted": encrypted_for_client
            })
            
    except Exception as e:
        print(f"{Colors.RED}MITM error: {e}{Colors.ENDC}")
    finally:
        mitm_socket.close()
        server_socket.close()

if __name__ == "__main__":
    import sys
    client_port = 5000
    server_port = 5001
    
    if len(sys.argv) > 1:
        client_port = int(sys.argv[1])
    if len(sys.argv) > 2:
        server_port = int(sys.argv[2])
        
    run_mitm_attack(client_port, server_port)