#include "blur.h"

void WoflBlurEngine::blur(
    std::vector<WoflBlurPixel>& pixels,
    int width,
    int height,
    int radius
) {
    if (width <= 0 ||
        height <= 0 ||
        radius <= 0 ||
        pixels.empty()) {
        return;
    }

    radius = std::max(1, radius);

    std::vector<WoflBlurPixel> source = pixels;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            long totalR = 0;
            long totalG = 0;
            long totalB = 0;
            long totalA = 0;
            int count = 0;

            int minX = std::max(0, x - radius);
            int maxX = std::min(width - 1, x + radius);
            int minY = std::max(0, y - radius);
            int maxY = std::min(height - 1, y + radius);

            for (int sy = minY; sy <= maxY; ++sy) {
                for (int sx = minX; sx <= maxX; ++sx) {

                    std::size_t index =
                        static_cast<std::size_t>(sy) *
                        static_cast<std::size_t>(width) +
                        static_cast<std::size_t>(sx);

                    const auto& pixel = source[index];

                    totalR += pixel.r;
                    totalG += pixel.g;
                    totalB += pixel.b;
                    totalA += pixel.a;

                    ++count;
                }
            }

            std::size_t index =
                static_cast<std::size_t>(y) *
                static_cast<std::size_t>(width) +
                static_cast<std::size_t>(x);

            pixels[index] = {
                static_cast<unsigned char>(totalR / count),
                static_cast<unsigned char>(totalG / count),
                static_cast<unsigned char>(totalB / count),
                static_cast<unsigned char>(totalA / count)
            };
        }
    }
}