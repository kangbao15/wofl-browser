#include <functional>
#include <string>
#include <cstddef>
#include <vector>
#include <stack>
#include <utility>
#include <algorithm>

enum class WoflNodeType {
    Document,
    Element,
    Text
};

struct WoflAttribute {
    std::string name;
    std::string value;
};

struct WoflStyle {
    std::string property;
    std::string value;
};

struct WoflCSSRule {
    std::string selector;
    std::vector<WoflStyle> styles;
};

struct WoflRect {
    float x;
    float y;
    float width;
    float height;
};

struct WoflBoxModel {
    float marginTop;
    float marginRight;
    float marginBottom;
    float marginLeft;

    float paddingTop;
    float paddingRight;
    float paddingBottom;
    float paddingLeft;

    float borderTop;
    float borderRight;
    float borderBottom;
    float borderLeft;
};

struct WoflNode {
    WoflNodeType type;
    std::string tag;
    std::string text;
    std::vector<WoflAttribute> attributes;
    std::vector<WoflNode> children;
    std::vector<WoflStyle> styles;

    WoflRect rect{
        0.0f, 0.0f, 0.0f, 0.0f
    };

    WoflBoxModel box{
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };
};

class WoflEngine {
public:
    std::string getName() const {
        return "Wofl Engine";
    }

    std::string getVersion() const {
        return "0.3.0";
    }

    WoflNode createElement(const std::string& tag) const {
        return {
            WoflNodeType::Element,
            tag,
            "",
            {},
            {},
            {},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f
            }
        };
    }

    WoflNode createText(const std::string& text) const {
        return {
            WoflNodeType::Text,
            "",
            text,
            {},
            {},
            {},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f
            }
        };
    }

    void addAttribute(
        WoflNode& node,
        const std::string& name,
        const std::string& value
    ) const {
        node.attributes.push_back({name, value});
    }

    bool isVoidElement(const std::string& tag) const {
        return tag == "area" ||
               tag == "base" ||
               tag == "br" ||
               tag == "col" ||
               tag == "embed" ||
               tag == "hr" ||
               tag == "img" ||
               tag == "input" ||
               tag == "link" ||
               tag == "meta" ||
               tag == "param" ||
               tag == "source" ||
               tag == "track" ||
               tag == "wbr";
    }

    void addStyle(
        WoflNode& node,
        const std::string& property,
        const std::string& value
    ) const {
        node.styles.push_back({property, value});
    }

    std::string getStyle(
        const WoflNode& node,
        const std::string& property
    ) const {
        for (auto it = node.styles.rbegin();
             it != node.styles.rend();
             ++it) {
            if (it->property == property) {
                return it->value;
            }
        }

        return "";
    }

    float parsePixels(
        const std::string& value,
        float fallback = 0.0f
    ) const {
        if (value.empty()) {
            return fallback;
        }

        std::string number;

        for (char c : value) {
            if ((c >= '0' && c <= '9') ||
                c == '.' ||
                c == '-') {
                number += c;
            } else {
                break;
            }
        }

        if (number.empty()) {
            return fallback;
        }

        try {
            return std::stof(number);
        } catch (...) {
            return fallback;
        }
    }

    void updateBoxModel(WoflNode& node) const {
        if (node.type != WoflNodeType::Element) {
            return;
        }

        float margin =
            parsePixels(getStyle(node, "margin"));

        float padding =
            parsePixels(getStyle(node, "padding"));

        float border =
            parsePixels(getStyle(node, "border-width"));

        node.box.marginTop =
            parsePixels(
                getStyle(node, "margin-top"),
                margin
            );

        node.box.marginRight =
            parsePixels(
                getStyle(node, "margin-right"),
                margin
            );

        node.box.marginBottom =
            parsePixels(
                getStyle(node, "margin-bottom"),
                margin
            );

        node.box.marginLeft =
            parsePixels(
                getStyle(node, "margin-left"),
                margin
            );

        node.box.paddingTop =
            parsePixels(
                getStyle(node, "padding-top"),
                padding
            );

        node.box.paddingRight =
            parsePixels(
                getStyle(node, "padding-right"),
                padding
            );

        node.box.paddingBottom =
            parsePixels(
                getStyle(node, "padding-bottom"),
                padding
            );

        node.box.paddingLeft =
            parsePixels(
                getStyle(node, "padding-left"),
                padding
            );

        node.box.borderTop =
            parsePixels(
                getStyle(node, "border-top-width"),
                border
            );

        node.box.borderRight =
            parsePixels(
                getStyle(node, "border-right-width"),
                border
            );

        node.box.borderBottom =
            parsePixels(
                getStyle(node, "border-bottom-width"),
                border
            );

        node.box.borderLeft =
            parsePixels(
                getStyle(node, "border-left-width"),
                border
            );
    }

    void calculateLayout(
        WoflNode& node,
        float containingWidth,
        float x = 0.0f,
        float y = 0.0f
    ) const {
        if (node.type == WoflNodeType::Text) {
            float characterWidth = 8.0f;
            float lineHeight = 18.0f;

            node.rect.x = x;
            node.rect.y = y;
            node.rect.width =
                static_cast<float>(
                    node.text.size()
                ) * characterWidth;
            node.rect.height = lineHeight;

            return;
        }

        if (node.type == WoflNodeType::Document) {
            float currentY = y;

            for (auto& child : node.children) {
                calculateLayout(
                    child,
                    containingWidth,
                    x,
                    currentY
                );

                currentY += child.rect.height;
            }

            node.rect = {
                x,
                y,
                containingWidth,
                currentY - y
            };

            return;
        }

        updateBoxModel(node);

        float width =
            parsePixels(
                getStyle(node, "width"),
                containingWidth
            );

        float height =
            parsePixels(
                getStyle(node, "height"),
                0.0f
            );

        if (width <= 0.0f) {
            width = containingWidth;
        }

        float contentX =
            x + node.box.marginLeft +
            node.box.borderLeft +
            node.box.paddingLeft;

        float contentY =
            y + node.box.marginTop +
            node.box.borderTop +
            node.box.paddingTop;

        float currentY = contentY;

        for (auto& child : node.children) {
            calculateLayout(
                child,
                std::max(
                    0.0f,
                    width -
                    node.box.paddingLeft -
                    node.box.paddingRight -
                    node.box.borderLeft -
                    node.box.borderRight
                ),
                contentX,
                currentY
            );

            currentY +=
                child.rect.height;
        }

        if (height <= 0.0f) {
            height =
                currentY -
                y +
                node.box.paddingBottom +
                node.box.borderBottom +
                node.box.marginBottom;
        }

        node.rect = {
            x,
            y,
            width,
            height
        };
    }

    std::vector<WoflStyle> parseStyle(
        const std::string& source
    ) const {
        std::vector<WoflStyle> styles;
        std::size_t start = 0;

        while (start < source.size()) {
            std::size_t end =
                source.find(';', start);

            if (end == std::string::npos) {
                end = source.size();
            }

            std::string declaration =
                source.substr(
                    start,
                    end - start
                );

            std::size_t colon =
                declaration.find(':');

            if (colon != std::string::npos) {
                std::string property =
                    declaration.substr(
                        0,
                        colon
                    );

                std::string value =
                    declaration.substr(
                        colon + 1
                    );

                while (!property.empty() &&
                       property.front() == ' ') {
                    property.erase(
                        property.begin()
                    );
                }

                while (!property.empty() &&
                       property.back() == ' ') {
                    property.pop_back();
                }

                while (!value.empty() &&
                       value.front() == ' ') {
                    value.erase(
                        value.begin()
                    );
                }

                while (!value.empty() &&
                       value.back() == ' ') {
                    value.pop_back();
                }

                if (!property.empty()) {
                    styles.push_back({
                        property,
                        value
                    });
                }
            }

            start = end + 1;
        }

        return styles;
    }

    void parseInlineStyle(
        WoflNode& node
    ) const {
        for (const auto& attribute :
             node.attributes) {
            if (attribute.name == "style") {
                node.styles =
                    parseStyle(
                        attribute.value
                    );
            }
        }
    }

    std::vector<WoflCSSRule> parseCSS(
        const std::string& css
    ) const {
        std::vector<WoflCSSRule> rules;
        std::size_t pos = 0;

        while (pos < css.size()) {
            std::size_t open =
                css.find('{', pos);

            if (open == std::string::npos) {
                break;
            }

            std::size_t close =
                css.find('}', open);

            if (close == std::string::npos) {
                break;
            }

            std::string selector =
                css.substr(
                    pos,
                    open - pos
                );

            while (!selector.empty() &&
                   selector.front() == ' ') {
                selector.erase(
                    selector.begin()
                );
            }

            while (!selector.empty() &&
                   selector.back() == ' ') {
                selector.pop_back();
            }

            WoflCSSRule rule;
            rule.selector = selector;

            std::string declarations =
                css.substr(
                    open + 1,
                    close - open - 1
                );

            rule.styles =
                parseStyle(
                    declarations
                );

            if (!rule.selector.empty()) {
                rules.push_back(
                    std::move(rule)
                );
            }

            pos = close + 1;
        }

        return rules;
    }

    bool matchesSelector(
        const WoflNode& node,
        const std::string& selector
    ) const {
        if (node.type !=
            WoflNodeType::Element) {
            return false;
        }

        if (selector == node.tag) {
            return true;
        }

        for (const auto& attribute :
             node.attributes) {

            if (!selector.empty() &&
                selector[0] == '#' &&
                attribute.name == "id" &&
                selector.substr(1) ==
                    attribute.value) {
                return true;
            }

            if (!selector.empty() &&
                selector[0] == '.' &&
                attribute.name == "class") {

                std::string classes =
                    attribute.value;

                std::size_t start = 0;

                while (start < classes.size()) {
                    std::size_t end =
                        classes.find(
                            ' ',
                            start
                        );

                    if (end ==
                        std::string::npos) {
                        end = classes.size();
                    }

                    if (selector.substr(1) ==
                        classes.substr(
                            start,
                            end - start
                        )) {
                        return true;
                    }

                    start = end + 1;
                }
            }
        }

        return false;
    }

    void applyCSS(
        WoflNode& node,
        const std::vector<WoflCSSRule>& rules
    ) const {
        if (node.type ==
            WoflNodeType::Element) {

            for (const auto& rule : rules) {
                if (matchesSelector(
                        node,
                        rule.selector)) {

                    for (const auto& style :
                         rule.styles) {

                        addStyle(
                            node,
                            style.property,
                            style.value
                        );
                    }
                }
            }

            updateBoxModel(node);
        }

        for (auto& child :
             node.children) {
            applyCSS(
                child,
                rules
            );
        }
    }

    void parseAttributes(
        WoflNode& node,
        const std::string& source
    ) const {
        std::size_t pos = 0;

        while (pos < source.size()) {
            while (pos < source.size() &&
                   (source[pos] == ' ' ||
                    source[pos] == '\t' ||
                    source[pos] == '\n')) {
                ++pos;
            }

            if (pos >= source.size()) {
                break;
            }

            std::size_t nameStart = pos;

            while (pos < source.size() &&
                   source[pos] != '=' &&
                   source[pos] != ' ' &&
                   source[pos] != '\t' &&
                   source[pos] != '\n') {
                ++pos;
            }

            std::string name =
                source.substr(
                    nameStart,
                    pos - nameStart
                );

            while (pos < source.size() &&
                   (source[pos] == ' ' ||
                    source[pos] == '\t' ||
                    source[pos] == '\n')) {
                ++pos;
            }

            if (pos >= source.size() ||
                source[pos] != '=') {

                addAttribute(
                    node,
                    name,
                    ""
                );

                continue;
            }

            ++pos;

            while (pos < source.size() &&
                   (source[pos] == ' ' ||
                    source[pos] == '\t' ||
                    source[pos] == '\n')) {
                ++pos;
            }

            std::string value;

            if (pos < source.size() &&
                (source[pos] == '"' ||
                 source[pos] == '\'')) {

                char quote =
                    source[pos++];

                std::size_t valueStart =
                    pos;

                while (pos < source.size() &&
                       source[pos] != quote) {
                    ++pos;
                }

                value =
                    source.substr(
                        valueStart,
                        pos - valueStart
                    );

                if (pos < source.size()) {
                    ++pos;
                }

            } else {

                std::size_t valueStart =
                    pos;

                while (pos < source.size() &&
                       source[pos] != ' ' &&
                       source[pos] != '\t' &&
                       source[pos] != '\n') {
                    ++pos;
                }

                value =
                    source.substr(
                        valueStart,
                        pos - valueStart
                    );
            }

            addAttribute(
                node,
                name,
                value
            );
        }
    }

    WoflNode parseDOM(
        const std::string& html
    ) const {
        WoflNode root{
            WoflNodeType::Document,
            "document",
            "",
            {},
            {},
            {},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f
            }
        };

        std::stack<WoflNode*> nodes;
        nodes.push(&root);

        std::size_t i = 0;

        while (i < html.size()) {
            if (html[i] == '<') {

                std::size_t end =
                    html.find('>', i);

                if (end ==
                    std::string::npos) {
                    break;
                }

                std::string source =
                    html.substr(
                        i + 1,
                        end - i - 1
                    );

                if (!source.empty() &&
                    source[0] == '/') {

                    if (nodes.size() > 1) {
                        nodes.pop();
                    }

                } else if (!source.empty()) {

                    std::size_t split = 0;

                    while (
                        split < source.size() &&
                        source[split] != ' ' &&
                        source[split] != '\t' &&
                        source[split] != '\n'
                    ) {
                        ++split;
                    }

                    std::string tag =
                        source.substr(
                            0,
                            split
                        );

                    WoflNode element =
                        createElement(tag);

                    if (split <
                        source.size()) {

                        parseAttributes(
                            element,
                            source.substr(
                                split
                            )
                        );
                    }

                    parseInlineStyle(
                        element
                    );

                    nodes.top()
                        ->children.push_back(
                            std::move(element)
                        );

                    WoflNode* child =
                        &nodes.top()
                            ->children.back();

                    if (!isVoidElement(tag)) {
                        nodes.push(child);
                    }
                }

                i = end + 1;

            } else {

                std::size_t end =
                    html.find('<', i);

                if (end ==
                    std::string::npos) {
                    end = html.size();
                }

                std::string text =
                    html.substr(
                        i,
                        end - i
                    );

                if (!text.empty()) {
                    nodes.top()
                        ->children.push_back(
                            createText(text)
                        );
                }

                i = end;
            }
        }

        return root;
    }
};
enum class WoflDisplayType {
    Background,
    Border,
    Text
};

struct WoflColor {
    int r;
    int g;
    int b;
    int a;

    WoflColor(
        int red = 0,
        int green = 0,
        int blue = 0,
        int alpha = 255
    )
        : r(red),
          g(green),
          b(blue),
          a(alpha) {}
};

struct WoflDisplayItem {
    WoflDisplayType type;
    WoflRect rect;
    WoflColor color;
    std::string text;
    float borderWidth;
};

class WoflRenderer {
public:
    std::vector<WoflDisplayItem> paintList;

    void clear() {
        paintList.clear();
    }

    WoflColor parseColor(
        const std::string& value
    ) const {
        if (value == "black") {
            return WoflColor(0, 0, 0);
        }

        if (value == "white") {
            return WoflColor(255, 255, 255);
        }

        if (value == "red") {
            return WoflColor(255, 0, 0);
        }

        if (value == "green") {
            return WoflColor(0, 128, 0);
        }

        if (value == "blue") {
            return WoflColor(0, 0, 255);
        }

        if (value == "transparent") {
            return WoflColor(0, 0, 0, 0);
        }

        if (value.size() == 7 &&
            value[0] == '#') {

            try {
                int r = std::stoi(
                    value.substr(1, 2),
                    nullptr,
                    16
                );

                int g = std::stoi(
                    value.substr(3, 2),
                    nullptr,
                    16
                );

                int b = std::stoi(
                    value.substr(5, 2),
                    nullptr,
                    16
                );

                return WoflColor(
                    r,
                    g,
                    b
                );
            } catch (...) {
                return WoflColor();
            }
        }

        return WoflColor();
    }

    std::string getStyle(
        const WoflNode& node,
        const std::string& property
    ) const {
        for (auto it =
             node.styles.rbegin();
             it != node.styles.rend();
             ++it) {

            if (it->property == property) {
                return it->value;
            }
        }

        return "";
    }

    void paintNode(
        const WoflNode& node
    ) {
        if (node.type ==
            WoflNodeType::Element) {

            paintBackground(node);
            paintBorder(node);
        }

        if (node.type ==
            WoflNodeType::Text) {

            paintText(node);
        }

        for (const auto& child :
             node.children) {

            paintNode(child);
        }
    }

    void paintBackground(
        const WoflNode& node
    ) {
        std::string value =
            getStyle(
                node,
                "background-color"
            );

        if (value.empty()) {
            return;
        }

        WoflDisplayItem item;

        item.type =
            WoflDisplayType::Background;

        item.rect =
            node.rect;

        item.color =
            parseColor(value);

        item.text = "";

        item.borderWidth = 0.0f;

        paintList.push_back(item);
    }

    void paintBorder(
        const WoflNode& node
    ) {
        std::string width =
            getStyle(
                node,
                "border-width"
            );

        if (width.empty()) {
            return;
        }

        std::string color =
            getStyle(
                node,
                "border-color"
            );

        WoflDisplayItem item;

        item.type =
            WoflDisplayType::Border;

        item.rect =
            node.rect;

        item.color =
            parseColor(color);

        item.text = "";

        item.borderWidth =
            parsePixels(width);

        paintList.push_back(item);
    }

    void paintText(
        const WoflNode& node
    ) {
        if (node.text.empty()) {
            return;
        }

        WoflDisplayItem item;

        item.type =
            WoflDisplayType::Text;

        item.rect =
            node.rect;

        std::string color =
            getStyle(
                node,
                "color"
            );

        if (color.empty()) {
            color = "black";
        }

        item.color =
            parseColor(color);

        item.text =
            node.text;

        item.borderWidth = 0.0f;

        paintList.push_back(item);
    }

    float parsePixels(
        const std::string& value
    ) const {
        if (value.empty()) {
            return 0.0f;
        }

        std::string number;

        for (char c : value) {
            if ((c >= '0' &&
                 c <= '9') ||
                c == '.' ||
                c == '-') {

                number += c;

            } else {
                break;
            }
        }

        if (number.empty()) {
            return 0.0f;
        }

        try {
            return std::stof(number);
        } catch (...) {
            return 0.0f;
        }
    }
};
struct WoflPixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class WoflFramebuffer {
public:
    int width;
    int height;
    std::vector<WoflPixel> pixels;

    WoflFramebuffer(
        int w,
        int h
    )
        : width(w),
          height(h),
          pixels(
              static_cast<std::size_t>(w) *
              static_cast<std::size_t>(h)
          ) {
        clear(
            WoflColor(255, 255, 255, 255)
        );
    }

    void clear(
        const WoflColor& color
    ) {
        WoflPixel pixel{
            static_cast<unsigned char>(
                color.r
            ),
            static_cast<unsigned char>(
                color.g
            ),
            static_cast<unsigned char>(
                color.b
            ),
            static_cast<unsigned char>(
                color.a
            )
        };

        std::fill(
            pixels.begin(),
            pixels.end(),
            pixel
        );
    }

    bool inside(
        int x,
        int y
    ) const {
        return x >= 0 &&
               y >= 0 &&
               x < width &&
               y < height;
    }

    void setPixel(
        int x,
        int y,
        const WoflColor& color
    ) {
        if (!inside(x, y)) {
            return;
        }

        std::size_t index =
            static_cast<std::size_t>(y) *
            static_cast<std::size_t>(width) +
            static_cast<std::size_t>(x);

        pixels[index] = {
            static_cast<unsigned char>(
                color.r
            ),
            static_cast<unsigned char>(
                color.g
            ),
            static_cast<unsigned char>(
                color.b
            ),
            static_cast<unsigned char>(
                color.a
            )
        };
    }

    void fillRect(
        const WoflRect& rect,
        const WoflColor& color
    ) {
        int left =
            static_cast<int>(rect.x);

        int top =
            static_cast<int>(rect.y);

        int right =
            static_cast<int>(
                rect.x + rect.width
            );

        int bottom =
            static_cast<int>(
                rect.y + rect.height
            );

        left =
            std::max(0, left);

        top =
            std::max(0, top);

        right =
            std::min(width, right);

        bottom =
            std::min(height, bottom);

        for (int y = top;
             y < bottom;
             ++y) {

            for (int x = left;
                 x < right;
                 ++x) {

                setPixel(
                    x,
                    y,
                    color
                );
            }
        }
    }

    void drawBorder(
        const WoflRect& rect,
        const WoflColor& color,
        float borderWidth
    ) {
        int thickness =
            std::max(
                1,
                static_cast<int>(
                    borderWidth
                )
            );

        WoflRect top{
            rect.x,
            rect.y,
            rect.width,
            static_cast<float>(
                thickness
            )
        };

        WoflRect bottom{
            rect.x,
            rect.y + rect.height -
                thickness,
            rect.width,
            static_cast<float>(
                thickness
            )
        };

        WoflRect left{
            rect.x,
            rect.y,
            static_cast<float>(
                thickness
            ),
            rect.height
        };

        WoflRect right{
            rect.x + rect.width -
                thickness,
            rect.y,
            static_cast<float>(
                thickness
            ),
            rect.height
        };

        fillRect(top, color);
        fillRect(bottom, color);
        fillRect(left, color);
        fillRect(right, color);
    }
};

class WoflRasterizer {
public:
    void rasterize(
        const WoflRenderer& renderer,
        WoflFramebuffer& framebuffer
    ) const {
        for (const auto& item :
             renderer.paintList) {

            if (item.type ==
                WoflDisplayType::Background) {

                framebuffer.fillRect(
                    item.rect,
                    item.color
                );
            }

            else if (
                item.type ==
                WoflDisplayType::Border
            ) {

                framebuffer.drawBorder(
                    item.rect,
                    item.color,
                    item.borderWidth
                );
            }
        }
    }
};
#include <fstream>

class WoflImageWriter {
public:
    static bool writePPM(
        const WoflFramebuffer& framebuffer,
        const std::string& filename
    ) {
        std::ofstream file(
            filename,
            std::ios::binary
        );

        if (!file.is_open()) {
            return false;
        }

        file << "P6\n";
        file << framebuffer.width
             << " "
             << framebuffer.height
             << "\n";
        file << "255\n";

        for (const auto& pixel :
             framebuffer.pixels) {

            file.put(
                static_cast<char>(pixel.r)
            );

            file.put(
                static_cast<char>(pixel.g)
            );

            file.put(
                static_cast<char>(pixel.b)
            );
        }

        return file.good();
    }
};

class WoflRenderPipeline {
public:
    WoflRenderer renderer;
    WoflRasterizer rasterizer;

    bool render(
        const WoflNode& document,
        int width,
        int height,
        const std::string& output
    ) {
        renderer.clear();

        renderer.paintNode(document);

        WoflFramebuffer framebuffer(
            width,
            height
        );

        rasterizer.rasterize(
            renderer,
            framebuffer
        );

        return WoflImageWriter::writePPM(
            framebuffer,
            output
        );
    }
};
struct WoflGlyph {
    int width;
    int height;
    std::vector<unsigned char> bitmap;
};

class WoflTextRasterizer {
public:
    static WoflGlyph makeGlyph(char character) {
        WoflGlyph glyph;

        glyph.width = 5;
        glyph.height = 7;
        glyph.bitmap.resize(35, 0);

        // Basic placeholder glyph patterns.
        // Real font loading will replace this later.
        static const unsigned char pattern[7][5] = {
            {1, 1, 1, 1, 1},
            {1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1},
            {1, 1, 1, 1, 1}
        };

        if (character == ' ') {
            std::fill(
                glyph.bitmap.begin(),
                glyph.bitmap.end(),
                0
            );
            return glyph;
        }

        for (int y = 0; y < 7; ++y) {
            for (int x = 0; x < 5; ++x) {
                glyph.bitmap[
                    static_cast<std::size_t>(y * 5 + x)
                ] = pattern[y][x];
            }
        }

        return glyph;
    }

    static void drawGlyph(
        WoflFramebuffer& framebuffer,
        char character,
        int x,
        int y,
        const WoflColor& color
    ) {
        WoflGlyph glyph =
            makeGlyph(character);

        for (int gy = 0;
             gy < glyph.height;
             ++gy) {

            for (int gx = 0;
                 gx < glyph.width;
                 ++gx) {

                std::size_t index =
                    static_cast<std::size_t>(
                        gy * glyph.width + gx
                    );

                if (glyph.bitmap[index] == 0) {
                    continue;
                }

                framebuffer.setPixel(
                    x + gx,
                    y + gy,
                    color
                );
            }
        }
    }

    static void drawText(
        WoflFramebuffer& framebuffer,
        const std::string& text,
        int x,
        int y,
        const WoflColor& color
    ) {
        int cursorX = x;

        for (char character : text) {
            drawGlyph(
                framebuffer,
                character,
                cursorX,
                y,
                color
            );

            cursorX += 6;
        }
    }
};

class WoflTextPaintStage {
public:
    void paint(
        const WoflRenderer& renderer,
        WoflFramebuffer& framebuffer
    ) const {
        for (const auto& item :
             renderer.paintList) {

            if (item.type !=
                WoflDisplayType::Text) {
                continue;
            }

            int x =
                static_cast<int>(item.rect.x);

            int y =
                static_cast<int>(item.rect.y);

            WoflTextRasterizer::drawText(
                framebuffer,
                item.text,
                x,
                y,
                item.color
            );
        }
    }
};
struct WoflRoundedRect {
    WoflRect rect;
    float radius;
};

class WoflBorderRadiusRasterizer {
public:
    static bool insideRoundedRect(
        float px,
        float py,
        const WoflRoundedRect& rounded
    ) {
        float x = px - rounded.rect.x;
        float y = py - rounded.rect.y;

        float w = rounded.rect.width;
        float h = rounded.rect.height;

        float r = std::min(
            rounded.radius,
            std::min(w, h) / 2.0f
        );

        if (x >= r &&
            x <= w - r) {
            return y >= 0 && y <= h;
        }

        if (y >= r &&
            y <= h - r) {
            return x >= 0 && x <= w;
        }

        float cx =
            x < r ? r : w - r;

        float cy =
            y < r ? r : h - r;

        float dx = x - cx;
        float dy = y - cy;

        return dx * dx + dy * dy <= r * r;
    }

    static void fillRoundedRect(
        WoflFramebuffer& framebuffer,
        const WoflRoundedRect& rounded,
        const WoflColor& color
    ) {
        int left =
            std::max(
                0,
                static_cast<int>(
                    rounded.rect.x
                )
            );

        int top =
            std::max(
                0,
                static_cast<int>(
                    rounded.rect.y
                )
            );

        int right =
            std::min(
                framebuffer.width,
                static_cast<int>(
                    rounded.rect.x +
                    rounded.rect.width
                )
            );

        int bottom =
            std::min(
                framebuffer.height,
                static_cast<int>(
                    rounded.rect.y +
                    rounded.rect.height
                )
            );

        for (int y = top;
             y < bottom;
             ++y) {

            for (int x = left;
                 x < right;
                 ++x) {

                if (insideRoundedRect(
                        static_cast<float>(x) + 0.5f,
                        static_cast<float>(y) + 0.5f,
                        rounded
                    )) {

                    framebuffer.setPixel(
                        x,
                        y,
                        color
                    );
                }
            }
        }
    }
};
struct WoflClipRect {
    int x;
    int y;
    int width;
    int height;

    bool contains(int px, int py) const {
        return px >= x &&
               py >= y &&
               px < x + width &&
               py < y + height;
    }
};

class WoflClipStack {
private:
    std::vector<WoflClipRect> stack;

public:
    void clear() {
        stack.clear();
    }

    void push(const WoflClipRect& clip) {
        if (stack.empty()) {
            stack.push_back(clip);
            return;
        }

        const WoflClipRect& current =
            stack.back();

        int left =
            std::max(current.x, clip.x);

        int top =
            std::max(current.y, clip.y);

        int right =
            std::min(
                current.x + current.width,
                clip.x + clip.width
            );

        int bottom =
            std::min(
                current.y + current.height,
                clip.y + clip.height
            );

        stack.push_back({
            left,
            top,
            std::max(0, right - left),
            std::max(0, bottom - top)
        });
    }

    void pop() {
        if (!stack.empty()) {
            stack.pop_back();
        }
    }

    bool empty() const {
        return stack.empty();
    }

    bool contains(int x, int y) const {
        if (stack.empty()) {
            return true;
        }

        return stack.back().contains(x, y);
    }

    WoflClipRect current() const {
        if (stack.empty()) {
            return {
                0,
                0,
                0,
                0
            };
        }

        return stack.back();
    }
};
struct WoflScrollOffset {
    float x = 0.0f;
    float y = 0.0f;
};

class WoflScrollState {
private:
    WoflScrollOffset offset;
    float contentWidth = 0.0f;
    float contentHeight = 0.0f;
    float viewportWidth = 0.0f;
    float viewportHeight = 0.0f;

public:
    void setContentSize(
        float width,
        float height
    ) {
        contentWidth = std::max(0.0f, width);
        contentHeight = std::max(0.0f, height);
        clamp();
    }

    void setViewportSize(
        float width,
        float height
    ) {
        viewportWidth = std::max(0.0f, width);
        viewportHeight = std::max(0.0f, height);
        clamp();
    }

    void scrollBy(
        float dx,
        float dy
    ) {
        offset.x += dx;
        offset.y += dy;
        clamp();
    }

    void scrollTo(
        float x,
        float y
    ) {
        offset.x = x;
        offset.y = y;
        clamp();
    }

    WoflScrollOffset getOffset() const {
        return offset;
    }

    float maxScrollX() const {
        return std::max(
            0.0f,
            contentWidth - viewportWidth
        );
    }

    float maxScrollY() const {
        return std::max(
            0.0f,
            contentHeight - viewportHeight
        );
    }

private:
    void clamp() {
        offset.x = std::clamp(
            offset.x,
            0.0f,
            maxScrollX()
        );

        offset.y = std::clamp(
            offset.y,
            0.0f,
            maxScrollY()
        );
    }
};
struct WoflLayer {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    float opacity = 1.0f;
    bool visible = true;
};

class WoflCompositor {
private:
    std::vector<WoflLayer> layers;

public:
    void clear() {
        layers.clear();
    }

    void addLayer(const WoflLayer& layer) {
        layers.push_back(layer);
    }

    std::size_t layerCount() const {
        return layers.size();
    }

    const WoflLayer* getLayer(
        std::size_t index
    ) const {
        if (index >= layers.size()) {
            return nullptr;
        }

        return &layers[index];
    }

    void setOpacity(
        std::size_t index,
        float opacity
    ) {
        if (index >= layers.size()) {
            return;
        }

        layers[index].opacity =
            std::clamp(opacity, 0.0f, 1.0f);
    }

    void setVisible(
        std::size_t index,
        bool visible
    ) {
        if (index >= layers.size()) {
            return;
        }

        layers[index].visible = visible;
    }

    void clearLayers() {
        layers.clear();
    }
};
enum class WoflEventType {
    MouseMove,
    MouseDown,
    MouseUp,
    MouseWheel,
    KeyDown,
    KeyUp
};

struct WoflEvent {
    WoflEventType type;

    float x = 0.0f;
    float y = 0.0f;

    float deltaX = 0.0f;
    float deltaY = 0.0f;

    int button = 0;
    int key = 0;
};

class WoflInputState {
private:
    float mouseX = 0.0f;
    float mouseY = 0.0f;

    bool buttons[8] = {};
    bool keys[512] = {};

    float wheelX = 0.0f;
    float wheelY = 0.0f;

public:
    void handleEvent(
        const WoflEvent& event
    ) {
        switch (event.type) {
        case WoflEventType::MouseMove:
            mouseX = event.x;
            mouseY = event.y;
            break;

        case WoflEventType::MouseDown:
            if (event.button >= 0 &&
                event.button < 8) {
                buttons[event.button] = true;
            }
            break;

        case WoflEventType::MouseUp:
            if (event.button >= 0 &&
                event.button < 8) {
                buttons[event.button] = false;
            }
            break;

        case WoflEventType::MouseWheel:
            wheelX += event.deltaX;
            wheelY += event.deltaY;
            break;

        case WoflEventType::KeyDown:
            if (event.key >= 0 &&
                event.key < 512) {
                keys[event.key] = true;
            }
            break;

        case WoflEventType::KeyUp:
            if (event.key >= 0 &&
                event.key < 512) {
                keys[event.key] = false;
            }
            break;
        }
    }

    float getMouseX() const {
        return mouseX;
    }

    float getMouseY() const {
        return mouseY;
    }

    bool isButtonDown(int button) const {
        if (button < 0 || button >= 8) {
            return false;
        }

        return buttons[button];
    }

    bool isKeyDown(int key) const {
        if (key < 0 || key >= 512) {
            return false;
        }

        return keys[key];
    }

    float getWheelX() const {
        return wheelX;
    }

    float getWheelY() const {
        return wheelY;
    }

    void clearWheel() {
        wheelX = 0.0f;
        wheelY = 0.0f;
    }
};
struct WoflFontMetrics {
    float ascent = 0.0f;
    float descent = 0.0f;
    float lineHeight = 0.0f;
    float averageAdvance = 0.0f;
};

class WoflFontMetricsEngine {
public:
    WoflFontMetrics measure(
        float fontSize
    ) const {
        WoflFontMetrics metrics;

        fontSize =
            std::max(1.0f, fontSize);

        metrics.ascent =
            fontSize * 0.8f;

        metrics.descent =
            fontSize * 0.2f;

        metrics.lineHeight =
            fontSize * 1.2f;

        metrics.averageAdvance =
            fontSize * 0.55f;

        return metrics;
    }

    float measureText(
        const std::string& text,
        float fontSize
    ) const {
        WoflFontMetrics metrics =
            measure(fontSize);

        return static_cast<float>(
            text.size()
        ) * metrics.averageAdvance;
    }

    float lineHeight(
        float fontSize
    ) const {
        return measure(fontSize)
            .lineHeight;
    }
};
struct WoflImage {
    int width = 0;
    int height = 0;
    std::vector<WoflPixel> pixels;
};

class WoflImageRasterizer {
public:
    static void drawImage(
        WoflFramebuffer& framebuffer,
        const WoflImage& image,
        int dstX,
        int dstY,
        int dstWidth,
        int dstHeight
    ) {
        if (image.width <= 0 ||
            image.height <= 0 ||
            dstWidth <= 0 ||
            dstHeight <= 0) {
            return;
        }

        for (int y = 0; y < dstHeight; ++y) {
            int srcY =
                (y * image.height) /
                dstHeight;

            for (int x = 0; x < dstWidth; ++x) {
                int srcX =
                    (x * image.width) /
                    dstWidth;

                std::size_t index =
                    static_cast<std::size_t>(srcY) *
                    static_cast<std::size_t>(image.width) +
                    static_cast<std::size_t>(srcX);

                if (index >= image.pixels.size()) {
                    continue;
                }

                const WoflPixel& pixel =
                    image.pixels[index];

                WoflColor color(
                    pixel.r,
                    pixel.g,
                    pixel.b,
                    pixel.a
                );

                framebuffer.setPixel(
                    dstX + x,
                    dstY + y,
                    color
                );
            }
        }
    }
};
struct WoflViewport {
    int width = 0;
    int height = 0;

    float deviceScale = 1.0f;

    void resize(
        int newWidth,
        int newHeight
    ) {
        width = std::max(0, newWidth);
        height = std::max(0, newHeight);
    }

    void setDeviceScale(
        float scale
    ) {
        deviceScale =
            std::max(0.1f, scale);
    }

    bool contains(
        float x,
        float y
    ) const {
        return x >= 0.0f &&
               y >= 0.0f &&
               x < static_cast<float>(width) &&
               y < static_cast<float>(height);
    }

    int pixelWidth() const {
        return static_cast<int>(
            width * deviceScale
        );
    }

    int pixelHeight() const {
        return static_cast<int>(
            height * deviceScale
        );
    }
};

class WoflViewportController {
private:
    WoflViewport viewport;

public:
    void setSize(
        int width,
        int height
    ) {
        viewport.resize(
            width,
            height
        );
    }

    void setDeviceScale(
        float scale
    ) {
        viewport.setDeviceScale(
            scale
        );
    }

    const WoflViewport& getViewport() const {
        return viewport;
    }

    bool hitViewport(
        float x,
        float y
    ) const {
        return viewport.contains(
            x,
            y
        );
    }
};
// ============================================================
// WOFL ENGINE — CHECKPOINT 4
// Real HTML + CSS Render Pipeline
// ============================================================

class WoflDocumentRenderer {
public:
    WoflEngine engine;
    WoflRenderPipeline pipeline;
    WoflTextPaintStage textStage;

    bool renderHTML(
        const std::string& html,
        const std::string& css,
        int width,
        int height,
        const std::string& output
    ) {
        if (width <= 0 || height <= 0) {
            return false;
        }

        // ----------------------------------------------------
        // 1. HTML -> DOM
        // ----------------------------------------------------

        WoflNode document =
            engine.parseDOM(html);

        // ----------------------------------------------------
        // 2. CSS -> CSS Rules
        // ----------------------------------------------------

        std::vector<WoflCSSRule> rules =
            engine.parseCSS(css);

        // ----------------------------------------------------
        // 3. Inline styles + CSS
        // ----------------------------------------------------

        applyStyles(
            document,
            rules
        );

        // ----------------------------------------------------
        // 4. Layout
        // ----------------------------------------------------

        engine.calculateLayout(
            document,
            static_cast<float>(width),
            0.0f,
            0.0f
        );

        // ----------------------------------------------------
        // 5. Paint
        // ----------------------------------------------------

        pipeline.renderer.clear();

        pipeline.renderer.paintNode(
            document
        );

        // ----------------------------------------------------
        // 6. Framebuffer
        // ----------------------------------------------------

        WoflFramebuffer framebuffer(
            width,
            height
        );

        // ----------------------------------------------------
        // 7. Rasterize backgrounds + borders
        // ----------------------------------------------------

        pipeline.rasterizer.rasterize(
            pipeline.renderer,
            framebuffer
        );

        // ----------------------------------------------------
        // 8. Rasterize text
        // ----------------------------------------------------

        textStage.paint(
            pipeline.renderer,
            framebuffer
        );

        // ----------------------------------------------------
        // 9. Write image
        // ----------------------------------------------------

        return WoflImageWriter::writePPM(
            framebuffer,
            output
        );
    }

private:

    void applyStyles(
        WoflNode& node,
        const std::vector<WoflCSSRule>& rules
    ) {
        // Apply CSS rules to this node.
        engine.applyCSS(
            node,
            rules
        );

        // Make sure inline styles exist.
        if (node.type ==
            WoflNodeType::Element) {

            engine.parseInlineStyle(
                node
            );

            engine.updateBoxModel(
                node
            );
        }
    }
};


// ============================================================
// WOFL STYLE CASCADE
// ============================================================

class WoflStyleEngine {
public:

    static void apply(
        WoflNode& document,
        WoflEngine& engine,
        const std::vector<WoflCSSRule>& rules
    ) {
        applyNode(
            document,
            engine,
            rules
        );
    }

private:

    static void applyNode(
        WoflNode& node,
        WoflEngine& engine,
        const std::vector<WoflCSSRule>& rules
    ) {
        if (node.type ==
            WoflNodeType::Element) {

            // Parse inline style first.
            engine.parseInlineStyle(
                node
            );

            // Apply stylesheet rules.
            for (const auto& rule :
                 rules) {

                if (!engine.matchesSelector(
                        node,
                        rule.selector
                    )) {

                    continue;
                }

                for (const auto& style :
                     rule.styles) {

                    engine.addStyle(
                        node,
                        style.property,
                        style.value
                    );
                }
            }

            engine.updateBoxModel(
                node
            );
        }

        for (auto& child :
             node.children) {

            applyNode(
                child,
                engine,
                rules
            );
        }
    }
};


// ============================================================
// WOFL DISPLAY LIST BUILDER
// ============================================================

class WoflDisplayListBuilder {
public:

    static void build(
        const WoflNode& node,
        WoflRenderer& renderer
    ) {
        renderer.paintNode(
            node
        );
    }
};


// ============================================================
// WOFL COMPLETE PIPELINE
// ============================================================

class WoflEngineRuntime {
public:

    WoflEngine engine;
    WoflRenderer renderer;
    WoflRasterizer rasterizer;
    WoflTextPaintStage textPainter;

    WoflEngineRuntime() = default;

    bool render(
        const std::string& html,
        const std::string& css,
        int width,
        int height,
        const std::string& output
    ) {
        if (width <= 0 ||
            height <= 0) {

            return false;
        }

        // ----------------------------------------------------
        // HTML
        // ----------------------------------------------------

        WoflNode document =
            engine.parseDOM(
                html
            );

        // ----------------------------------------------------
        // CSS
        // ----------------------------------------------------

        std::vector<WoflCSSRule> rules =
            engine.parseCSS(
                css
            );

        // ----------------------------------------------------
        // STYLE
        // ----------------------------------------------------

        WoflStyleEngine::apply(
            document,
            engine,
            rules
        );

        // ----------------------------------------------------
        // LAYOUT
        // ----------------------------------------------------

        engine.calculateLayout(
            document,
            static_cast<float>(width),
            0.0f,
            0.0f
        );

        // ----------------------------------------------------
        // DISPLAY LIST
        // ----------------------------------------------------

        renderer.clear();

        WoflDisplayListBuilder::build(
            document,
            renderer
        );

        // ----------------------------------------------------
        // FRAMEBUFFER
        // ----------------------------------------------------

        WoflFramebuffer framebuffer(
            width,
            height
        );

        // ----------------------------------------------------
        // RASTER
        // ----------------------------------------------------

        rasterizer.rasterize(
            renderer,
            framebuffer
        );

        // ----------------------------------------------------
        // TEXT
        // ----------------------------------------------------

        textPainter.paint(
            renderer,
            framebuffer
        );

        // ----------------------------------------------------
        // OUTPUT
        // ----------------------------------------------------

        return WoflImageWriter::writePPM(
            framebuffer,
            output
        );
    }

    std::string engineName() const {
        return engine.getName();
    }

    std::string engineVersion() const {
        return "0.4.0";
    }
};


// ============================================================
// WOFL CHECKPOINT 4 TEST
// ============================================================

static bool WoflCheckpoint4Test(
    const std::string& output
) {
    WoflEngineRuntime runtime;

    const std::string html =
        "<html>"
        "<body>"
        "<div id=\"box\">"
        "Wofl Browser"
        "</div>"
        "</body>"
        "</html>";

    const std::string css =
        "body {"
        "background-color: white;"
        "padding: 20px;"
        "}"
        "#box {"
        "background-color: #eeeeff;"
        "border-width: 2px;"
        "border-color: #663399;"
        "padding: 20px;"
        "margin: 10px;"
        "color: black;"
        "}";

    return runtime.render(
        html,
        css,
        800,
        600,
        output
    );
}
// ============================================================
// Wofl Browser - Checkpoint 5
// Page Engine: HTML + CSS -> DOM -> Layout -> Paint
// ============================================================

struct WoflPage {
    WoflNode document;
    std::vector<WoflCSSRule> cssRules;
};

class WoflPageEngine {
public:
    WoflEngine engine;

    WoflPage createPage(
        const std::string& html,
        const std::string& css = ""
    ) const {
        WoflPage page;

        page.document =
            engine.parseDOM(html);

        page.cssRules =
            engine.parseCSS(css);

        engine.applyCSS(
            page.document,
            page.cssRules
        );

        return page;
    }

    void layout(
        WoflPage& page,
        float width,
        float height
    ) const {
        engine.calculateLayout(
            page.document,
            width,
            0.0f,
            0.0f
        );

        page.document.rect.width =
            width;

        if (page.document.rect.height >
            height) {
            page.document.rect.height =
                page.document.rect.height;
        }
    }

    const WoflNode& getDocument(
        const WoflPage& page
    ) const {
        return page.document;
    }
};

class WoflBrowserView {
private:
    WoflPageEngine pageEngine;
    WoflRenderPipeline pipeline;
    WoflTextPaintStage textStage;

public:
    bool renderPage(
        const std::string& html,
        const std::string& css,
        int width,
        int height,
        const std::string& output
    ) {
        WoflPage page =
            pageEngine.createPage(
                html,
                css
            );

        pageEngine.layout(
            page,
            static_cast<float>(width),
            static_cast<float>(height)
        );

        pipeline.renderer.clear();

        pipeline.renderer.paintNode(
            page.document
        );

        WoflFramebuffer framebuffer(
            width,
            height
        );

        pipeline.rasterizer.rasterize(
            pipeline.renderer,
            framebuffer
        );

        textStage.paint(
            pipeline.renderer,
            framebuffer
        );

        return WoflImageWriter::writePPM(
            framebuffer,
            output
        );
    }
};

class WoflBrowser {
private:
    WoflPageEngine pageEngine;
    WoflBrowserView view;

    std::string currentURL;
    std::string currentHTML;
    std::string currentCSS;

public:
    void load(
        const std::string& url,
        const std::string& html,
        const std::string& css = ""
    ) {
        currentURL = url;
        currentHTML = html;
        currentCSS = css;
    }

    bool render(
        int width,
        int height,
        const std::string& output
    ) {
        if (currentHTML.empty()) {
            return false;
        }

        return view.renderPage(
            currentHTML,
            currentCSS,
            width,
            height,
            output
        );
    }

    const std::string& getURL() const {
        return currentURL;
    }

    const std::string& getHTML() const {
        return currentHTML;
    }

    const std::string& getCSS() const {
        return currentCSS;
    }
};
// ============================================================
// WOFL ENGINE — CHECKPOINT 6
// Browser Window + Navigation State
// ============================================================

enum class WoflNavigationState {
    Empty,
    Loading,
    Loaded,
    Failed
};

struct WoflPageState {
    std::string url;
    std::string html;
    std::string css;
    WoflNavigationState state =
        WoflNavigationState::Empty;
};

class WoflNavigationController {
private:
    WoflPageState current;

public:
    void beginNavigation(
        const std::string& url
    ) {
        current.url = url;
        current.html.clear();
        current.css.clear();
        current.state =
            WoflNavigationState::Loading;
    }

    void setContent(
        const std::string& html,
        const std::string& css
    ) {
        current.html = html;
        current.css = css;
        current.state =
            WoflNavigationState::Loaded;
    }

    void fail() {
        current.state =
            WoflNavigationState::Failed;
    }

    const WoflPageState& getPage() const {
        return current;
    }

    bool isLoading() const {
        return current.state ==
            WoflNavigationState::Loading;
    }

    bool isLoaded() const {
        return current.state ==
            WoflNavigationState::Loaded;
    }
};

struct WoflWindow {
    int width = 1280;
    int height = 720;

    std::string title =
        "Wofl Browser";

    bool visible = true;

    void resize(
        int newWidth,
        int newHeight
    ) {
        width =
            std::max(1, newWidth);

        height =
            std::max(1, newHeight);
    }
};

class WoflBrowserWindow {
private:
    WoflWindow window;
    WoflNavigationController navigation;
    WoflBrowser browser;

public:
    void resize(
        int width,
        int height
    ) {
        window.resize(
            width,
            height
        );
    }

    void navigate(
        const std::string& url,
        const std::string& html,
        const std::string& css = ""
    ) {
        navigation.beginNavigation(
            url
        );

        navigation.setContent(
            html,
            css
        );

        browser.load(
            url,
            html,
            css
        );
    }

    bool render(
        const std::string& output
    ) {
        if (!navigation.isLoaded()) {
            return false;
        }

        return browser.render(
            window.width,
            window.height,
            output
        );
    }

    const WoflWindow& getWindow() const {
        return window;
    }

    const WoflPageState& getPage() const {
        return navigation.getPage();
    }
};

// ============================================================
// CHECKPOINT 6 TEST
// ============================================================

static bool WoflCheckpoint6Test(
    const std::string& output
) {
    WoflBrowserWindow browser;

    browser.resize(
        1280,
        720
    );

    browser.navigate(
        "wofl://home",
        "<html>"
        "<body>"
        "<div id=\"content\">"
        "Welcome to Wofl Browser"
        "</div>"
        "</body>"
        "</html>",
        "body {"
        "background-color: white;"
        "padding: 24px;"
        "}"
        "#content {"
        "background-color: #eeeeff;"
        "border-width: 2px;"
        "border-color: #663399;"
        "padding: 24px;"
        "color: black;"
        "}"
    );

    return browser.render(
        output
    );
}
// ============================================================
// WOFL ENGINE — CHECKPOINT 7
// DOM Query + Browser Interaction
// ============================================================

class WoflDOMQuery {
public:
    static WoflNode* findById(
        WoflNode& node,
        const std::string& id
    ) {
        if (node.type == WoflNodeType::Element) {
            for (const auto& attribute : node.attributes) {
                if (attribute.name == "id" &&
                    attribute.value == id) {
                    return &node;
                }
            }
        }

        for (auto& child : node.children) {
            WoflNode* result =
                findById(child, id);

            if (result != nullptr) {
                return result;
            }
        }

        return nullptr;
    }

    static WoflNode* findByTag(
        WoflNode& node,
        const std::string& tag
    ) {
        if (node.type == WoflNodeType::Element &&
            node.tag == tag) {
            return &node;
        }

        for (auto& child : node.children) {
            WoflNode* result =
                findByTag(child, tag);

            if (result != nullptr) {
                return result;
            }
        }

        return nullptr;
    }
};

class WoflBrowserInteraction {
private:
    WoflInputState input;

public:
    void handleEvent(
        const WoflEvent& event
    ) {
        input.handleEvent(event);
    }

    bool isMouseDown(
        int button
    ) const {
        return input.isButtonDown(button);
    }

    bool isKeyDown(
        int key
    ) const {
        return input.isKeyDown(key);
    }

    float mouseX() const {
        return input.getMouseX();
    }

    float mouseY() const {
        return input.getMouseY();
    }

    float wheelY() const {
        return input.getWheelY();
    }

    void clearWheel() {
        input.clearWheel();
    }
};

class WoflInteractivePage {
private:
    WoflPage page;
    WoflBrowserInteraction interaction;

public:
    void load(
        WoflPageEngine& engine,
        const std::string& html,
        const std::string& css = ""
    ) {
        page =
            engine.createPage(
                html,
                css
            );
    }

    WoflNode* getElementById(
        const std::string& id
    ) {
        return WoflDOMQuery::findById(
            page.document,
            id
        );
    }

    WoflNode* getElementByTag(
        const std::string& tag
    ) {
        return WoflDOMQuery::findByTag(
            page.document,
            tag
        );
    }

    WoflBrowserInteraction& getInteraction() {
        return interaction;
    }

    WoflPage& getPage() {
        return page;
    }
};

// ============================================================
// CHECKPOINT 7 TEST
// ============================================================

static bool WoflCheckpoint7Test() {
    WoflPageEngine engine;
    WoflInteractivePage page;

    page.load(
        engine,
        "<html>"
        "<body>"
        "<div id=\"main\">"
        "<p>Hello Wofl</p>"
        "</div>"
        "</body>"
        "</html>"
    );

    WoflNode* main =
        page.getElementById("main");

    if (main == nullptr) {
        return false;
    }

    WoflNode* paragraph =
        page.getElementByTag("p");

    if (paragraph == nullptr) {
        return false;
    }

    WoflEvent event{
        WoflEventType::MouseMove
    };

    event.x = 100.0f;
    event.y = 200.0f;

    page.getInteraction()
        .handleEvent(event);

    return
        page.getInteraction().mouseX()
        == 100.0f &&
        page.getInteraction().mouseY()
        == 200.0f;
}
// ============================================================
// WOFL ENGINE — CHECKPOINT 8
// Event Dispatch + Click Handling
// ============================================================

enum class WoflInteractionType {
    None,
    Click,
    Hover,
    Scroll
};

struct WoflInteraction {
    WoflInteractionType type =
        WoflInteractionType::None;

    std::string targetId;

    float x = 0.0f;
    float y = 0.0f;

    float deltaX = 0.0f;
    float deltaY = 0.0f;
};

class WoflEventDispatcher {
private:
    std::vector<WoflInteraction> interactions;

public:
    void clear() {
        interactions.clear();
    }

    void dispatch(
        const WoflEvent& event,
        WoflNode& document
    ) {
        if (event.type ==
            WoflEventType::MouseDown) {

            WoflNode* target =
                findNodeAt(
                    document,
                    event.x,
                    event.y
                );

            if (target != nullptr) {
                WoflInteraction interaction;

                interaction.type =
                    WoflInteractionType::Click;

                interaction.x =
                    event.x;

                interaction.y =
                    event.y;

                interaction.targetId =
                    getId(*target);

                interactions.push_back(
                    interaction
                );
            }
        }

        else if (
            event.type ==
            WoflEventType::MouseWheel
        ) {
            WoflInteraction interaction;

            interaction.type =
                WoflInteractionType::Scroll;

            interaction.deltaX =
                event.deltaX;

            interaction.deltaY =
                event.deltaY;

            interactions.push_back(
                interaction
            );
        }
    }

    const std::vector<WoflInteraction>&
    getInteractions() const {
        return interactions;
    }

private:
    static std::string getId(
        const WoflNode& node
    ) {
        for (const auto& attribute :
             node.attributes) {

            if (attribute.name == "id") {
                return attribute.value;
            }
        }

        return "";
    }

    static WoflNode* findNodeAt(
        WoflNode& node,
        float x,
        float y
    ) {
        for (auto it =
             node.children.rbegin();
             it != node.children.rend();
             ++it) {

            WoflNode* result =
                findNodeAt(
                    *it,
                    x,
                    y
                );

            if (result != nullptr) {
                return result;
            }
        }

        if (node.type !=
            WoflNodeType::Element) {
            return nullptr;
        }

        if (x >= node.rect.x &&
            y >= node.rect.y &&
            x < node.rect.x +
                node.rect.width &&
            y < node.rect.y +
                node.rect.height) {

            return &node;
        }

        return nullptr;
    }
};

class WoflInteractionEngine {
private:
    WoflEventDispatcher dispatcher;

public:
    void handleEvent(
        const WoflEvent& event,
        WoflNode& document
    ) {
        dispatcher.dispatch(
            event,
            document
        );
    }

    void clear() {
        dispatcher.clear();
    }

    const std::vector<WoflInteraction>&
    getInteractions() const {
        return dispatcher
            .getInteractions();
    }

    bool hasClick() const {
        for (const auto& interaction :
             dispatcher.getInteractions()) {

            if (interaction.type ==
                WoflInteractionType::Click) {
                return true;
            }
        }

        return false;
    }
};

// ============================================================
// CHECKPOINT 8 TEST
// ============================================================

static bool WoflCheckpoint8Test() {
    WoflEngine engine;

    WoflNode document =
        engine.parseDOM(
            "<html>"
            "<body>"
            "<button id=\"start\">"
            "Start"
            "</button>"
            "</body>"
            "</html>"
        );

    engine.calculateLayout(
        document,
        800.0f
    );

    WoflInteractionEngine interaction;

    WoflEvent click{
        WoflEventType::MouseDown
    };

    click.x = 10.0f;
    click.y = 10.0f;

    interaction.handleEvent(
        click,
        document
    );

    return true;
}
// ============================================================
// WOFL ENGINE — CHECKPOINT 9
// Scroll + Viewport Integration
// ============================================================

class WoflViewportScrollController {
private:
    WoflViewportController viewport;
    WoflScrollState scroll;

public:
    void setViewport(
        int width,
        int height
    ) {
        viewport.setSize(
            width,
            height
        );

        scroll.setViewportSize(
            static_cast<float>(width),
            static_cast<float>(height)
        );
    }

    void setContentSize(
        float width,
        float height
    ) {
        scroll.setContentSize(
            width,
            height
        );
    }

    void scrollBy(
        float dx,
        float dy
    ) {
        scroll.scrollBy(
            dx,
            dy
        );
    }

    void scrollTo(
        float x,
        float y
    ) {
        scroll.scrollTo(
            x,
            y
        );
    }

    WoflScrollOffset getOffset() const {
        return scroll.getOffset();
    }

    float maxScrollX() const {
        return scroll.maxScrollX();
    }

    float maxScrollY() const {
        return scroll.maxScrollY();
    }

    bool contains(
        float x,
        float y
    ) const {
        return viewport.hitViewport(
            x,
            y
        );
    }
};

class WoflScrollInteractionEngine {
private:
    WoflViewportScrollController controller;

public:
    void setViewport(
        int width,
        int height
    ) {
        controller.setViewport(
            width,
            height
        );
    }

    void setContentSize(
        float width,
        float height
    ) {
        controller.setContentSize(
            width,
            height
        );
    }

    void handleWheel(
        float deltaX,
        float deltaY
    ) {
        controller.scrollBy(
            deltaX,
            deltaY
        );
    }

    WoflScrollOffset getOffset() const {
        return controller.getOffset();
    }

    float maxScrollY() const {
        return controller.maxScrollY();
    }
};

// ============================================================
// CHECKPOINT 9 TEST
// ============================================================

static bool WoflCheckpoint9Test() {
    WoflScrollInteractionEngine engine;

    engine.setViewport(
        1280,
        720
    );

    engine.setContentSize(
        1280.0f,
        2000.0f
    );

    engine.handleWheel(
        0.0f,
        500.0f
    );

    WoflScrollOffset offset =
        engine.getOffset();

    if (offset.y <= 0.0f) {
        return false;
    }

    if (offset.y >
        engine.maxScrollY()) {
        return false;
    }

    return true;
}
// ============================================================
// WOFL ENGINE — CHECKPOINT 10
// Layer Compositor + Opacity
// ============================================================

class WoflLayerCompositorEngine {
private:
    WoflCompositor compositor;

public:
    void clear() {
        compositor.clear();
    }

    std::size_t addLayer(
        int x,
        int y,
        int width,
        int height,
        float opacity = 1.0f,
        bool visible = true
    ) {
        WoflLayer layer;

        layer.x = x;
        layer.y = y;
        layer.width = width;
        layer.height = height;
        layer.opacity =
            std::clamp(opacity, 0.0f, 1.0f);
        layer.visible = visible;

        compositor.addLayer(layer);

        return compositor.layerCount() - 1;
    }

    void setOpacity(
        std::size_t index,
        float opacity
    ) {
        compositor.setOpacity(
            index,
            opacity
        );
    }

    void setVisible(
        std::size_t index,
        bool visible
    ) {
        compositor.setVisible(
            index,
            visible
        );
    }

    std::size_t layerCount() const {
        return compositor.layerCount();
    }

    const WoflLayer* getLayer(
        std::size_t index
    ) const {
        return compositor.getLayer(index);
    }

    void clearLayers() {
        compositor.clearLayers();
    }
};

// ============================================================
// CHECKPOINT 10 TEST
// ============================================================

static bool WoflCheckpoint10Test() {
    WoflLayerCompositorEngine compositor;

    compositor.clear();

    std::size_t background =
        compositor.addLayer(
            0,
            0,
            1280,
            720,
            1.0f,
            true
        );

    std::size_t overlay =
        compositor.addLayer(
            100,
            100,
            500,
            300,
            0.75f,
            true
        );

    if (compositor.layerCount() != 2) {
        return false;
    }

    const WoflLayer* layer =
        compositor.getLayer(overlay);

    if (layer == nullptr) {
        return false;
    }

    if (layer->opacity != 0.75f) {
        return false;
    }

    compositor.setOpacity(
        background,
        0.5f
    );

    layer =
        compositor.getLayer(background);

    if (layer == nullptr ||
        layer->opacity != 0.5f) {
        return false;
    }

    compositor.setVisible(
        overlay,
        false
    );

    layer =
        compositor.getLayer(overlay);

    if (layer == nullptr ||
        layer->visible) {
        return false;
    }

    return true;
}
// ============================================================
// WOFL ENGINE — CHECKPOINT 11
// Image Rendering + Text Paint Integration
// ============================================================

class WoflPaintEngine {
private:
    WoflTextPaintStage textStage;

public:
    void paintText(
        const WoflRenderer& renderer,
        WoflFramebuffer& framebuffer
    ) const {
        textStage.paint(
            renderer,
            framebuffer
        );
    }

    void paintImage(
        WoflFramebuffer& framebuffer,
        const WoflImage& image,
        int x,
        int y,
        int width,
        int height
    ) const {
        WoflImageRasterizer::drawImage(
            framebuffer,
            image,
            x,
            y,
            width,
            height
        );
    }
};

// ============================================================
// CHECKPOINT 11 TEST
// ============================================================

static bool WoflCheckpoint11Test() {
    WoflFramebuffer framebuffer(
        320,
        200
    );

    WoflRenderer renderer;

    WoflNode text =
        WoflEngine().createText(
            "Wofl Browser"
        );

    text.rect = {
        10.0f,
        10.0f,
        100.0f,
        20.0f
    };

    renderer.paintNode(text);

    WoflPaintEngine paint;

    paint.paintText(
        renderer,
        framebuffer
    );

    WoflImage image;

    image.width = 2;
    image.height = 2;

    image.pixels.resize(
        4,
        WoflPixel{
            255,
            255,
            255,
            255
        }
    );

    paint.paintImage(
        framebuffer,
        image,
        20,
        20,
        40,
        40
    );

    return
        framebuffer.width == 320 &&
        framebuffer.height == 200 &&
        framebuffer.pixels.size() ==
            static_cast<std::size_t>(
                320 * 200
            );
}
// ============================================================
// WOFL ENGINE — CHECKPOINT 12
// FINAL ENGINE PIPELINE
// ============================================================

class WoflEnginePipeline {
private:
    WoflEngine engine;
    WoflRenderer renderer;
    WoflRasterizer rasterizer;
    WoflTextPaintStage textPainter;
    WoflInteractionEngine interaction;
    WoflViewportScrollController viewport;
    WoflLayerCompositorEngine compositor;

public:
    WoflNode document;

    bool loadHTML(
        const std::string& html
    ) {
        document =
            engine.parseDOM(html);

        return true;
    }

    void applyStyles(
        const std::string& css
    ) {
        std::vector<WoflCSSRule> rules =
            engine.parseCSS(css);

        engine.applyCSS(
            document,
            rules
        );
    }

    void layout(
        float width
    ) {
        engine.calculateLayout(
            document,
            width
        );
    }

    void setupViewport(
        int width,
        int height
    ) {
        viewport.setViewport(
            width,
            height
        );

        viewport.setContentSize(
            document.rect.width,
            document.rect.height
        );
    }

    void handleEvent(
        const WoflEvent& event
    ) {
        interaction.handleEvent(
            event,
            document
        );

        if (event.type ==
            WoflEventType::MouseWheel) {

            viewport.scrollBy(
                event.deltaX,
                event.deltaY
            );
        }
    }

    void render(
        WoflFramebuffer& framebuffer
    ) {
        renderer.clear();

        renderer.paintNode(
            document
        );

        rasterizer.rasterize(
            renderer,
            framebuffer
        );

        textPainter.paint(
            renderer,
            framebuffer
        );
    }

    WoflScrollOffset getScrollOffset()
        const {
        return viewport.getOffset();
    }

    std::size_t layerCount() const {
        return compositor.layerCount();
    }

    WoflEngine& getEngine() {
        return engine;
    }

    WoflRenderer& getRenderer() {
        return renderer;
    }
};

// ============================================================
// CHECKPOINT 12 FINAL TEST
// ============================================================

static bool WoflCheckpoint12Test() {
    WoflEnginePipeline pipeline;

    if (!pipeline.loadHTML(
        "<html>"
        "<body>"
        "<div id=\"main\">"
        "Wofl Browser"
        "</div>"
        "</body>"
        "</html>"
    )) {
        return false;
    }

    pipeline.applyStyles(
        "body { background-color: white; }"
        "#main { width: 400px; height: 200px; "
        "background-color: #eeeeee; "
        "border-width: 2px; "
        "border-color: black; }"
    );

    pipeline.layout(
        1280.0f
    );

    pipeline.setupViewport(
        1280,
        720
    );

    WoflFramebuffer framebuffer(
        1280,
        720
    );

    pipeline.render(
        framebuffer
    );

    WoflEvent wheel{
        WoflEventType::MouseWheel
    };

    wheel.deltaY = 100.0f;

    pipeline.handleEvent(
        wheel
    );

    return
        pipeline.document.type ==
            WoflNodeType::Document &&
        framebuffer.width == 1280 &&
        framebuffer.height == 720 &&
        framebuffer.pixels.size() ==
            static_cast<std::size_t>(
                1280 * 720
            );
}
class WoflEventDispatcher {
public:
    using Handler = std::function<void(const WoflEvent&)>;

private:
    std::vector<Handler> handlers;

public:
    void addHandler(Handler handler) {
        handlers.push_back(std::move(handler));
    }

    void dispatch(const WoflEvent& event) const {
        for (const auto& handler : handlers) {
            if (handler) {
                handler(event);
            }
        }
    }

    void clear() {
        handlers.clear();
    }

    std::size_t handlerCount() const {
        return handlers.size();
    }
};