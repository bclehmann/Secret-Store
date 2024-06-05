#pragma once
#include <cstdio>

struct FileWrapper {
	FileWrapper(FILE* f)
		: file{f}
	{}
	~FileWrapper() {
		std::fclose(file);
	}
	FILE* file = nullptr;
};