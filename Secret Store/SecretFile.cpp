#include "SecretFile.h"

#include <cstring>

// expected must be null-terminated, actual needn't be (but it will return false)
bool SafeStringMatches(const char* actual, const char* expected) {
	int i = 0;
	while (actual[i] == expected[i]) {
		if (expected[i] == '\0')
			return true;

		i++;
	}

	return false;
}

SecretFile::SecretFile(const std::string& path)
	:path{path}
{
}

SecretFile::~SecretFile()
{
}

std::vector<char> SecretFile::Read(const std::string& decryption_key)
{
	FileWrapper file(std::fopen(path.c_str(), "rb"));
	char header_buf[sizeof(SecretFileHeader)] = {};

	size_t read_chars = std::fread(header_buf, 1, sizeof(SecretFileHeader), file.file);

	if (read_chars != sizeof(SecretFileHeader))
		throw new std::runtime_error("Secret file is not readable");

	SecretFileHeader* header = reinterpret_cast<SecretFileHeader*>(header_buf);


	if (!SafeStringMatches(header->magic_number, MAGIC_NUMBER))
		throw new std::runtime_error("Secret file is not readable");

	Decrypt(header->encryption_test_string, std::strlen(ENCRYPTION_TEST_STRING) + 1);

	if (!SafeStringMatches(header->encryption_test_string, ENCRYPTION_TEST_STRING))
		throw new std::runtime_error("Decryption key was incorrect");


	std::vector<char> output;
	while (!std::feof(file.file) && !std::ferror(file.file)) {
		const int buffer_size = 1024;
		char buf[buffer_size] = {};

		auto bytes_read = std::fread(buf, sizeof(char), buffer_size, file.file);

		output.reserve(output.size() + bytes_read);
		for (int i = 0; i < bytes_read; i++)
			output.push_back(buf[i]);
	}

	// TODO: Decrypt
	return output;
}

void SecretFile::Write(std::vector<char>& data, const std::string& encryption_key)
{
	FileWrapper file(std::fopen(path.c_str(), "wb"));
	SecretFileHeader header;

	std::fwrite(reinterpret_cast<char*>(&header), sizeof(SecretFileHeader), 1, file.file);

	char* buf = new char[data.size()];
	std::memcpy(buf, data.data(), data.size());
	Encrypt(buf, data.size());
	std::fwrite(buf, sizeof(char), data.size(), file.file);
}

void SecretFile::Encrypt(char* s, size_t len)
{
	// TODO
}

void SecretFile::Decrypt(char* s, size_t len)
{
	// TODO
}
