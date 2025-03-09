import java.io.*;
import java.net.*;

public class Client {
    private static final String SERVER_ADDRESS = "localhost";  // Server address
    private static final int SERVER_PORT = 1234;               // Server port
    private static final int SHIFT = 3;                         // Caesar cipher shift value

    public static void main(String[] args) {
        try (Socket socket = new Socket(SERVER_ADDRESS, SERVER_PORT);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
             BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in))) {

            System.out.println("Enter a message to send to the server:");

            String message = userInput.readLine();
            String encryptedMessage = CaesarCipher.encrypt(message, SHIFT);

            // Send encrypted message to the server
            out.println(encryptedMessage);

            // Get server's response
            String response = in.readLine();
            System.out.println("Server response: " + response);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}