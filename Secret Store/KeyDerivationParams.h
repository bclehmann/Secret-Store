#pragma once
#include <sodium.h>
#include <memory>
#include <string>
#include <stdexcept>

class KeyDerivationParams {
public:
	KeyDerivationParams() {
		randombytes_buf(salt, sizeof salt);
	}

	std::unique_ptr<unsigned char[]> GetKey(std::string& password) {
		std::unique_ptr<unsigned char[]> key = std::make_unique<unsigned char[]>(outlen);

		if (crypto_pwhash(key.get(),
			outlen,
			password.c_str(),
			password.size(), // This is not an off-by-one error, this API does not care about the null terminator
			salt,
			opslimit,
			memlimit,
			algorithm) != 0) {
			throw std::runtime_error("Could not generate key");
		}

		return key;
	}
private:
	int algorithm = crypto_pwhash_ALG_DEFAULT;
	unsigned long long outlen = crypto_secretstream_xchacha20poly1305_KEYBYTES; // 256 bit
	unsigned char salt[crypto_pwhash_SALTBYTES] = {}; // 128 bit
	unsigned long long opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	size_t memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
};