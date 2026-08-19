#include "input.h"

#include <array>

class WoflInputManager {
public:
    void handle(const WoflInputEvent& event) {
        switch (event.type) {
        case WoflInputEvent::Type::MouseMove:
            mouseX = event.x;
            mouseY = event.y;
            break;

        case WoflInputEvent::Type::MouseDown:
            if (event.button >= 0 &&
                event.button < static_cast<int>(buttons.size()))
                buttons[event.button] = true;
            break;

        case WoflInputEvent::Type::MouseUp:
            if (event.button >= 0 &&
                event.button < static_cast<int>(buttons.size()))
                buttons[event.button] = false;
            break;

        case WoflInputEvent::Type::Scroll:
            scrollX += event.deltaX;
            scrollY += event.deltaY;
            break;

        case WoflInputEvent::Type::KeyDown:
            if (event.key >= 0 &&
                event.key < static_cast<int>(keys.size()))
                keys[event.key] = true;
            break;

        case WoflInputEvent::Type::KeyUp:
            if (event.key >= 0 &&
                event.key < static_cast<int>(keys.size()))
                keys[event.key] = false;
            break;
        }
    }

private:
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    float scrollX = 0.0f;
    float scrollY = 0.0f;

    std::array<bool, 8> buttons{};
    std::array<bool, 512> keys{};
};
