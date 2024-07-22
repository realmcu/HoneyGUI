#pragma once
#include <string>
#include <vector>
#include <gumbo.h>
#include <functional>
#include <iostream>
#include <atomic>
#include <map>
using ControlHandle = void*;
namespace WebParser
{

class HtmlParser
{
public:
    HtmlParser(const std::string &html);
    ~HtmlParser();

    std::vector<GumboNode *> findElementsByTag(const std::string &tag);
    std::vector<GumboNode *> findChildElementsByTag(GumboNode *node, const std::string &tag);
    void traverseBody(std::function<void(GumboNode *, int, void *)> callback, void *parent = nullptr);
    std::map<std::string, std::string> parseAttributes(const GumboElement *element);
    void traverseBodyWithParent(ControlHandle parent);
private:
    GumboOutput *output_;
    void searchForElements(GumboNode *node, GumboTag tag, std::vector<GumboNode *> &results);
    void traverseNode(GumboNode *node, int depth, std::function<void(GumboNode *, int)> callback);
    void printAttributes(const GumboElement *element, int depth);
    static void printNode(GumboNode *node, int depth);
    void traverseNodeWithParent(GumboNode *node, ControlHandle parent);
    static std::string generateUniqueId()
    {
        static std::atomic<int> counter(0);
        return "auto_id_" + std::to_string(counter++);
    }
    std::string extractTextContent(GumboNode *node);

};

} // namespace WebParser
