#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <limits>
#include "codec-core/api.h"

// Helper: Read file to byte vector
std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Helper: Write byte vector to file
void writeFile(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write file: " + path);
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void printUsage(const char* progName) {
    std::cout << "Usage:\n";
    std::cout << "  1. Interactive Mode (Run without arguments)\n";
    std::cout << "     " << progName << "\n";
    std::cout << "  2. Generate Keys:\n";
    std::cout << "     " << progName << " keygen [output_dir]\n";
    std::cout << "  3. Encrypt (Sign):\n";
    std::cout << "     " << progName << " sign <input_file> <secret_key_file> [output_file]\n";
    std::cout << "  4. Decrypt (Verify & Open):\n";
    std::cout << "     " << progName << " verify <signed_file> <public_key_file> [output_file]\n";
}

void interactiveMode() {
    while (true) {
        std::cout << "\n================================\n";
        std::cout << "   Dilithium Encryption Tool    \n";
        std::cout << "================================\n";
        std::cout << "1. Encrypt file\n";
        std::cout << "2. Decrypt file\n";
        std::cout << "3. Exit\n";
        std::cout << "Select option: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // consume newline

        if (choice == 3) {
            std::cout << "Exiting...\n";
            break;
        }

        try {
            if (choice == 1) {
                std::string inputPath, skPath;
                std::cout << "Enter path to file to encrypt: ";
                std::getline(std::cin, inputPath);
                std::cout << "Enter path to secret key (e.g., secrets/secret.key): ";
                std::getline(std::cin, skPath);

                if (inputPath.empty() || skPath.empty()) {
                    std::cout << "[-] Paths cannot be empty.\n";
                    continue;
                }

                std::cout << "[*] Reading input...\n";
                auto data = readFile(inputPath);
                std::cout << "[*] Reading secret key...\n";
                auto sk = readFile(skPath);
                
                std::string dataStr(data.begin(), data.end());
                std::cout << "[*] Signing/Encrypting...\n";
                auto signedData = DilithiumCodec::CryptoService::encrypt(dataStr, sk);
                
                std::string outPath = inputPath + ".signed";
                writeFile(outPath, signedData);
                std::cout << "[+] Done! Encrypted file saved to: " << outPath << "\n";

            } else if (choice == 2) {
                std::string inputPath, pkPath;
                std::cout << "Enter path to file to decrypt (signed file): ";
                std::getline(std::cin, inputPath);
                std::cout << "Enter path to public key (e.g., secrets/public.key): ";
                std::getline(std::cin, pkPath);

                if (inputPath.empty() || pkPath.empty()) {
                    std::cout << "[-] Paths cannot be empty.\n";
                    continue;
                }

                std::cout << "[*] Reading signed file...\n";
                auto signedData = readFile(inputPath);
                std::cout << "[*] Reading public key...\n";
                auto pk = readFile(pkPath);

                std::cout << "[*] Verifying/Decrypting...\n";
                std::string restored = DilithiumCodec::CryptoService::decrypt(signedData, pk);
                
                std::string outPath = inputPath + ".restored";
                // Try to restore original name if it ends with .signed
                if (inputPath.size() > 7 && inputPath.substr(inputPath.size() - 7) == ".signed") {
                    outPath = inputPath.substr(0, inputPath.size() - 7); 
                }

                std::vector<uint8_t> outData(restored.begin(), restored.end());
                writeFile(outPath, outData);
                std::cout << "[+] Done! Original content restored to: " << outPath << "\n";
            } else {
                std::cout << "Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "[-] Error: " << e.what() << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        interactiveMode();
        return 0;
    }

    std::string command = argv[1];

    try {
        if (command == "keygen") {
            std::string outDir = (argc >= 3) ? argv[2] : ".";
            
            std::cout << "[*] Generating Keypair...\n";
            auto kp = DilithiumCodec::CryptoService::generateKeyPair();
            
            std::string pkPath = outDir + "/public.key";
            std::string skPath = outDir + "/secret.key";
            
            writeFile(pkPath, kp.publicKey);
            writeFile(skPath, kp.secretKey);
            
            std::cout << "[+] Success!\n";
            std::cout << "    Public Key: " << pkPath << " (" << kp.publicKey.size() << " bytes)\n";
            std::cout << "    Secret Key: " << skPath << " (" << kp.secretKey.size() << " bytes)\n";

        } else if (command == "sign") {
            if (argc < 4) { printUsage(argv[0]); return 1; }
            std::string inputFile = argv[2];
            std::string skFile = argv[3];
            std::string outFile = (argc >= 5) ? argv[4] : inputFile + ".signed";

            std::cout << "[*] Reading input: " << inputFile << "\n";
            auto data = readFile(inputFile);
            std::cout << "[*] Reading secret key: " << skFile << "\n";
            auto sk = readFile(skFile);
            
            // Convert data to string for the API
            std::string dataStr(data.begin(), data.end());

            std::cout << "[*] Signing...\n";
            auto signedData = DilithiumCodec::CryptoService::encrypt(dataStr, sk);
            
            writeFile(outFile, signedData);
            std::cout << "[+] Signed successfully!\n";
            std::cout << "    Output: " << outFile << " (" << signedData.size() << " bytes)\n";

        } else if (command == "verify") {
            if (argc < 4) { printUsage(argv[0]); return 1; }
            std::string signedFile = argv[2];
            std::string pkFile = argv[3];
            std::string outFile = (argc >= 5) ? argv[4] : signedFile + ".restored";

            std::cout << "[*] Reading signed file: " << signedFile << "\n";
            auto signedData = readFile(signedFile);
            std::cout << "[*] Reading public key: " << pkFile << "\n";
            auto pk = readFile(pkFile);

            std::cout << "[*] Verifying...\n";
            std::string restored = DilithiumCodec::CryptoService::decrypt(signedData, pk);
            
            // Convert back to bytes to write
            std::vector<uint8_t> outData(restored.begin(), restored.end());
            writeFile(outFile, outData);
            
            std::cout << "[+] Verification Successful! Signature is valid.\n";
            std::cout << "    Original content restored to: " << outFile << "\n";

        } else {
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "[-] Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}