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

/**
 * @brief 一个轻量级的的 C++ 工具箱。
 */
namespace chh {

	/**
	 * @brief 将字符向量转换为字符串。
	 * @param vec 字符向量。
	 * @return 转换后的字符串。
	 */
	std::string toString(const std::vector<char>& vec);

	/**
	 * @brief 将宽字符串转换为窄字符串。
	 * @param wstr 宽字符串。
	 * @return 转换后的窄字符串。
	 * @throw std::runtime_error 如果转换失败。
	 */
	std::string toString(const std::wstring& wstr);

	/**
	 * @brief 将窄字符串转换为宽字符串。
	 * @param str 宽字符串。
	 * @return 转换后的窄字符串。
	 * @throw std::runtime_error 如果转换失败。
	 */
	std::wstring toWString(const std::string& str);

	/**
	 * @brief 从文件中读取字符向量。
	 * @param file_name 文件名。
	 * @return 文件内容。
	 * @throw std::runtime_error 如果文件读取失败。
	 */
	std::vector<char> readFile(const std::string& file_name);

#if CHH_IS_WINDOWS
	/**
	 * @brief 从 Windows 资源中读取字符向量。
	 * @note 该函数仅在 Windows 平台上可用。
	 * @param name 资源 ID 。
	 * @param type 资源类型。
	 * @return 资源内容。
	 * @throw std::runtime_error 如果资源读取失败。
	 */
	std::vector<char> readResource(const size_t& name, const std::string& type);
#elif CHH_IS_LINUX
	/**
	 * @brief 从 Windows 资源中读取字符向量。
	 * @warning 该函数仅在 Windows 平台上可用。
	 * @param name 资源 ID 。
	 * @param type 资源类型。
	 * @return 资源内容。
	 * @throw std::runtime_error 如果资源读取失败。
	 */
	std::vector<char> readResource(const size_t& name, const std::string& type) = delete;
#endif

	/**
	 * @brief 从 JSON 字符串中解析对象。
	 * @param content JSON 字符串。
	 * @return JSON 对象。
	 * @throws std::runtime_error 如果 JSON 解析失败。
	 */
	Json::Value parseJson(const std::string& content);

}