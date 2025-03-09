# client.py
import socket
import pickle
import sys

def encrypt(public_key, message):
    """Encrypt message using public key"""
    n, e = public_key
    
    message_int = int.from_bytes(message.encode(), 'big')
    if message_int >= n:
        raise ValueError("Message too long for key size")
    
    ciphertext = pow(message_int, e, n)
    return ciphertext

def decrypt(private_key, ciphertext):
    """Decrypt ciphertext using private key"""
    n, d = private_key
    
    message_int = pow(ciphertext, d, n)
    
    message_bytes = message_int.to_bytes((message_int.bit_length() + 7) // 8, 'big')
    return message_bytes.decode('utf-8', errors='ignore')

def main():
    
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        client_socket.connect(('localhost', 8000))
        print("Connected to server")
        
       
        data = client_socket.recv(4096)
        public_key = pickle.loads(data)
        print(f"Received server public key: {public_key}")
        
        while True:
            
            message = input("Enter message to encrypt (or 'quit' to exit): ")
            if message.lower() == 'quit':
                break
            
            message_int = int.from_bytes(message.encode(), 'big')
            if message_int >= public_key[0]:
                print("Message too long for key size, please enter a shorter message")
                continue
            
          
            encrypted_message = encrypt(public_key, message)
            print(f"Encrypted message: {encrypted_message}")
            client_socket.send(pickle.dumps(encrypted_message))
            
            
            data = client_socket.recv(4096)
            if not data:
                print("Server disconnected")
                break
                
            encrypted_response = pickle.loads(data)
            print(f"Received encrypted response: {encrypted_response}")
            
    except Exception as e:
        print(f"Error: {e}")
    
    finally:
        client_socket.close()
        print("Client shutdown")

if __name__ == "__main__":
    main()