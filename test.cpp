#include "hti.hpp"
#include <thread>
#include <chrono>

using namespace hti;
using namespace hti::widgets;
using namespace hti::i18n;

// 本地化字符串辅助函数
Text i18n_(std::string key) {
    return LocalizingString(key);
}

int main() {
    // 1. 初始化应用
    auto* app = new Application();

    // 2. 加载多语言资源
    app->loadLanguage("zh", R"({
        "main_title": "HTI演示",
        "page1": "页面1",
        "page2": "页面2", 
        "page3": "语言", 
        "counter": "计数器",
        "incr": "增加",
        "decr": "减少",
        "exit": "退出"
    })");
    app->loadLanguage("en", R"({
        "main_title": "HTI Demonstration",
        "page1": "Page 1",
        "page2": "Page 2", 
        "page3": "Languages", 
        "counter": "Counter",
        "incr": "Increase",
        "decr": "Decrease",
        "exit": "Exit"
    })");
    app->switchLanguage("zh");

    // 3. 创建主界面结构
    auto* root = app->add<List>();  // 根容器

    // 4. 添加页面堆栈控件
    auto* stack = root->add<PageStack>(i18n_("main_title"));

    // 5. 添加页面1 - 简单文本
    stack->addPage<Label>(i18n_("page1"), "这是页面1的内容");

    // 6. 添加页面2 - 计数器功能
    auto* page2 = stack->addPage<List>(i18n_("page2"));
    long count = 0;

    // 6.1 计数器UI组件
    page2->add<Label>(i18n_("counter"));  // 标题
    auto* countLabel = page2->add<Label>("0");  // 计数显示

    // 6.2 操作按钮(水平布局)
    auto* btnGroup = page2->add<List>(List::STYLE_HORIZONTAL);
    btnGroup->add<Button>(i18n_("incr"))->bind([&](Widget*) {
        countLabel->text(std::to_string(++count));
        });
    btnGroup->add<Button>(i18n_("decr"))->bind([&](Widget*) {
        countLabel->text(std::to_string(--count));
        });

    // 7. 添加页面3 - 语言
    auto* page3 = stack->addPage<List>(i18n_("page3"));
    page3->add<Button>("中文")->bind([&](Widget*) {
        app->switchLanguage("zh");
        });
    page3->add<Button>("English")->bind([&](Widget*) {
        app->switchLanguage("en");
        });

    // 8. 线程安全演示 - 每秒更新计数
    std::thread([app, countLabel]() {
        for (int i = 0;; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            app->tryPostEvent(std::make_shared<LambdaEvent>([countLabel, i](Event*) {
                countLabel->text("Tick: " + std::to_string(i));
                }));
        }
        }).detach();

    // 9. 添加退出按钮
    root->add<Button>(i18n_("exit"))->bind([app](Widget*) {
        app->exit();
        });

    // 10. 启动主循环
    app->mainloop();

    // 11. 清理资源
    delete app;
    return 0;
}
