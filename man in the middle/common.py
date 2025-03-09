import hashlib
import json


class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'    # Client
    GREEN = '\033[92m'   # Server
    RED = '\033[91m'     # Attacker
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def generate_key(g, p, private_key):
    """Generate public key using g^private_key mod p"""
    return pow(g, private_key, p)

def compute_shared_secret(public_key, private_key, p):
    """Compute shared secret using public_key^private_key mod p"""
    return pow(public_key, private_key, p)

def derive_session_key(shared_secret):
    """Derive a session key from the shared secret using SHA-256"""
    return hashlib.sha256(str(shared_secret).encode()).hexdigest()[:16]

def encrypt_message(message, key):
    """Simple XOR encryption for demonstration"""
    encrypted = []
    for i in range(len(message)):
        
        key_char = ord(key[i % len(key)])
        msg_char = ord(message[i])
        encrypted.append(chr(msg_char ^ key_char))
    return ''.join(encrypted)

def decrypt_message(encrypted, key):
    """Decrypt XOR encrypted message"""
   
    return encrypt_message(encrypted, key)

def send_data(sock, data):
    """Send data over socket as JSON"""
    try:
        message = json.dumps(data)
        sock.sendall(message.encode())
    except Exception as e:
        print(f"Error sending data: {e}")

def receive_data(sock):
    """Receive data from socket as JSON"""
    try:
        data = b''
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            data += chunk
            try:
                return json.loads(data.decode())
            except json.JSONDecodeError:
                continue
    except Exception as e:
        print(f"Error receiving data: {e}")
        return None