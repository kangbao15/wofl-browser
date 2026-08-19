#pragma once

#include <vector>
#include <algorithm>

struct WoflBlurPixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class WoflBlurEngine {
public:
    static void blur(
        std::vector<WoflBlurPixel>& pixels,
        int width,
        int height,
        int radius
    );
};
class WoflGlassMaterial {
public:
    static void apply(
        std::vector<WoflBlurPixel>& pixels,
        int width,
        int height,
        int blurRadius,
        float opacity
    );
};