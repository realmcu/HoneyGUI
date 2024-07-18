#include "HtmlParser.h"
#include <gumbo.h>
#include <stdexcept>

namespace WebParser
{

HtmlParser::HtmlParser(const std::string &html)
{
    output_ = gumbo_parse(html.c_str());
    if (!output_)
    {
        throw std::runtime_error("Failed to parse HTML.");
    }
}

HtmlParser::~HtmlParser()
{
    gumbo_destroy_output(&kGumboDefaultOptions, output_);
}

void HtmlParser::searchForElements(GumboNode *node, GumboTag tag,
                                   std::vector<GumboNode *> &results)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    GumboElement *element = &node->v.element;
    if (element->tag == tag)
    {
        results.push_back(node);
    }

    for (unsigned int i = 0; i < element->children.length; ++i)
    {
        searchForElements(static_cast<GumboNode *>(element->children.data[i]), tag, results);
    }
}

std::vector<GumboNode *> HtmlParser::findElementsByTag(const std::string &tag)
{
    GumboTag gumboTag = gumbo_tag_enum(tag.c_str());
    std::vector<GumboNode *> results;
    searchForElements(output_->root, gumboTag, results);
    return results;
}

} // namespace WebParser
