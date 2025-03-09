
import random
import socket

from common import Colors, generate_key, compute_shared_secret, derive_session_key
from common import encrypt_message, decrypt_message, send_data, receive_data

def run_client(server_host='localhost', server_port=5000):
    """Run the Diffie-Hellman client"""
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        print(f"{Colors.BLUE}Connecting to server at {server_host}:{server_port}{Colors.ENDC}")
        client_socket.connect((server_host, server_port))
   
        data = receive_data(client_socket)
        if not data or "p" not in data or "g" not in data or "public_key" not in data:
            print(f"{Colors.RED}Error: Invalid data received from server{Colors.ENDC}")
            return
        
        p = data["p"]
        g = data["g"]
        server_public = data["public_key"]
        
        print(f"{Colors.BLUE}Received parameters: p={p}, g={g}{Colors.ENDC}")
        print(f"{Colors.BLUE}Received server's public key: {server_public}{Colors.ENDC}")
        
        
        client_private = random.randint(1, p-1)
        client_public = generate_key(g, p, client_private)
        
        print(f"{Colors.BLUE}Client generated private key: {client_private}{Colors.ENDC}")
        print(f"{Colors.BLUE}Client calculated public key: {client_public}{Colors.ENDC}")
        
        send_data(client_socket, {
            "public_key": client_public
        })
        
        
        shared_secret = compute_shared_secret(server_public, client_private, p)
        print(f"{Colors.BLUE}Computed shared secret: {shared_secret}{Colors.ENDC}")
        
        
        session_key = derive_session_key(shared_secret)
        print(f"{Colors.BLUE}Derived session key: {session_key}{Colors.ENDC}")
        
       
        message = "Hello Server! This is a secret message from the client."
        encrypted = encrypt_message(message, session_key)
        
        print(f"{Colors.BLUE}Original message: '{message}'{Colors.ENDC}")
        print(f"{Colors.BLUE}Encrypted message: '{encrypted}'{Colors.ENDC}")
        
        send_data(client_socket, {
            "encrypted": encrypted
        })
        
        data = receive_data(client_socket)
        if not data or "encrypted" not in data:
            print(f"{Colors.RED}Error: Invalid response from server{Colors.ENDC}")
            return
        
        encrypted_response = data["encrypted"]
        decrypted_response = decrypt_message(encrypted_response, session_key)
        
        print(f"{Colors.BLUE}Received encrypted response: '{encrypted_response}'{Colors.ENDC}")
        print(f"{Colors.BLUE}Decrypted response: '{decrypted_response}'{Colors.ENDC}")
        
    except Exception as e:
        print(f"{Colors.RED}Client error: {e}{Colors.ENDC}")
    finally:
        client_socket.close()

if __name__ == "__main__":
    import sys
    host = 'localhost'
    port = 5000
    
    if len(sys.argv) > 1:
        host = sys.argv[1]
    if len(sys.argv) > 2:
        port = int(sys.argv[2])
        
    run_client(host, port)