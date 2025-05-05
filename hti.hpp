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
            std::string current_name() const;
            // 获取当前选中的语言
            const Language* current() const;
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
            mutable const Language* _cache_language;
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

    typedef unsigned short Style;

    class Key {
        unsigned int _key;
        unsigned int _keycode;
    public:
        Key(unsigned int key = 0, unsigned int keycode = 0);
        unsigned int key();
        bool isNone();
        bool isPrev();
        bool isNext();
        bool isPress();
        bool isUp();
        bool isDown();
        bool isLeft();
        bool isRight();
    };

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
        // 执行函数（Lambda）
        void execute() override;
    };

    // 控件
    namespace widgets {

        // 控件（抽象类）
        class Widget {
            Application* const _app;
            Widget* const _parent;
            std::list<Widget*> _children;
            std::list<Widget*>::iterator _app_iter;
            std::list<Widget*>::iterator _parent_iter;
            bool _visible;
            friend class Application;
            // 注意，如果不是主线程则会崩溃。
            std::list<Widget*>& children();
        protected:
            Widget(Widget* parent);
        public:
            // 获取孩子的迭代器
            // 注意，如果不是主线程则会崩溃。
            std::list<Widget*>::iterator children_begin();
            // 获取孩子的迭代器
            // 注意，如果不是主线程则会崩溃。
            std::list<Widget*>::iterator children_end();
            // 获取孩子大小
            size_t children_size();
            template <typename T, typename... Args>
            // 加入新的子控件并返回
            // 类似工厂函数。生命周期由 Application 管理！
            T* add(Args... args) {
                static_assert(std::is_base_of<Widget, T>::value,
                    "T must derive from Widget");
                if (this->children().size() > 0 && this == (Widget*)this->app()) {
                    throw std::runtime_error("Application can only have one root child");
                }
                auto* widget = new T(this, std::forward<Args>(args)...);
                // 已完成初始化。
                this->app()->tryPostEvent(std::make_shared<LambdaEvent>([widget](Event*) {
                    widget->_app_iter = widget->_app->_widgets.insert(
                        widget->_app->_widgets.end(), widget);
                    widget->_parent_iter = widget->_parent->_children.insert(
                        widget->_parent->_children.end(), widget);
                    widget->_parent->onChildAdd();
                    }));
                return widget;
            }
            // 禁止深复制。
            Widget(const Widget&) = delete;
            // 禁止深复制。
            Widget& operator=(const Widget&) = delete;
            // 销毁控件
            // 一并从父控件的 _children 中删除。
            virtual ~Widget();
            // 获取应用
            Application* app() const;
            // 获取父控件
            Widget* parent() const;
            // 获取子控件（副本）
            const std::list<Widget*> children_copy() const;
            // 获取当前显示状态
            bool visible();
            // 设置当前显示状态
            void visible(bool visible);
            // 是否可以被选中（常量）
            // 默认返回 false。
            virtual bool canBeSelected() const;
            // 返回渲染内容
            // 在主线程运行。
            virtual std::string onRender(bool focus);
            // 处理按键
            // 在主线程运行。
            virtual bool onKeyPress(Key key);
            // 当添加了一个子控件时
            virtual void onChildAdd();
            // 当获得焦点
            virtual void onFocusGained();
            // 当焦点没了
            virtual void onFocusLost();
        };

        // 可被选中的（抽象类）
        class SelectableWidget : virtual public Widget {
            friend class Widget;
        protected:
            SelectableWidget(Widget* parent);
        public:
            // 可被选中
            // 返回 true。
            bool canBeSelected() const override;
        };

        // 文本控件（抽象类）
        class TextWidget : virtual public Widget {
            i18n::Text _text;
            friend class Widget;
        protected:
            TextWidget(Widget* parent, i18n::Text text = {});
        public:
            virtual ~TextWidget();
            // 返回文字
            i18n::Text text() const;
            // 设置文字
            void text(i18n::Text text);
        };

        // 文本
        class Label : public TextWidget {
            friend class Widget;
        protected:
            Label(Widget* parent, i18n::Text text = {});
        public:
            // 返回渲染内容
            std::string onRender(bool focus) override;
        };

        typedef std::function<void(Widget*)> WidFunc;

        // 文本
        class Button : public SelectableWidget, public TextWidget {
            WidFunc _action = nullptr;
            friend class Widget;
        protected:
            Button(Widget* parent, i18n::Text text = {});
        public:
            // 返回渲染内容
            std::string onRender(bool focus) override;
            // 绑定回调函数
            void bind(WidFunc action);
            // 处理按键
            bool onKeyPress(Key key) override;
            using SelectableWidget::canBeSelected;
        };

        class List : public SelectableWidget {
            std::list<Widget*>::iterator _index;
            Style _style;
            friend class Widget;
        protected:
            List(Widget* parent, Style style = STYLE_VERTICAL);
        public:
            // 竖着排列。
            const static int STYLE_VERTICAL = 0x0;
            // 横着排列。
            const static int STYLE_HORIZONTAL = 0x1;
            // 返回渲染内容
            std::string onRender(bool focus) override;
            // 处理按键
            bool onKeyPress(Key key) override;
            // 当添加了一个子控件时
            // 找到一个能被选中的控件。
            void onChildAdd() override;
        };

        // 类似列表，但仅显示当前选中的控件。
        class Pages : public SelectableWidget {
            std::list<Widget*>::iterator _index;
            friend class Widget;
        protected:
            Pages(Widget* parent);
        public:
            // 返回渲染内容
            std::string onRender(bool focus) override;
            // 处理按键
            bool onKeyPress(Key key) override;
            // 当添加了一个子控件时
            // 找到一个能被选中的控件。
            void onChildAdd() override;
            // 选择前一个
            // 返回是否成功。
            bool selPrev();
            // 选择后一个
            // 返回是否成功。
            bool selNext();
            // 选择第一个
            void selBegin();
            // 选择最后一个
            void selEnd();
            // 选择
            // 效率O(n)！
            void select(size_t index);
            // 可被选中
            // 返回当前控件能否被选中。
            bool canBeSelected() const override;
        };

        class PageStack : public TextWidget, public SelectableWidget {
            typedef std::list<std::pair<i18n::Text, std::list<Widget*>::iterator>> l;
            l _navigation;
            l::iterator _it;
            Pages* _pages;
            std::map<Key, i18n::Text> _help;
            Style _style;
            bool _pos = 0;
            friend class Widget;
        protected:
            PageStack(Widget* parent, i18n::Text title = {}, Style style = STYLE_UP_DOWN);
            ~PageStack();
        public:
            // 上边是导航栏，下边是内容。
            const static int STYLE_UP_DOWN = 0x0;
            // 左边是导航栏，右边是内容。（暂时不打算实现）
            //const static size_t STYLE_LEFT_RIGHT = 0x1;
            // 在导航栏目添加一个页面。
            template <typename T, typename... Args>
            T* addPage(i18n::Text name, Args... args) {
                auto* ret = this->_pages->add<T>((args)...);
                this->app()->tryPostEvent(std::make_shared<LambdaEvent>([self = this, name](Event* ev) {
                    auto it = self->_pages->children_end();
                    it--;
                    self->_navigation.push_back({ name, it });
                    if (self->_it == self->_navigation.end()) {
                        self->_it = self->_navigation.begin();
                    }
                    }));
                return ret;
            }
            // 返回渲染内容
            std::string onRender(bool focus) override;
            // 处理按键
            bool onKeyPress(Key key) override;
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
        HANDLE _ihandle;
        HANDLE _ohandle;
#endif
        /* 语言管理 */
        i18n::LanguageManager _languages;
        bool processEvent();
        void getConsoleSize(int& width, int& height);
        friend class Widget;
    public:
        Application();
        ~Application();

        /* 其它功能 */
        // 读取键盘一个按键
        // 不阻塞，如果没有按键返回Key(0,0)。
        Key getch();

        /* 线程安全 */
        // 判断调用方是不是主线程
        bool isMainThread() const;
        // 用于工作线程加入事件
        // 如果调用方是主线程，崩溃。
        void postEvent(std::shared_ptr<Event> event);
        // 用于工作线程加入事件
        // 如果调用方是主线程，直接执行，不同于 postEvent。
        // 提高代码复用率。
        void tryPostEvent(std::shared_ptr<Event> event);

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
        bool onKeyPress(Key key) override;

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