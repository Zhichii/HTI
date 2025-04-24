#include "hti.hpp"
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>

using namespace hti;
using namespace hti::widgets;
using namespace hti::i18n;

// 自定义带标题的列表控件
// 继承自TextWidget和List，展示组合继承用法
class TitleList : public TextWidget, public List {
public:
    TitleList(Widget* parent, Text text = {})
        : Widget(parent), TextWidget(parent, text), List(parent) {
    }
    std::string onRender(bool focus) override {
        std::ostringstream oss;
        // 根据焦点状态渲染不同标题样式
        if (focus) oss << "<" << text().localize(app()->languages()) << ">\n";
        else oss << " " << text().localize(app()->languages()) << " \n";
        oss << List::onRender(focus);
        return oss.str();
    }
};

int main() {
    // 1. 创建应用实例
    auto* app = new Application();

    // 加载示例语言包
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

    // 2. 创建根控件（必须且只能有一个）
    auto* rootList = app->make<TitleList>(app, LocalizingString("main_title"));

    // 3. 创建计数器控件组
    auto* counterGroup = app->make<TitleList>(rootList, LocalizingString("counter"));
    long long counter = 0;
    auto* counterLabel = app->make<Label>(counterGroup, "0");

    // 4. 创建控制按钮
    auto* prevBtn = app->make<Button>(counterGroup, LocalizingString("decr"));
    auto* nextBtn = app->make<Button>(counterGroup, LocalizingString("incr"));

    // 5. 绑定按钮事件
    prevBtn->bind([&](Widget*) {
        counter--;
        counterLabel->text(std::to_string(counter));
        });

    nextBtn->bind([&](Widget*) {
        counter++;
        counterLabel->text(std::to_string(counter));
        });

    // 6. 添加退出按钮
    auto* exitBtn = app->make<Button>(rootList, LocalizingString("exit"));
    exitBtn->bind([app](Widget*) {
        app->exit();
        });

    // 7. 演示线程安全操作
    std::thread worker([app, counterLabel]() {
        for (int i = 0; ; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // 安全地从工作线程更新UI
            app->tryPostEvent(std::make_shared<LambdaEvent>([counterLabel, i](Event*) {
                counterLabel->text("Tick: " + std::to_string(i));
                }));
        }
        });
    worker.detach();

    // 8. 启动主循环
    app->mainloop();

    // 9. 清理资源
    delete app;
    return 0;
}
