#pragma once

#include "chh.hpp"

/**
 * @brief 一个轻量级的 TUI（文本用户界面）库。
 */
namespace hti {

    /**
     * @brief 国际化。
     */
    namespace i18n {

        /**
         * @brief 单个语言。
         */
        class Language {
        private:
            std::map<std::string, std::string> _content;
        public:
            /**
             * @brief 默认构造器。
             */
            Language();
            /**
             * @brief 从 JSON 字符串中加载语言。
             * @param content JSON 字符串
             * @throw std::runtime_error 如果 JSON 解析失败。
             */
            Language(const std::string& content);
            /**
             * @brief 从键名本地化。
             * @param key 键名。
             * @return 本地化后的字符串。
             * @return 如果键名不存在，直接返回键值。
             */
            std::string localize(const std::string& key) const;
        };

        /**
         * @brief 管理所有的语言。
         */
        class LanguageManager {
        private:
            std::map<std::string, Language> _languages;
            std::string _current;
        public:
            /**
             * @brief 默认构造器。
             * @note 默认初始化一个叫做 default 的语言。
             */
            LanguageManager();
            /**
             * @brief 从 JSON 字符串中加载语言。
             * @param name 语言名称。
             * @param content JSON 字符串。
             * @throw std::runtime_error 如果 JSON 解析失败。
             */
            void load(const std::string& name, const std::string& content);
            /**
             * @brief 从 JSON 文件中加载语言。
             * @param name 语言名称。
             * @param file_name 文件名。
             * @throw std::runtime_error 如果文件读取或 JSON 解析失败。
             */
            void loadFile(const std::string& name, const std::string& file_name);
#if CHH_IS_WINDOWS
            /**
             * @brief 从 Windows 资源中加载语言。
             * @note 该函数仅在 Windows 平台上可用。
             * @param name 语言名称。
             * @param res_name 资源 ID 。
             * @param res_type 资源类型。
             * @throw std::runtime_error 如果资源读取或 JSON 解析失败。
             */
            void loadResource(std::string name, size_t res_name, std::string res_type);
#elif CHH_IS_LINUX
            /**
             * @brief 从 Windows 资源中加载语言。
             * @warning 该函数仅在 Windows 平台上可用。
             * @param name 语言名称。
             * @param res_name 资源 ID 。
             * @param res_type 资源类型。
             * @throw std::runtime_error 如果资源读取或 JSON 解析失败。
             */
            void loadResource(std::string name, size_t res_name, std::string res_type) = delete;
#endif
            /**
             * @brief 切换语言。
             * @brief 如果该语言不存在，则不执行操作。
             * @param name 语言名称。
             */
            void switchLanguage(std::string name);
            /**
             * @brief 从键名本地化。
             * @param key 键名。
             * @return 本地化后的字符串。
             * @return 如果键名不存在，直接返回键值。
             */
            std::string localize(std::string key) const;
            /**
             * @brief 获取当前选中的语言。
             * @return 当前选中的语言的名称。
             */
            std::string current() const;
        };

        /**
         * @brief 本地化单个键名。
         * @note 请使用 Text 将多个 LocalizingString 与 std::string 拼接。
         */
        class LocalizingString {
        private:
            std::string _key;
        public:
            /**
             * @brief 初始化键名。
             */
            LocalizingString(std::string key);
            /**
             * @brief 从键名本地化。
             * @param languages 语言管理器。
             * @return 本地化后的字符串。
             * @return 如果键名不存在，直接返回键值。
             */
            std::string localize(const LanguageManager& languages) const;
        };

        /**
         * @brief 将多个 LocalizingString 与 std::string 拼接。
         */
        class Text {
        private:
            std::vector<std::variant<LocalizingString, std::string>> _parts;
            mutable std::string _cache;
            mutable std::string _cache_language;
        public:
            /**
             * @brief 默认构造器。
             */
            Text();
            /**
             * @brief 拷贝构造器。
             */
            Text(const Text& text);
            /**
             * @brief 从 LocalizingString 构造。
             */
            Text(const LocalizingString& string);
            /**
             * @brief 从 std::string 构造。
             */
            Text(const std::string& string);
            /**
             * @brief 从 C 风格字符串构造。
             */
            Text(const char* string);
            /**
             * @brief 与另一个 Text 拼接。
             * @note LocalizingString、std::string 和 C 风格字符串会自动从 Text 构造器转换。
             */
            Text operator+(const Text& that) const;
            /**
             * @brief 追加另一个 Text 。
             * @note LocalizingString、std::string 和 C 风格字符串会自动从 Text 构造器转换。
             */
            Text& operator+=(const Text& that);
            /**
             * @brief 从键名本地化。
             * @param languages 语言管理器。
             * @return 本地化后的字符串。
             * @return 如果键名不存在，直接返回键值。
             */
            std::string localize(const LanguageManager& languages) const;
            /**
             * @brief 获取部件的数量。
             * @return 部件的数量。
             */
            size_t size() const;
        };

    }

    class Event;
    /**
     * @brief 事件回调函数。
     * @note 不要释放 Event！EventQueue 会自动释放 Event！
     */
    typedef std::function<void(Event*)> EventFunc;

    enum EventType {
        EV_EXIT, EV_RENDER
    };

    /**
     * @brief 事件。
     * @note EventQueue 会自动释放 Event！
     */
    class Event {
    public:
        /**
         * @brief 构造器。
         */
        Event(EventType type, EventFunc func = [](Event*){});
        /**
         * @brief 获取回调函数。
         * @return 回调函数。
         * @note 回调函数应在主线程运行。
         */
        EventFunc func() const;
        /**
         * @brief 获取事件类型。
         * @return 事件类型。
         */
        const EventType& type() const {
            return this->_type;
        }
    private:
        EventType _type;
        EventFunc _func;
    };

    namespace widgets {
        class Widget;
    }

    /**
     * @brief 整个程序的入口。
     */
    class Application {
    public:
        /**
         * @brief 构造器。
         */
        Application();
        /**
         * @brief 加入一个事件。
         */
        void pushEvent(Event* ev);
        /**
         * @brief 主循环。
         */
        void mainloop();
        /**
         * @brief 退出。
         */
        void exit();
    private:
        class ExitEvent : Event {
        public:
            ExitEvent(Application* app);
        private:
            Application* _app;
        };
        std::vector<widgets::Widget*>    _widgets;
        i18n::LanguageManager           _languages;
        std::mutex                      _mutex;
        std::queue<Event*>              _queue;
        size_t                          _focus;
        size_t                          _displaying;
        bool                            _should_exit;
        Event* popEvent();
    };

    /**
     * @brief 控件。
     */
    namespace widgets {

        /**
         * @brief 控件。
         */
        class Widget {
        public:
            Widget(Widget* parent);
            ~Widget() {
                for (auto& i : this->_children) delete i;
            }
            virtual void onRender();
        private:
            Application*        _app;
            Widget*             _parent;
            std::list<Widget*>  _children;
            i18n::Text			_text;
            std::string         _render_cache;
            bool				_focus;
        };

    }

}