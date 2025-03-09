public class CaesarCipher {

    // Encrypt message using Caesar Cipher with both positive and negative shifts
    public static String encrypt(String text, int shift) {
        StringBuilder encrypted = new StringBuilder();
        for (char i : text.toCharArray()) {
            if (Character.isLetter(i)) {
                char base = Character.isLowerCase(i) ? 'a' : 'A';
                encrypted.append((char) ((i - base + shift + 26) % 26 + base));
            } else {
                encrypted.append(i); // Keep non-alphabet characters unchanged
            }
        }
        return encrypted.toString();
    }

    // Decrypt message using Caesar Cipher with both positive and negative shifts
    public static String decrypt(String text, int shift) {
        return encrypt(text, -shift);  // To decrypt, just use the negative shift
    }
}

 