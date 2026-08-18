#include <string>
#include <cstddef>
#include <vector>
#include <stack>
#include <utility>

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

struct WoflNode {
    WoflNodeType type;
    std::string tag;
    std::string text;
    std::vector<WoflAttribute> attributes;
    std::vector<WoflNode> children;
    std::vector<WoflStyle> styles;
};

class WoflEngine {
public:
    std::string getName() const {
        return "Wofl Engine";
    }

    std::string getVersion() const {
        return "0.2.0";
    }

    WoflNode createElement(const std::string& tag) const {
        return {
            WoflNodeType::Element,
            tag,
            "",
            {},
            {},
            {}
        };
    }

    WoflNode createText(const std::string& text) const {
        return {
            WoflNodeType::Text,
            "",
            text,
            {},
            {},
            {}
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

    std::vector<WoflStyle> parseStyle(
        const std::string& source
    ) const {
        std::vector<WoflStyle> styles;
        std::size_t start = 0;

        while (start < source.size()) {
            std::size_t end = source.find(';', start);

            if (end == std::string::npos) {
                end = source.size();
            }

            std::string declaration =
                source.substr(start, end - start);

            std::size_t colon =
                declaration.find(':');

            if (colon != std::string::npos) {
                std::string property =
                    declaration.substr(0, colon);

                std::string value =
                    declaration.substr(colon + 1);

                while (!property.empty() &&
                       property.front() == ' ') {
                    property.erase(property.begin());
                }

                while (!property.empty() &&
                       property.back() == ' ') {
                    property.pop_back();
                }

                while (!value.empty() &&
                       value.front() == ' ') {
                    value.erase(value.begin());
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

    void parseInlineStyle(WoflNode& node) const {
        for (const auto& attribute : node.attributes) {
            if (attribute.name == "style") {
                node.styles =
                    parseStyle(attribute.value);
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
                css.substr(pos, open - pos);

            while (!selector.empty() &&
                   selector.front() == ' ') {
                selector.erase(selector.begin());
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
                parseStyle(declarations);

            if (!rule.selector.empty()) {
                rules.push_back(std::move(rule));
            }

            pos = close + 1;
        }

        return rules;
    }

    void applyCSS(
        WoflNode& node,
        const std::vector<WoflCSSRule>& rules
    ) const {
        if (node.type == WoflNodeType::Element) {
            for (const auto& rule : rules) {
                if (matchesSelector(node, rule.selector)) {
                    for (const auto& style : rule.styles) {
                        addStyle(
                            node,
                            style.property,
                            style.value
                        );
                    }
                }
            }
        }

        for (auto& child : node.children) {
            applyCSS(child, rules);
        }
    }

    bool matchesSelector(
        const WoflNode& node,
        const std::string& selector
    ) const {
        if (node.type != WoflNodeType::Element) {
            return false;
        }

        if (selector == node.tag) {
            return true;
        }

        for (const auto& attribute : node.attributes) {
            if (selector[0] == '#' &&
                attribute.name == "id" &&
                selector.substr(1) == attribute.value) {
                return true;
            }

            if (selector[0] == '.' &&
                attribute.name == "class") {
                std::string classes =
                    attribute.value;

                std::size_t start = 0;

                while (start < classes.size()) {
                    std::size_t end =
                        classes.find(' ', start);

                    if (end == std::string::npos) {
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
                addAttribute(node, name, "");
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

                char quote = source[pos++];
                std::size_t valueStart = pos;

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
                std::size_t valueStart = pos;

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

            addAttribute(node, name, value);
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
            {}
        };

        std::stack<WoflNode*> nodes;
        nodes.push(&root);

        std::size_t i = 0;

        while (i < html.size()) {
            if (html[i] == '<') {
                std::size_t end =
                    html.find('>', i);

                if (end == std::string::npos) {
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

                    while (split < source.size() &&
                           source[split] != ' ' &&
                           source[split] != '\t' &&
                           source[split] != '\n') {
                        ++split;
                    }

                    std::string tag =
                        source.substr(0, split);

                    WoflNode element =
                        createElement(tag);

                    if (split < source.size()) {
                        parseAttributes(
                            element,
                            source.substr(split)
                        );
                    }

                    parseInlineStyle(element);

                    nodes.top()->children.push_back(
                        std::move(element)
                    );

                    WoflNode* child =
                        &nodes.top()->children.back();

                    if (!isVoidElement(tag)) {
                        nodes.push(child);
                    }
                }

                i = end + 1;

            } else {

                std::size_t end =
                    html.find('<', i);

                if (end == std::string::npos) {
                    end = html.size();
                }

                std::string text =
                    html.substr(
                        i,
                        end - i
                    );

                if (!text.empty()) {
                    nodes.top()->children.push_back(
                        createText(text)
                    );
                }

                i = end;
            }
        }

        return root;
    }
};