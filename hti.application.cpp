#include "hti.hpp"

namespace hti {

	Event::~Event() = default;

	hti::LambdaEvent::LambdaEvent(EventFunc action) : _action(action) {}

	void LambdaEvent::execute() { _action(this); }

	bool Application::processEvent() {
		std::shared_ptr<Event> event;
		{
			std::lock_guard<std::mutex> lock(this->_ev_mtx);
			if (this->_events.empty()) return false;
			event = this->_events.front();
			this->_events.pop();
		}
		event->execute();
		return true;
	}

	void Application::getConsoleSize(int& width, int& height) {
#if CHH_IS_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		width = w.ws_col;
		height = w.ws_row;
#endif
	}

#if CHH_IS_WINDOWS
	Application::Application()
		: Widget(NULL), _should_exit(false), _thrd_id(std::this_thread::get_id()) {
		this->_ihandle = GetStdHandle(STD_INPUT_HANDLE);
		this->_ohandle = GetStdHandle(STD_OUTPUT_HANDLE);
		HWND hWnd = GetConsoleWindow();
		char buffer[129]; GetClassNameA(hWnd, buffer, 128);
		if (std::string(buffer) == "ConsoleWindowClass") {
			printf("Unsupported console. Please use Windows Terminal.");
			this->_should_exit = true;
			return;
		}
		CONSOLE_CURSOR_INFO cci;
		GetConsoleCursorInfo(this->_ohandle, &cci);
		cci.bVisible = false;
		SetConsoleCursorInfo(this->_ohandle, &cci);
		setlocale(LC_ALL, "zh_cn.utf8");
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
	}
#elif CHH_IS_LINUX
	Application::Application()
		: Widget(NULL), _should_exit(false), _displaying(0), _focus(0), _thrd_id(std::this_thread::get_id()) {
		setlocale(LC_ALL, "zh_cn.utf8");
	}
#endif

#if CHH_IS_WINDOWS
	Application::~Application() {
		CloseHandle(this->_ihandle);
		CloseHandle(this->_ohandle);
		const auto& childList = this->children();
		if (!childList.empty()) {
			delete childList.front();
		}
		// 此时再调用 Widget::~Widget() 也没事了。
	}
#elif CHH_IS_LINUX
	Application::~Application() = default;
#endif

	Key Application::getch() {
#if CHH_IS_WINDOWS
		INPUT_RECORD input;
		DWORD count;
		if (PeekConsoleInput(this->_ihandle, &input, 1, &count)) {
			if (count == 0) return Key();
			ReadConsoleInput(this->_ihandle, &input, 1, &count);
			if (input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown) {
				return Key(input.Event.KeyEvent.uChar.UnicodeChar,
					input.Event.KeyEvent.wVirtualKeyCode);
			}
		}
		return Key();
#else CHH_IS_LINUX
		termios oldt, newt;
		int ch;
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		return Key(ch, ch);
#endif
	}

	bool Application::isMainThread() const {
		return std::this_thread::get_id() == this->_thrd_id;
	}

	void Application::postEvent(std::shared_ptr<Event> event) {
		if (this->isMainThread()) throw std::runtime_error("Will cause deadlock.");
		else {
			std::lock_guard<std::mutex> lock(this->_ev_mtx);
			this->_events.push(event);
		}
	}

	void Application::tryPostEvent(std::shared_ptr<Event> event) {
		if (this->isMainThread()) event->execute();
		else {
			std::lock_guard<std::mutex> lock(this->_ev_mtx);
			this->_events.push(event);
		}
	}

	std::string Application::onRender(bool focus) {
		if (this->children().size() != 1) return "";
		if (!this->children().front()->visible()) return "";
		return this->children().front()->onRender(true);
	}

	bool Application::onKeyPress(Key key) {
		if (this->children().size() != 1) return false;
		if (!this->children().front()->visible()) return false;
		return this->children().front()->onKeyPress(key);
	}

	void Application::render() {
		printf("\033[H"); // 移动光标到 (0, 0) 。
		std::string temp = this->onRender(true) + "\n";
		int width, height;
		getConsoleSize(width, height);
		// 用空格填充每一行。
		char* buffer = new char[width + 1];
		memset(buffer, ' ', width);
		buffer[width] = 0;
		int y = 0, x = 0;
		std::ostringstream oss;
		for (const auto& i : temp) {
			if (y >= height - 1) break;
			switch (i) {
			default: {
				if (x >= width); // fallthrough 到换行。
				else {
					buffer[x] = i;
					x++;
					break;
				}
				[[fallthrough]];
			}
			case '\n': {
				oss << buffer << "\n";
				memset(buffer, ' ', width);
				buffer[width] = 0;
				y++;
				// 挺有趣的：y++ 相当于 \n, x=0 相当于 \r。
				[[fallthrough]];
			}
			case '\r': {
				x = 0;
				break;
			}
			case '\b': {
				x--;
				break;
			}
			}
		}
		printf("%s", oss.str().c_str());
		memset(buffer, ' ', width);
		buffer[width] = 0;
		while (true) {
			if (y >= height - 1) break;
			printf("%s", buffer);
			y++;
		}
	}

	void hti::Application::mainloop() {
		if (!_should_exit) this->render(); // 先渲染。
		Key key;
		while (!_should_exit) {
			if (this->processEvent()) this->render(); // 有事件时刷新。
			if (!(key = this->getch()).isNone()) {
				this->onKeyPress(key);
				this->render(); // 处理按键后刷新。
			}
			sleep_ms(50);
		}
#if CHH_IS_WINDOWS
		system("cls");
#else CHH_IS_LINUX
		system("clear");
#endif
	}

	void Application::exit() {

		if (this->isMainThread()) {
			this->_should_exit = true;
			return;
		}

		this->postEvent(std::make_shared<LambdaEvent>([self = this](Event*) {
			self->_should_exit = true;
			}));
	}

	void Application::loadLanguage(const std::string& name, const std::string& content) {
		this->_languages.load(name, content);
	}

	void Application::loadLanguageFromFile(const std::string& name, const std::string& file_name) {
		this->_languages.load(name, chh::toString(chh::readFile(file_name)));
	}

#if CHH_IS_WINDOWS
	void Application::loadLanguageFromResource(const std::string& name, const size_t& res_name, const std::string& res_type) {
		this->_languages.load(name, chh::toString(chh::readResource(res_name, res_type)));
	}
#elif CHH_IS_LINUX
#endif

	void Application::switchLanguage(const std::string& name) {
		this->_languages.current(name);
	}

}
