#pragma once

#include <istream>
#include <ostream>

namespace Utils {
	class Console {
	public:
		Console(std::istream& cin, std::ostream& cout, std::ostream& cerr, std::ostream& clog);
		void PrintError(const std::string& text);
		void Print(const std::string& text);
		void Print(char c);
		std::string ReadPassword();

	private:
		void ToggleInputEcho();
		bool supportsAnsiColourCodes;
		std::istream& cin;
		std::ostream& cout;
		std::ostream& cerr;
		std::ostream& clog;

		static constexpr std::string_view RED_ANSI_CODE = "\x1B[31m";
		static constexpr std::string_view RESET_ANSI_CODE = "\x1B[0m";
	};
}
