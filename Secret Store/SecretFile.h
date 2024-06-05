#pragma once
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include "FileWrapper.h"

#define MAGIC_NUMBER "SNEAKY!"
#define ENCRYPTION_TEST_STRING "We hope this is the same as when we created the file"

#pragma pack(push, 1)
struct SecretFileHeader {
	char magic_number[8] = MAGIC_NUMBER;
	uint16_t version_major = 1;
	uint16_t version_minor = 0;
	uint32_t reserved[8] = {};
	uint32_t align = 0;
	// All of the above are unencrypted
	char encryption_test_string[64] = ENCRYPTION_TEST_STRING; // Used to check decryption worked
};
#pragma pack(pop)

class SecretFile {
public:
	SecretFile(const std::string& path);
	~SecretFile();
	std::vector<char> Read(const std::string& decryption_key);
	void Write(std::vector<char>& data, const std::string& encryption_key);

private:
	void Encrypt(char* s, size_t len);
	void Decrypt(char* s, size_t len);
	std::string path;
};