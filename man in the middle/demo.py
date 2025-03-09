

def run_demo():
    """Run a demonstration of the MITM attack"""
    import subprocess
    import threading
    import time
    
    def start_server():
        print(f"{Colors.HEADER}Starting legitimate server on port 5001...{Colors.ENDC}")
        subprocess.run(["python3", "server.py", "5001"])
    
    def start_mitm():
        print(f"{Colors.HEADER}Starting MITM attacker on port 5000, forwarding to 5001...{Colors.ENDC}")
        subprocess.run(["python3", "mitm.py", "5000", "5001"])
    
    def start_client():
        print(f"{Colors.HEADER}Starting client connecting to port 5000 (MITM)...{Colors.ENDC}")
        subprocess.run(["python3", "client.py", "localhost", "5000"])
    
    server_thread = threading.Thread(target=start_server)
    server_thread.daemon = True
    server_thread.start()
    
    time.sleep(1)
    
    mitm_thread = threading.Thread(target=start_mitm)
    mitm_thread.daemon = True
    mitm_thread.start()

    time.sleep(1)
    start_client()
    try:
        while server_thread.is_alive() and mitm_thread.is_alive():
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("Demo terminated by user")

if __name__ == "__main__":
    
    from common import Colors
    print(f"{Colors.HEADER}{Colors.BOLD}Diffie-Hellman MITM Attack Demonstration{Colors.ENDC}")
    run_demo()