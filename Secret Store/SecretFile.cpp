#include "SecretFile.h"

#include <cstring>
#include <sodium.h>

#define CHUNK_SIZE 12

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
	:path{ path }
{
}

SecretFile::~SecretFile()
{
}

// See https://libsodium.gitbook.io/doc/secret-key_cryptography/secretstream#file-encryption-example-code
std::vector<char> SecretFile::Read(std::string& password)
{
	FileWrapper file(std::fopen(path.c_str(), "rb"));
	char header_buf[sizeof(SecretFileHeader)] = {};

	size_t read_chars = std::fread(header_buf, 1, sizeof(SecretFileHeader) - ENCRYPTION_TEST_STRING_LENGTH, file.file);

	if (read_chars != sizeof(SecretFileHeader) - (std::strlen(ENCRYPTION_TEST_STRING) + 1))
		throw std::runtime_error("Secret file is not readable");

	SecretFileHeader* header = reinterpret_cast<SecretFileHeader*>(header_buf);

	if (!SafeStringMatches(header->magic_number, MAGIC_NUMBER))
		throw std::runtime_error("Secret file is not readable");

	std::unique_ptr<unsigned char[]> key = header->params.GetKey(password);

	unsigned char crypto_header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
	crypto_secretstream_xchacha20poly1305_state st;
	std::fread(crypto_header, 1, sizeof crypto_header, file.file);
	if (crypto_secretstream_xchacha20poly1305_init_pull(&st, crypto_header, key.get()) != 0) {
		throw std::runtime_error("Could not read cryptography header");
	}

	std::vector<char> encryption_test_string;
	std::vector<char> output;
	int eof = 0;
	do {
		unsigned char buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
		unsigned char buf_out[CHUNK_SIZE];
		int rlen = std::fread(buf_in, sizeof(char), sizeof buf_in, file.file);
		eof = std::feof(file.file);

		unsigned long long out_len;
		unsigned char tag = 0;
		if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
			buf_in, rlen, NULL, 0) != 0) {
			throw std::runtime_error("Encountered corrupted chunk while decrypting");
		}
		if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL) {
			if (!eof) {
				throw std::runtime_error("End of stream reached before the end of the file.");
			}
		}
		else { /* not the final chunk yet */
			if (eof) {
				throw std::runtime_error("End of file reached before the end of the stream.");
			}
		}

		for (int i = 0; i < out_len; i++) {
			if (encryption_test_string.size() < ENCRYPTION_TEST_STRING_LENGTH) {
				encryption_test_string.push_back(buf_out[i]);
			}
			else {
				output.push_back(buf_out[i]);
			}
		}


	} while (!eof);

	if (!SafeStringMatches(encryption_test_string.data(), ENCRYPTION_TEST_STRING))
		throw std::runtime_error("Decryption key was incorrect");

	return output;
}

void SecretFile::Write(std::vector<char>& data, std::string& password)
{
	FileWrapper file(std::fopen(path.c_str(), "wb"));
	SecretFileHeader header;
	std::unique_ptr<unsigned char[]> key = header.params.GetKey(password);

	std::fwrite(reinterpret_cast<char*>(&header), 1, sizeof(SecretFileHeader) - ENCRYPTION_TEST_STRING_LENGTH, file.file);

	std::vector<char> input;
	std::string s(ENCRYPTION_TEST_STRING);
	for (int i = 0; i < ENCRYPTION_TEST_STRING_LENGTH; i++) {
		if (i <= s.size())
			input.push_back(s[i]);
		else
			input.push_back('\0');
	}
	for (char c : data)
		input.push_back(c);

	unsigned char  crypto_header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
	crypto_secretstream_xchacha20poly1305_state st;
	crypto_secretstream_xchacha20poly1305_init_push(&st, crypto_header, key.get());
	std::fwrite(crypto_header, 1, sizeof(crypto_header), file.file);

	int i = 0;
	bool eof = false;
	do {
		unsigned char  buf_in[CHUNK_SIZE] = {};
		unsigned char  buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
		unsigned long long out_len;

		int j = 0;
		for (; j < CHUNK_SIZE && i * CHUNK_SIZE + j < input.size(); j++)
			buf_in[j] = input[i * CHUNK_SIZE + j];

		int rlen = j;
		eof = j < CHUNK_SIZE;
		unsigned char tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
		crypto_secretstream_xchacha20poly1305_push(&st, buf_out, &out_len, buf_in, rlen,
			NULL, 0, tag);
		std::fwrite(buf_out, 1, (size_t)out_len, file.file);

		i++;
	} while (!eof);
}
