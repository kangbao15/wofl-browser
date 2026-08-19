#pragma once

#include <cstdint>

struct WoflInputEvent {
    enum class Type {
        MouseMove,
        MouseDown,
        MouseUp,
        Scroll,
        KeyDown,
        KeyUp
    };

    Type type;
    float x = 0.0f;
    float y = 0.0f;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    int key = 0;
    int button = 0;
};
