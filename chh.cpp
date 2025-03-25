#include "chh.hpp"

#if defined __linux__
int getch() {
	termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}
int kbhit() {
	termios oldt, newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if (ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}
#endif

namespace chh {

	std::string toString(const std::vector<char>& vec) {
		return std::string(vec.begin(), vec.end());
	}

	std::string toString(const std::wstring& wstr) {
		if (wstr.empty()) return std::string();
		size_t size_needed = std::wcstombs(nullptr, wstr.c_str(), 0);
		if (size_needed == static_cast<size_t>(-1)) {
			throw std::runtime_error("Invalid wide character encountered during conversion.");
		}
		std::string str(size_needed, 0);
		std::wcstombs(str.data(), wstr.c_str(), size_needed);
		return str;
	}

	std::wstring toWString(const std::string& str) {
		if (str.empty()) return std::wstring();
		size_t size_needed = std::mbstowcs(nullptr, str.c_str(), 0);
		if (size_needed == static_cast<size_t>(-1)) {
			throw std::runtime_error("Invalid character encountered during conversion.");
		}
		std::wstring wstr(size_needed, 0);
		std::mbstowcs(wstr.data(), str.c_str(), size_needed);
		return wstr;
	}

	std::vector<char> readFile(const std::string& file_name) {
		try {
			std::ifstream file(file_name, std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Unable to open file: " + file_name + ".");
			}
			std::uintmax_t size = std::filesystem::file_size(file_name);
			if (size > std::numeric_limits<std::streamsize>::max()) {
				throw std::runtime_error("File too large to read into a vector.");
			}
			std::vector<char> vec(size);
			if (!file.read(vec.data(), std::streamsize(size))) {
				throw std::runtime_error("Unable to read file: " + file_name + ".");
			}
			return vec;
		}
		catch (const std::filesystem::filesystem_error& e) {
			throw std::runtime_error("Filesystem error: " + std::string(e.what()) + ".");
		}
	}

#if CHH_IS_WINDOWS
	std::vector<char> readResource(const size_t& name, const std::string& type) {
		HRSRC hRsrc = FindResourceA(NULL, MAKEINTRESOURCEA(name), type.c_str());
		if (!hRsrc) {
			throw std::runtime_error("Failed to find resource: " + std::to_string(name) + ".");
		}
		HGLOBAL IDR = LoadResource(NULL, hRsrc);
		if (!IDR) {
			throw std::runtime_error("Failed to load resource: " + std::to_string(name) + ".");
		}
		DWORD size = SizeofResource(NULL, hRsrc);
		if (size == 0) {
			FreeResource(IDR);
			throw std::runtime_error("Resource size is 0: " + std::to_string(name) + ".");
		}
		char* arr = (char*)LockResource(IDR);
		if (!arr) {
			FreeResource(IDR);
			throw std::runtime_error("Failed to lock resource: " + std::to_string(name) + ".");
		}
		std::vector<char> vec(arr, arr + size);
		FreeResource(IDR);
		return vec;
	}
#endif

	Json::Value parseJson(const std::string& content) {
		Json::Value root;
		Json::CharReaderBuilder reader;
		std::unique_ptr<Json::CharReader> parser(reader.newCharReader());
		std::string errs;
		if (!parser->parse(content.c_str(), content.c_str() + content.size(), &root, &errs)) {
			throw std::runtime_error("Failed to parse JSON: \n" + errs);
		}
		return root;
	}

}