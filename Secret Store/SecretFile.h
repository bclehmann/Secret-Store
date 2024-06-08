#pragma once
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include "FileWrapper.h"
#include "KeyDerivationParams.h"

#define MAGIC_NUMBER "SNEAKY!"
#define ENCRYPTION_TEST_STRING "We hope this is the same as when we created the file 0123456789"
#define ENCRYPTION_TEST_STRING_LENGTH 64

#pragma pack(push, 1)
struct SecretFileHeader {
	char magic_number[8] = MAGIC_NUMBER;
	uint16_t version_major = 1;
	uint16_t version_minor = 0;
	KeyDerivationParams params = {};
	uint32_t reserved[8] = {};
	// All of the above are unencrypted
	
	// Used to check the key was correct, it's not necessary as libsodium does this for us,
	// but it's a nice sanity check if/when we move this to a different algorithm
	char encryption_test_string[ENCRYPTION_TEST_STRING_LENGTH] = ENCRYPTION_TEST_STRING;
};
#pragma pack(pop)

class SecretFile {
public:
	SecretFile(const std::string& path);
	~SecretFile();
	std::vector<char> Read(std::string& password);
	void Write(std::vector<char>& data, std::string& password);

private:
	std::string path;
};