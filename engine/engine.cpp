#include <string>
#include <cstddef>
#include <vector>

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
};