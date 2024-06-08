#include "Console.h"

#include <cstdlib>
#include <string>

#ifdef _WIN32 || _WIN64
#include<windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace Utils {
	Console::Console(std::istream& cin, std::ostream& cout, std::ostream& cerr, std::ostream& clog)
		:cin{ cin },
		cout{ cout },
		cerr{ cerr },
		clog{ clog }
	{
		supportsAnsiColourCodes = false;

#ifdef _WIN32 || _WIN64
		// Assumes that the terminal supports colour codes
		supportsAnsiColourCodes = true;
#else
		char* termEnvironmentVariable = std::getenv("TERM");
		if (termEnvironmentVariable != nullptr && std::string(termEnvironmentVariable).find("color"))
			supportsAnsiColourCodes = true; // TODO: This is by no means exhaustive
#endif
	}

	void Console::PrintError(const std::string& text) {
		if (supportsAnsiColourCodes)
			cerr << RED_ANSI_CODE;

		cerr << text;

		if (supportsAnsiColourCodes)
			cerr << RESET_ANSI_CODE;
	}
	void Console::Print(const std::string& text)
	{
		cout << text;
	}
	void Console::Print(char c)
	{
		cout << c;
	}
	std::string Console::ReadPassword()
	{
		ToggleInputEcho();
		std::string pass;
		std::getline(cin, pass);

		ToggleInputEcho();
		return pass;
	}

	void Console::ToggleInputEcho() {
#ifdef _WIN32 || _WIN64
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode;
		GetConsoleMode(hStdin, &mode);

		mode ^= ENABLE_ECHO_INPUT;
		SetConsoleMode(hStdin, mode);
#else
		struct termios tty;
		tcgetattr(STDIN_FILENO, &tty);

		tty.c_lflag ^= ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
	}
}
