## 语言

[English](README.md) [繁體中文](README.TC.md)

# HTI (HillQiu的文本用户界面)
**[⚠️AI生成，人工审核]**

一个轻量级、线程安全的文本用户界面库，用于使用现代C++构建跨平台终端应用。

## 重要提示
⚠️ **Application只能有一个根子控件！** 请始终使用`PageStack`、`List`或其他布局控件作为Application的根子控件。

## 快速入门

### 你好世界示例

```cpp
#include "hti.hpp"

int main() {
    // 创建应用实例
    auto* app = new hti::Application;

    // Application必须有一个根子控件(通常是List)
    auto* rootList = app->make<hti::widgets::List>(app);

    // 在根List中创建标签
    auto* text = app->make<hti::widgets::Label>(rootList, "你好世界!");

    // 创建带点击处理器的按钮
    auto* btn = app->make<hti::widgets::Button>(rootList, "点击我");
    btn->bind([btn](hti::widgets::Widget*) {
        btn->text("已点击!");
    });

    // 启动主事件循环
    app->mainloop();

    // 清理资源(自动删除所有子控件)
    delete app;
    return 0;
}
```

## 架构概述

### 核心组件
- **Application**: 管理事件循环、控件层次结构和渲染(只能有一个根子控件)
- **Widget**: 所有UI组件的基类，支持父子关系
- **事件系统**: 线程安全的事件队列，主线程分发

### 主要特性
- 🖥️ **跨平台** (支持Windows/Linux控制台)
- 🌍 **内置国际化** (基于JSON的语言包)
- 🧩 **可扩展控件系统** (使用`make<T>`工厂模式)
- ⚡ **线程安全** (自动主线程分发)
- 🛠️ **简洁API** (直观的控件层次结构)

## 线程安全模型

HTI通过以下方式确保线程安全：
1. 主线程验证(`isMainThread()`)
2. 非UI线程自动事件排队
3. 关键部分的原子操作

```cpp
// 安全的跨线程控件更新
app->tryPostEvent([label](Event*) {
    label->text("从工作线程更新");
});
```

## 控件系统深入

### 控件生命周期
1. 通过`app->make<T>()`工厂创建
2. 自动注册到父控件的子控件列表
3. 父控件删除时自动销毁

### 事件处理
```cpp
button->bind([](Widget* btn) {
    // 处理器在主线程运行
    auto* button = dynamic_cast<Button*>(btn);
    button->text("已点击!");
});
```

## 性能考虑

1. **渲染**:
   - 使用双缓冲减少闪烁
   - 优化控制台输出(批量写入)

2. **内存**:
   - 控件树自动清理
   - 事件对象使用共享指针

3. **事件处理**:
   - 批量事件执行
   - 轮询间10ms休眠

## 高级用法

### 自定义控件示例
```cpp
class CustomWidget : public Widget, public TextWidget {
public:
    CustomWidget(Widget* parent, Text text) 
        : Widget(parent), TextWidget(parent, text) {}
        
    std::string onRender(bool focus) override {
        return focus ? "[" + text() + "]" : text();
    }
};

// 使用
auto* custom = app->make<CustomWidget>(parent, "自定义");
```

### 语言包格式
```json
{
    "key": "翻译",
    "welcome": "你好世界"
}
```

## API参考(关键类)

| 类 | 描述 |
|-------|-------------|
| `Application` | 主入口点，管理事件循环(只能有一个根子控件) |
| `Widget` | 所有UI组件的基类 |
| `Button` | 带回调的可点击按钮 |
| `Label` | 支持国际化的文本显示 |
| `List` | 垂直/水平容器(推荐作为根子控件) |

## 最佳实践

1. **控件创建**:
   - 始终使用`app->make<T>()`而非`new`
   - 短生命周期控件优先栈分配
   - ⚠️ Application必须有一个根子控件(使用List作为根)

2. **事件处理**:
   - 保持处理器简短高效
   - 跨线程更新使用`tryPostEvent`

3. **性能**:
   - 最小化渲染调用
   - 批量更新控件
