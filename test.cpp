#include "hti.hpp"
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>

using namespace hti;
using namespace hti::widgets;
using namespace hti::i18n;

class TitleList : public TextWidget, public List {
    friend class Widget;
protected:
    TitleList(Widget* parent, Text text = {})
        : Widget(parent), TextWidget(parent, text), List(parent) {}
public:
    std::string onRender(bool focus) override {
        std::ostringstream oss;
        if (focus) oss << "<" << text().localize(app()->languages()) << ">\n";
        else oss << " " << text().localize(app()->languages()) << " \n";
        oss << List::onRender(focus);
        return oss.str();
    }
};

int main() {
    try {
        auto* app = new Application();
        
        app->loadLanguage("zh", R"({
            "main_title": "主界面",
            "counter": "计数器",
            "decr": "减少",
            "incr": "增加",
            "exit": "退出"
        })");
        
        app->loadLanguage("en", R"({
            "main_title": "Main Menu",
            "counter": "Counter",
            "decr": "Decrease",
            "incr": "Increase",
            "exit": "Exit"
        })");
        app->switchLanguage("zh");

        // 添加根列表
        auto* rootList = app->add<PageStack>(LocalizingString("main_title"));
        
        rootList->addPage<Button>("Page1", "hi");
        rootList->addPage<Label>("Page2", "what?");

        app->mainloop();
        delete app;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
