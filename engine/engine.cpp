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
        return WoflNode{
            WoflNodeType::Element,
            tag,
            "",
            {},
            {}
        };
    }

    WoflNode createText(const std::string& text) const {
        return WoflNode{
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

                std::string tag =
                    html.substr(i + 1, end - i - 1);

                if (!tag.empty() && tag[0] == '/') {
                    if (nodes.size() > 1) {
                        nodes.pop();
                    }
                } else if (!tag.empty()) {
                    WoflNode element = createElement(tag);

                    nodes.top()->children.push_back(
                        std::move(element)
                    );

                    WoflNode* child =
                        &nodes.top()->children.back();

                    nodes.push(child);
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