#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include "des_crypto.h"

using namespace std;

string binaryStringToBytes(const string& binary) {
    string bytes = "";
    for (size_t i = 0; i < binary.length(); i += 8) {
        string byte = binary.substr(i, 8);
        char c = (char)stoi(byte, 0, 2);
        bytes += c;
    }
    return bytes;
}

string bytesToBinaryString(const string& bytes) {
    string binary = "";
    for (unsigned char c : bytes) {
        binary += bitset<8>(c).to_string();
    }
    return binary;
}

void receiveMessages(int clientSocket, DESCrypto& crypto, const string& iv) {
    char buffer[8192];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (bytesReceived <= 0) {
            cout << "\n[Connection closed by peer]" << endl;
            break;
        }
        
        string receivedBytes(buffer, bytesReceived);
        string ciphertext_binary = bytesToBinaryString(receivedBytes);
        
        cout << "\n┌─ RECEIVED ENCRYPTED MESSAGE (Binary) ─┐" << endl;
        cout << "│ Length: " << ciphertext_binary.length() << " bits" << endl;
        cout << "│ " << ciphertext_binary << endl;
        cout << "└────────────────────────────────────────┘" << endl;
        
        string decrypted = crypto.decrypt(ciphertext_binary, iv);
        cout << "[Client]: " << decrypted << endl;
        cout << ">> ";
        cout.flush();
    }
}

int main() {
    string key_text, iv;
    
    cout << "=== DES Encrypted Chat Server ===" << endl;
    cout << "Enter encryption key (exactly 8 characters): ";
    getline(cin, key_text);
    
    if (key_text.length() != 8) {
        cout << "Error: Key must be exactly 8 characters!" << endl;
        return 1;
    }
    
    DESCrypto crypto;
    string key_binary = "";
    for (char c : key_text) {
        key_binary += bitset<8>(c).to_string();
    }
    crypto.generate_keys(key_binary);
    
    iv = "12345678"; 
    
    cout << "\n[Key Set]: " << key_text << endl;
    cout << "[Key Binary]: " << key_binary << endl;
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error binding socket" << endl;
        close(serverSocket);
        return 1;
    }
    
    listen(serverSocket, 5);
    cout << "\n[Server is listening on port 8080...]" << endl;
    
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0) {
        cerr << "Error accepting connection" << endl;
        close(serverSocket);
        return 1;
    }
    
    cout << "[Client connected!]" << endl;
    cout << "\nYou can now send encrypted messages. Type 'quit' to exit.\n" << endl;
    
    thread receiveThread(receiveMessages, clientSocket, ref(crypto), ref(iv));
    receiveThread.detach();
    
    string message;
    while (true) {
        cout << ">> ";
        getline(cin, message);
        
        if (message == "quit") {
            break;
        }
        
        if (!message.empty()) {
            string encrypted_binary = crypto.encrypt(message, iv);
            
            cout << "\n┌─ SENDING ENCRYPTED MESSAGE (Binary) ─┐" << endl;
            cout << "│ Length: " << encrypted_binary.length() << " bits" << endl;
            cout << "│ " << encrypted_binary << endl;
            cout << "└───────────────────────────────────────┘" << endl;
            
            string encrypted_bytes = binaryStringToBytes(encrypted_binary);
            send(clientSocket, encrypted_bytes.c_str(), encrypted_bytes.length(), 0);
        }
    }
    
    close(clientSocket);
    close(serverSocket);
    
    cout << "\n[Server closed]" << endl;
    return 0;
}