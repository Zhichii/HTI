#pragma once

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <list>
#include <map>
#include <any>
#include <variant>
#include <functional>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <mutex>

#if defined _WIN32
#include <Windows.h>
#include <conio.h>
#define CHH_IS_WINDOWS 1
#define CHH_IS_LINUX 0
#define getch _getch
#define kbhit _kbhit
#define sleep Sleep
#undef max
#undef min
#elif defined __linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#define CHH_IS_WINDOWS 0
#define CHH_IS_LINUX 1
#define sleep usleep
int getch();
int kbhit();
#endif

#include <json/json.h>

// 一个轻量级的的 C++ 工具箱。
namespace chh {

	// 一个基于 std::atomic 的包装类，用于原子操作。
	template <typename T>
	class Atomic {
	public:
		// 默认构造。
		Atomic() : atom(new T) {}
		// 拷贝构造。
		Atomic(const T& value) : atom(new T(value)) {}
		// 移动构造。
		Atomic(T&& value) : atom(new T(std::move(value))) {}
		// 析构函数。
		~Atomic() {
			delete atom.load();
		}
		// 拷贝赋值。
		Atomic& operator=(const T& value) {
			const T* old = atom.exchange(new T(value));
			delete old;
			return *this;
		}
		// 移动赋值
		Atomic& operator=(T&& value) {
			const T* old = atom.exchange(new T(std::move(value)));
			delete old;
			return *this;
		}
		// 安全读取。
		T load() const {
			return *atom.load(std::memory_order_acquire);
		}
		// 隐式转换。
		operator T() const {
			return load();
		}
	private:
		std::atomic<const T*> atom;
	};

	// 将字符向量转换为字符串。
	std::string toString(const std::vector<char>& vec);

	// 将宽字符串转换为窄字符串。
	std::string toString(const std::wstring& wstr);

	// 将窄字符串转换为宽字符串。
	std::wstring toWString(const std::string& str);

	// 从文件中读取字符向量。
	std::vector<char> readFile(const std::string& file_name);

#if CHH_IS_WINDOWS
	// 从 Windows 资源中读取字符向量。
	// 该函数仅在 Windows 平台上可用。
	std::vector<char> readResource(const size_t& name, const std::string& type);
#elif CHH_IS_LINUX
	// 从 Windows 资源中读取字符向量。
	// 该函数在 Linux 平台上不可用。
	std::vector<char> readResource(const size_t& name, const std::string& type) = delete;
#endif

	// 从 JSON 字符串中解析对象。
	Json::Value parseJson(const std::string& content);

}