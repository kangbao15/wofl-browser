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

struct WoflNode {
    WoflNodeType type;
    std::string tag;
    std::string text;
    std::vector<WoflAttribute> attributes;
    std::vector<WoflNode> children;
};

class WoflEngine {
public:
    std::string getName() const {
        return "Wofl Engine";
    }

    std::string getVersion() const {
        return "0.1.0";
    }

    WoflNode createElement(const std::string& tag) const {
        return {
            WoflNodeType::Element,
            tag,
            "",
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
                source.substr(nameStart, pos - nameStart);

            while (pos < source.size() &&
                   (source[pos] == ' ' ||
                    source[pos] == '\t' ||
                    source[pos] == '\n')) {
                ++pos;
            }

            if (pos >= source.size() || source[pos] != '=') {
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
                    source.substr(valueStart, pos - valueStart);

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
                    source.substr(valueStart, pos - valueStart);
            }

            addAttribute(node, name, value);
        }
    }

    WoflNode parseDOM(const std::string& html) const {
        WoflNode root{
            WoflNodeType::Document,
            "document",
            "",
            {},
            {}
        };

        std::stack<WoflNode*> nodes;
        nodes.push(&root);

        std::size_t i = 0;

        while (i < html.size()) {
            if (html[i] == '<') {
                std::size_t end = html.find('>', i);

                if (end == std::string::npos) {
                    break;
                }

                std::string source =
                    html.substr(i + 1, end - i - 1);

                if (!source.empty() && source[0] == '/') {
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
                std::size_t end = html.find('<', i);

                if (end == std::string::npos) {
                    end = html.size();
                }

                std::string text =
                    html.substr(i, end - i);

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