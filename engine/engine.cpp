#include <string>
#include <cstddef>
#include <vector>
#include <stack>

struct WoflNode {
    std::string tag;
    std::string text;
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

    std::size_t countHTMLTags(const std::string& html) const {
        std::size_t count = 0;

        for (std::size_t i = 0; i < html.size(); ++i) {
            if (html[i] == '<' &&
                i + 1 < html.size() &&
                html[i + 1] != '/') {
                ++count;
            }
        }

        return count;
    }

    WoflNode createNode(const std::string& tag) const {
        return WoflNode{tag, "", {}};
    }

    WoflNode parseDOM(const std::string& html) const {
        WoflNode root{"document", "", {}};
        std::stack<WoflNode*> nodes;

        nodes.push(&root);

        std::size_t i = 0;

        while (i < html.size()) {
            if (html[i] == '<') {
                std::size_t end = html.find('>', i);

                if (end == std::string::npos) {
                    break;
                }

                std::string tag = html.substr(
                    i + 1,
                    end - i - 1
                );

                if (!tag.empty() && tag[0] == '/') {
                    if (nodes.size() > 1) {
                        nodes.pop();
                    }
                } else {
                    WoflNode node{tag, "", {}};

                    nodes.top()->children.push_back(node);

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

                nodes.top()->text += html.substr(i, end - i);
                i = end;
            }
        }

        return root;
    }
};