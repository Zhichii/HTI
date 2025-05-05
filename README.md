## Languages

[简体中文](README.SC.md) [繁體中文](README.TC.md)

# HTI (HillQiu's Text User Interface)
**[⚠️AI-generated with human review]**

A lightweight, thread-safe text-based UI library for building cross-platform terminal applications with modern C++.

## Important Note
⚠️ **Application can only have one root child!** Always use a `PageStack`, a `List` or other layout widget as the root child of Application.

## Quick Start

### Hello World Example

```cpp
#include "hti.hpp"

int main() {
    // Create application instance
    auto* app = new hti::Application;

    // Application must have exactly one root child (usually a List)
    auto* rootList = app->make<hti::widgets::List>(app);

    // Create label inside the root List
    auto* text = app->make<hti::widgets::Label>(rootList, "Hello World!");

    // Create button with click handler
    auto* btn = app->make<hti::widgets::Button>(rootList, "Click me");
    btn->bind([btn](hti::widgets::Widget*) {
        btn->text("Clicked!");
    });

    // Start main event loop
    app->mainloop();

    // Cleanup (automatically deletes all child widgets)
    delete app;
    return 0;
}
```

## Architecture Overview

### Core Components
- **Application**: Manages event loop, widget hierarchy and rendering (can only have one root child)
- **Widget**: Base class for all UI components with parent-child relationships
- **Event System**: Thread-safe event queue with main thread dispatching

### Key Features
- 🖥️ **Cross-platform** (Windows/Linux console support)
- 🌍 **Built-in i18n** (JSON-based language packs)
- 🧩 **Extensible Widget System** (Factory pattern with `make<T>`)
- ⚡ **Thread Safety** (Automatic main thread dispatching)
- 🛠️ **Simple API** (Intuitive widget hierarchy)

## Thread Safety Model

HTI enforces strict thread safety through:
1. Main thread verification (`isMainThread()`)
2. Automatic event queueing for non-UI threads
3. Atomic operations for critical sections

```cpp
// Safe cross-thread widget update
app->tryPostEvent([label](Event*) {
    label->text("Updated from worker thread");
});
```

## Widget System Deep Dive

### Widget Lifecycle
1. Creation via `app->make<T>()` factory
2. Automatic registration in parent's children list
3. Destruction when parent is deleted

### Event Handling
```cpp
button->bind([](Widget* btn) {
    // Handler runs on main thread
    auto* button = dynamic_cast<Button*>(btn);
    button->text("Clicked!");
});
```

## Performance Considerations

1. **Rendering**:
   - Uses double buffering to minimize flicker
   - Optimized console output with bulk writes

2. **Memory**:
   - Widget tree automatically cleaned up
   - Shared pointers for event objects

3. **Event Processing**:
   - Batched event execution
   - 10ms sleep between polls

## Advanced Usage

### Custom Widget Example
```cpp
class CustomWidget : public Widget, public TextWidget {
public:
    CustomWidget(Widget* parent, Text text) 
        : Widget(parent), TextWidget(parent, text) {}
        
    std::string onRender(bool focus) override {
        return focus ? "[" + text() + "]" : text();
    }
};

// Usage
auto* custom = app->make<CustomWidget>(parent, "Custom");
```

### Language Pack Format
```json
{
    "key": "translation",
    "welcome": "Hello World"
}
```

## API Reference (Key Classes)

| Class | Description |
|-------|-------------|
| `Application` | Main entry point, manages event loop (single root child only) |
| `Widget` | Base class for all UI components |
| `Button` | Clickable button with callback |
| `Label` | Text display with i18n support |
| `List` | Vertical/horizontal container (recommended as root child) |

## Best Practices

1. **Widget Creation**:
   - Always use `app->make<T>()` instead of `new`
   - Prefer stack allocation for short-lived widgets
   - ⚠️ Application must have exactly one root child (use List as root)

2. **Event Handling**:
   - Keep handlers short and fast
   - Use `tryPostEvent` for cross-thread updates

3. **Performance**:
   - Minimize render calls
   - Batch widget updates
