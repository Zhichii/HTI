#pragma once

#include "chh.hpp"

// 一个轻量级的 TUI（文本用户界面）库
namespace hti {

    // 国际化（不自带多线程支持）
    namespace i18n {

        // 语言
        class Language {
            std::map<std::string, std::string> _content;
        public:
            Language();
            // 从 JSON 字符串中加载语言。
            Language(const std::string& content);
            // 从键名本地化
            // 如果键名不存在，直接返回键值。
            std::string localize(const std::string& key) const;
        };

        // 管理所有的语言
        class LanguageManager {
            std::map<std::string, Language> _languages;
            std::string _current;
        public:
            // 默认初始化一个叫做 default 的语言。
            LanguageManager();
            // 从 JSON 字符串中加载语言
            void load(const std::string& name, const std::string& content);
            // 从键名本地化
            // 如果键名不存在，直接返回键值。
            std::string localize(std::string key) const;
            // 获取当前选中的语言
            std::string current() const;
            // 切换语言
            void current(std::string name);
        };

        // 本地化单个键名
        // 请使用 Text 将多个 LocalizingString 与 std::string 拼接。
        class LocalizingString {
            std::string _key;
        public:
            // 从键名初始化
            LocalizingString(std::string key);
            // 本地化
            std::string localize(const LanguageManager& languages) const;
        };

        // 将多个 LocalizingString 与 std::string 拼接
        class Text {
            std::vector<std::variant<LocalizingString, std::string>> _parts;
            mutable std::string _cache;
            mutable std::string _cache_language;
        public:
            Text();
            Text(const Text& text);
            Text(const LocalizingString& string);
            Text(const std::string& string);
            Text(const char* string);
            // 与另一个 Text 拼接
            // LocalizingString、std::string 和 C 风格字符串会自动从 Text 构造器转换。
            Text operator+(const Text& that) const;
            // 追加一个 Text
            // LocalizingString、std::string 和 C 风格字符串会自动从 Text 构造器转换。
            Text& operator+=(const Text& that);
            // 从键名本地化
            // 如果键名不存在，直接返回键值。
            std::string localize(const LanguageManager& languages) const;
            // 获取拼接数量
            size_t size() const;
        };

    }

    class Event;
    // 事件回调函数
    // 不要释放 Event！EventQueue 会自动释放 Event！
    typedef std::function<void(Event*)> EventFunc;

    class Application;

    // 事件
    class Event {
    public:
        virtual ~Event();
        // 执行函数
        // 必须覆写。
        virtual void execute() = 0;
    };

    // 函数（Lambda）事件
    class LambdaEvent : public Event {
        EventFunc _action;
    public:
        LambdaEvent(EventFunc action);
        // 覆写执行函数
        void execute() override;
    };

    // 控件
    namespace widgets {

        class Widget;
        // 控件（基类）
        class Widget {
            Application* const _app;
            Widget* const _parent;
            std::list<Widget*> _children;
            std::list<Widget*>::iterator _app_iter;
            std::list<Widget*>::iterator _parent_iter;
        public:
            Widget(Widget* parent);
            // 禁止深复制。
            Widget(const Widget&) = delete;
            // 禁止深复制。
            Widget& operator=(const Widget&) = delete;
            // 销毁控件
            // 一并从父控件的 _children 中删除。
            ~Widget();
            // 获取应用
            Application* app() const;
            // 获取父控件
            Widget* parent() const;
            // 获取子控件
            const std::list<Widget*> children() const;
            // 返回渲染内容
            virtual std::string onRender(bool focus);
            // 处理按键
            virtual bool onKeyPress(char key);
        };

        // 文本控件（基类）
        class TextWidget : public Widget {
            i18n::Text _text;
        public:
            TextWidget(Widget* parent, i18n::Text text = {});
            // 返回文字
            i18n::Text text() const;
            // 设置文字
            void text(i18n::Text text);
        };

        // 文本
        class Label : public TextWidget {
        public:
            Label(Widget* parent, i18n::Text text = {});
            std::string onRender(bool focus) override;
        };

    }

    // 程序入口
    class Application : public widgets::Widget {
        /* 线程安全 */
        std::mutex _ev_mtx;
        std::thread::id const _thrd_id;
        /* 控件功能 */
        bool _should_exit;
        std::queue<std::shared_ptr<Event>> _events;
        std::list<widgets::Widget*> _widgets;
        std::list<Widget*>::iterator _focus;
#if CHH_IS_WINDOWS
        HANDLE _console;
#endif
        /* 语言管理 */
        i18n::LanguageManager _languages;
        bool processEvent();
        void getConsoleSize(int& width, int& height);
        friend class Widget;
    public:
        Application();
        ~Application();

        /* 线程安全 */
        // 判断调用方是不是主线程
        bool isMainThread() const;
        // 用于工作线程加入事件
        // 如果调用方是主线程，就直接执行。
        void postEventWithCheck(std::shared_ptr<Event> event);

        /* 控件功能 */
        // 渲染
        void render();
        // 主循环
        void mainloop();
        // 退出
        void exit();
        // 渲染根控件
        std::string onRender(bool focus) override;
        // 处理按键
        bool onKeyPress(char key) override;

        /* 语言管理 */
        // 获取语言管理器
        const i18n::LanguageManager& languages() { return this->_languages; }
        // 从 JSON 字符串加载语言
        void loadLanguage(const std::string& name, const std::string& content);
        // 从 JSON 文件加载语言
        void loadLanguageFromFile(const std::string& name, const std::string& file_name);
#if CHH_IS_WINDOWS
        // 从资源加载语言
        // 该函数仅在 Windows 平台上可用。
        void loadLanguageFromResource(const std::string& name, const size_t& res_name, const std::string& res_type);
#elif CHH_IS_LINUX
        // 从资源加载语言
        // 该函数在 Linux 平台上不可用。
        void loadLanguageFromResource(const std::string& name, const size_t& res_name, const std::string& res_type) = delete;
#endif
        // 切换语言
        void switchLanguage(const std::string& name);
    };

}