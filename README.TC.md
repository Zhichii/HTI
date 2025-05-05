## 語言

[English](README.md) [简体中文](README.SC.md)

# HTI (HillQiu的文字使用者介面)
**[⚠️AI生成，人工審核]**

一個輕量級、執行緒安全的文字使用者介面庫，用於使用現代C++構建跨平台終端應用程式。

## 重要提示
⚠️ **Application只能有一個根子控制項！** 請始終使用`PageStack`、`List`或其他佈局控制項作為Application的根子控制項。

## 快速入門

### 你好世界範例

```cpp
#include "hti.hpp"

int main() {
    // 建立應用實例
    auto* app = new hti::Application;

    // Application必須有一個根子控制項(通常是List)
    auto* rootList = app->make<hti::widgets::List>(app);

    // 在根List中建立標籤
    auto* text = app->make<hti::widgets::Label>(rootList, "你好世界!");

    // 建立帶點擊處理器的按鈕
    auto* btn = app->make<hti::widgets::Button>(rootList, "點擊我");
    btn->bind([btn](hti::widgets::Widget*) {
        btn->text("已點擊!");
    });

    // 啟動主事件循環
    app->mainloop();

    // 清理資源(自動刪除所有子控制項)
    delete app;
    return 0;
}
```

## 架構概述

### 核心元件
- **Application**: 管理事件循環、控制項層次結構和渲染(只能有一個根子控制項)
- **Widget**: 所有UI元件的基類，支援父子關係
- **事件系統**: 執行緒安全的事件佇列，主執行緒分發

### 主要特性
- 🖥️ **跨平台** (支援Windows/Linux控制台)
- 🌍 **內建國際化** (基於JSON的語言包)
- 🧩 **可擴展控制項系統** (使用`make<T>`工廠模式)
- ⚡ **執行緒安全** (自動主執行緒分發)
- 🛠️ **簡潔API** (直觀的控制項層次結構)

## 執行緒安全模型

HTI通過以下方式確保執行緒安全：
1. 主執行緒驗證(`isMainThread()`)
2. 非UI執行緒自動事件排隊
3. 關鍵部分的原子操作

```cpp
// 安全的跨執行緒控制項更新
app->tryPostEvent([label](Event*) {
    label->text("從工作執行緒更新");
});
```

## 控制項系統深入

### 控制項生命週期
1. 通過`app->make<T>()`工廠建立
2. 自動註冊到父控制項的子控制項列表
3. 父控制項刪除時自動銷毀

### 事件處理
```cpp
button->bind([](Widget* btn) {
    // 處理器在主執行緒運行
    auto* button = dynamic_cast<Button*>(btn);
    button->text("已點擊!");
});
```

## 效能考慮

1. **渲染**:
   - 使用雙緩衝減少閃爍
   - 優化控制台輸出(批量寫入)

2. **記憶體**:
   - 控制項樹自動清理
   - 事件物件使用共享指標

3. **事件處理**:
   - 批量事件執行
   - 輪詢間10ms休眠

## 進階用法

### 自定義控制項範例
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
auto* custom = app->make<CustomWidget>(parent, "自定義");
```

### 語言包格式
```json
{
    "key": "翻譯",
    "welcome": "你好世界"
}
```

## API參考(關鍵類)

| 類 | 描述 |
|-------|-------------|
| `Application` | 主入口點，管理事件循環(只能有一個根子控制項) |
| `Widget` | 所有UI元件的基類 |
| `Button` | 帶回調的可點擊按鈕 |
| `Label` | 支援國際化的文字顯示 |
| `List` | 垂直/水平容器(推薦作為根子控制項) |

## 最佳實踐

1. **控制項建立**:
   - 始終使用`app->make<T>()`而非`new`
   - 短生命週期控制項優先棧分配
   - ⚠️ Application必須有一個根子控制項(使用List作為根)

2. **事件處理**:
   - 保持處理器簡短高效
   - 跨執行緒更新使用`tryPostEvent`

3. **效能**:
   - 最小化渲染呼叫
   - 批量更新控制項
