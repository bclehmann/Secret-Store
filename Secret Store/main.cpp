#include <iostream>;
#include <string>;
#include <algorithm>
#include <sodium.h>
#include "Console.h";
#include <vector>
#include "SecretFile.h"

#ifdef _WIN32 || _WIN64
#include <io.h>
#else
#include <unistd.h>
#endif

const int ERROR_EXIT_CODE = 127;

void stringToUpper(std::string& s) {
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

std::string getHome() {
#ifdef _WIN32 || _WIN64
	return std::getenv("USERPROFILE");
#else
	return std::getenv("HOME");
#endif
}

int main(int argc, char* argv[])
{
	Utils::Console console(std::cin, std::cout, std::cerr, std::clog);

	if (sodium_init() != 0) {
		console.PrintError("Could not initialize cryptography\n");
		return ERROR_EXIT_CODE;
	}

	// TODO: user settable
	unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES] = "this is an ok key i suppose";
	crypto_secretstream_xchacha20poly1305_keygen(key);


	std::string home = getHome();
	std::string path = home + std::string("/.sneak");

	if (argc < 2) {
		console.PrintError("No command specified\n");
		return ERROR_EXIT_CODE;
	}

	std::vector<std::string> args;
	args.reserve(argc);
	for (int i = 0; i < argc; i++) {
		args.emplace_back(argv[i]);
	}

	std::string& command = args[1];
	stringToUpper(command);

	if (command == "READ") {
		SecretFile secret(path);
		std::vector<char> data = secret.Read(key);

		FILE* const out = fdopen(dup(fileno(stdout)), "wb");
		std::fwrite(data.data(), sizeof(char), data.size(), stdout);
	}
	else if (command == "WRITE") {
		std::vector<char> data;
		FILE* const in = fdopen(dup(fileno(stdin)), "rb");
		while (!std::feof(in)) {
			const int buf_size = 1024;
			char buf[buf_size] = {};

			auto read_chars = std::fread(buf, sizeof(char), buf_size, in);
			data.reserve(data.size() + read_chars);
			for (int i = 0; i < read_chars; i++)
				data.push_back(buf[i]);
		}

		SecretFile secret(path);
		secret.Write(data, key);
	}
	else {
		console.PrintError("Unknown command\n");
	}

	return 0;
}
