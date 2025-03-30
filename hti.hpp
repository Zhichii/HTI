#pragma once

#include "chh.hpp"

// 一个轻量级的 TUI（文本用户界面）库
namespace hti {

    // 国际化
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
            // 从 JSON 文件中加载语言
            void loadFile(const std::string& name, const std::string& file_name);
#if CHH_IS_WINDOWS
            // 从 Windows 资源中加载语言
            // 该函数仅在 Windows 平台上可用。
            void loadResource(std::string name, size_t res_name, std::string res_type);
#elif CHH_IS_LINUX
            // 从 Windows 资源中加载语言
            // 该函数在 Linux 平台上不可用。
            void loadResource(std::string name, size_t res_name, std::string res_type) = delete;
#endif
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

    // 事件类型
    enum EventType {
        EV_EXIT, EV_TEXT, EV_UPDATE
    };

    // 事件
    // EventQueue 会自动释放 Event！
    class Event {
        EventType _type;
        EventFunc _func;
    public:
        Event(EventType type, EventFunc func = [](Event*){});
        // 获取回调函数
        // 回调函数应在主线程运行。
        EventFunc func() const;
        // 获取事件类型
        const EventType& type() const {
            return this->_type;
        }
    };

    namespace widgets {
        class Widget;
    }

    // 程序入口
    class Application {
        class ExitEvent : Event {
        public:
            ExitEvent(Application* app);
        private:
            Application* _app;
        };
        std::mutex                      _mutex;
        std::vector<widgets::Widget*>   _widgets;
        i18n::LanguageManager           _languages;
        std::queue<Event*>              _queue;
        size_t                          _focus;
        size_t                          _displaying;
        bool                            _should_exit;
        Event* popEvent();
    public:
        Application();
        ~Application();
        // 加入事件
        void pushEvent(Event* ev);
        // 主循环
        void mainloop();
        // 退出
        void exit();
    };

    // 控件
    namespace widgets {

        // 控件基类
        class Widget {
            Application*        _app;
            Widget*             _parent;
            std::list<Widget*>  _children;
        public:
            Widget(Widget* parent);
            ~Widget() {
                for (auto& i : this->_children) delete i;
            }
            Application* app() const { return this->_app; }
            Widget* parent() const { return this->_parent; }
            virtual std::string onRender();
        };

        // 带有文本的控件的基类。
        class TextWidget : Widget {
            class TextEvent : Event {
            public:
                TextEvent(TextWidget* widget, i18n::Text text);
            private:
                TextWidget* _widget;
                i18n::Text _text;
            };
            i18n::Text _text;
        public:
            i18n::Text text() const;
            void text(i18n::Text text);
        };

        // 文本。
        class Label : TextWidget {
        public:
            Label(i18n::Text text);
        };

    }

}