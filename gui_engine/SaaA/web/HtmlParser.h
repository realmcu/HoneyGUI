#pragma once
#include <string>
#include <vector>
#include <gumbo.h>

namespace WebParser
{

class HtmlParser
{
public:
    HtmlParser(const std::string &html);
    ~HtmlParser();

    std::vector<GumboNode *> findElementsByTag(const std::string &tag);

private:
    GumboOutput *output_;
    void searchForElements(GumboNode *node, GumboTag tag, std::vector<GumboNode *> &results);
};

} // namespace WebParser
