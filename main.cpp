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
// Forward declaration so interactiveMode() and main() can call it.
static std::string makeRestoredFilename(const std::string &signedPath);
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
        std::cout << "1. KeyGen" << "\n";
        std::cout << "2. Encrypt file\n";
        std::cout << "3. Decrypt file\n";
        std::cout << "4. Regenerate pk from sk\n";
        std::cout << "5. Exit\n";
        std::cout << "Select option: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // consume newline

        if (choice == 5) {
            std::cout << "Exiting...\n";
            break;
        }

        try {
            if (choice == 1) {
                std::string outDir;
                std::cout << "Enter output directory for keys (default: secrets): ";
                std::getline(std::cin, outDir);
                if (outDir.empty()) {
                    outDir = "secrets";
                }

                std::cout << "[*] Generating Keypair...\n";
                auto kp = DilithiumCodec::CryptoService::generateKeyPair();
                
                // Try to create directory if it doesn't exist
                std::string cmd = "if not exist \"" + outDir + "\" mkdir \"" + outDir + "\"";
                system(cmd.c_str());

                std::string pkPath = outDir + "/public.key";
                std::string skPath = outDir + "/secret.key";
                
                writeFile(pkPath, kp.publicKey);
                writeFile(skPath, kp.secretKey);
                
                std::cout << "[+] Success!\n";
                std::cout << "    Public Key: " << pkPath << " (" << kp.publicKey.size() << " bytes)\n";
                std::cout << "    Secret Key: " << skPath << " (" << kp.secretKey.size() << " bytes)\n";

            } else if (choice == 2) {
                std::string inputPath;
                std::cout << "Enter path to file to encrypt: ";
                std::getline(std::cin, inputPath);

                if (inputPath.empty()) {
                    std::cout << "[-] Path cannot be empty.\n";
                    continue;
                }

                std::string skPath = "secrets/secret.key";
                std::cout << "[*] Reading input...\n";
                auto data = readFile(inputPath);
                std::cout << "[*] Reading secret key (" << skPath << ")...\n";
                auto sk = readFile(skPath);
                
                std::string dataStr(data.begin(), data.end());
                std::cout << "[*] Signing/Encrypting...\n";
                auto signedData = DilithiumCodec::CryptoService::encrypt(dataStr, sk);
                
                std::string outPath = inputPath + ".signed";
                writeFile(outPath, signedData);
                std::cout << "[+] Done! Encrypted file saved to: " << outPath << "\n";

            } else if (choice == 3) {
                std::string inputPath;
                std::cout << "Enter path to file to decrypt (signed file): ";
                std::getline(std::cin, inputPath);

                if (inputPath.empty()) {
                    std::cout << "[-] Path cannot be empty.\n";
                    continue;
                }

                std::string pkPath = "secrets/public.key";
                std::cout << "[*] Reading signed file...\n";
                auto signedData = readFile(inputPath);
                std::cout << "[*] Reading public key (" << pkPath << ")...\n";
                auto pk = readFile(pkPath);

                std::cout << "[*] Verifying/Decrypting...\n";
                std::string restored = DilithiumCodec::CryptoService::decrypt(signedData, pk);
                
                std::string outPath = makeRestoredFilename(inputPath);
                
                std::vector<uint8_t> outData(restored.begin(), restored.end());
                writeFile(outPath, outData);
                std::cout << "[+] Done! Original content restored to: " << outPath << "\n";
            } else if (choice == 4) {
                std::cout << "[-] Feature not implemented yet.\n";
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
            std::string outFile = (argc >= 5) ? argv[4] : makeRestoredFilename(signedFile);

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

// Helper: Build restored filename from a signed filename.
// If ends with ".signed" insert "_restored" before the file extension
static std::string makeRestoredFilename(const std::string &signedPath) {
    std::string base = signedPath;

    if (base.size() > 7 && base.substr(base.size() - 7) == ".signed") {
        base = base.substr(0, base.size() - 7);
    }

    size_t sep = base.find_last_of("/\\");
    size_t dot = base.find_last_of('.');

    if (dot != std::string::npos && (sep == std::string::npos || dot > sep)) {
        // has extension, insert _restored before the dot
        return base.substr(0, dot) + "_restored" + base.substr(dot);
    }

    // no extension -> append suffix
    return base + "_restored";
}

// Forward declaration so interactiveMode() and main() can call it.
// (prototype already declared earlier)