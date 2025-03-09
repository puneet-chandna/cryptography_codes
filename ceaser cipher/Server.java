import java.io.*;
import java.net.*;

public class Server {
    private static final int PORT = 1234;  // Port to listen for client connections
    private static final int SHIFT = 3;    // Caesar cipher shift value

    public static void main(String[] args) {
        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Server is listening on port " + PORT);
            while (true) {
                new ClientHandler(serverSocket.accept()).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Thread to handle each client
    private static class ClientHandler extends Thread {
        private Socket socket;
        private PrintWriter out;
        private BufferedReader in;

        public ClientHandler(Socket socket) {
            this.socket = socket;
        }

        public void run() {
            try {
                // Setup input and output streams
                in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                out = new PrintWriter(socket.getOutputStream(), true);

                String encryptedMessage;
                while ((encryptedMessage = in.readLine()) != null) {
                    String decryptedMessage = CaesarCipher.decrypt(encryptedMessage, SHIFT);
                    System.out.println("Received encrypted: " + encryptedMessage);
                    System.out.println("Decrypted message: " + decryptedMessage);
                    out.println("Message received and decrypted: " + decryptedMessage);
                }
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}