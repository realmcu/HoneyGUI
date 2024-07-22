#include "HtmlParser.h"
#include <stdexcept>
#include <iostream>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "ControlFactory.h"
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

std::vector<GumboNode *> HtmlParser::findChildElementsByTag(GumboNode *node,
                                                            const std::string &tag)
{
    GumboTag gumboTag = gumbo_tag_enum(tag.c_str());
    std::vector<GumboNode *> results;
    searchForElements(node, gumboTag, results);
    return results;
}

void HtmlParser::traverseNode(GumboNode *node, int depth,
                              std::function<void(GumboNode *, int)> callback)
{
    if (node->type != GUMBO_NODE_ELEMENT && node->type != GUMBO_NODE_TEXT)
    {
        return;
    }

    callback(node, depth);

    if (node->type == GUMBO_NODE_ELEMENT)
    {
        GumboElement *element = &node->v.element;
        for (unsigned int i = 0; i < element->children.length; ++i)
        {
            traverseNode(static_cast<GumboNode *>(element->children.data[i]), depth + 1, callback);
        }
    }
}


void HtmlParser::printAttributes(const GumboElement *element, int depth)
{
    for (unsigned int i = 0; i < element->attributes.length; ++i)
    {
        for (int j = 0; j < depth + 1; ++j)
        {
            std::cout << "  ";
        }
        GumboAttribute *attribute = static_cast<GumboAttribute *>(element->attributes.data[i]);
        std::cout << attribute->name << "=\"" << attribute->value << "\"" << std::endl;
    }
}
void HtmlParser::printNode(GumboNode *node, int depth)
{
    for (int i = 0; i < depth; ++i)
    {
        std::cout << "  ";
    }

    if (node->type == GUMBO_NODE_ELEMENT)
    {
        const char *tagname = gumbo_normalized_tagname(node->v.element.tag);
        std::cout << "<" << tagname << ">" << std::endl;

        // Print attributes
        const GumboElement *element = &node->v.element;
        for (unsigned int i = 0; i < element->attributes.length; ++i)
        {
            GumboAttribute *attribute = static_cast<GumboAttribute *>(element->attributes.data[i]);
            for (int j = 0; j < depth + 1; ++j)
            {
                std::cout << "  ";
            }
            std::cout << attribute->name << "=\"" << attribute->value << "\"" << std::endl;
        }
    }
    else if (node->type == GUMBO_NODE_TEXT)
    {
        std::cout << node->v.text.text;
    }
}
std::map<std::string, std::string> HtmlParser::parseAttributes(const GumboElement *element)
{
    std::map<std::string, std::string> attributes;
    for (unsigned int i = 0; i < element->attributes.length; ++i)
    {
        GumboAttribute *attribute = static_cast<GumboAttribute *>(element->attributes.data[i]);
        attributes[attribute->name] = attribute->value;
    }
    return attributes;
}
void HtmlParser::traverseNodeWithParent(GumboNode *node, ControlHandle parent)
{
    if (node->type != GUMBO_NODE_ELEMENT) { return; }

    const char *tagname = gumbo_normalized_tagname(node->v.element.tag);
    std::string tag(tagname);

    const GumboElement *element = &node->v.element;
    std::map<std::string, std::string> attributes = parseAttributes(element);

    // Check and/or generate ID
    if (attributes.find("id") == attributes.end())
    {
        attributes["id"] = generateUniqueId(); // cppcheck-suppress stlFindInsert
    }

    // Extract text content from the node
    if (tag == "div")
    {
        attributes["text"] = extractTextContent(node);
    }

    // Add default placeholder values for location and size if not present
    if (attributes.find("x") == attributes.end()) { attributes["x"] = "0"; }// cppcheck-suppress stlFindInsert
    if (attributes.find("y") == attributes.end()) { attributes["y"] = "0"; }// cppcheck-suppress stlFindInsert
    if (attributes.find("width") == attributes.end()) { attributes["width"] = "100"; }// cppcheck-suppress stlFindInsert
    if (attributes.find("height") == attributes.end()) { attributes["height"] = "100"; }// cppcheck-suppress stlFindInsert

    // Create control and get new parent for children
    ControlHandle newParent = ControlFactory::getInstance().createControl(parent, tag, attributes);

    for (unsigned int i = 0; i < element->children.length; ++i)
    {
        traverseNodeWithParent(static_cast<GumboNode *>(element->children.data[i]), newParent);
    }
}
// Extract the text content from a node
std::string HtmlParser::extractTextContent(GumboNode *node)
{
    if (node->type == GUMBO_NODE_TEXT)
    {
        return std::string(node->v.text.text);
    }

    std::stringstream textContent;
    if (node->type == GUMBO_NODE_ELEMENT)
    {
        const GumboElement *element = &node->v.element;
        for (unsigned int i = 0; i < element->children.length; ++i)
        {
            textContent << extractTextContent(static_cast<GumboNode *>(element->children.data[i]));
        }
    }
    return textContent.str();
}
void HtmlParser::traverseBodyWithParent(ControlHandle parent)
{
    std::vector<GumboNode *> bodyNodes = findElementsByTag("body");
    if (bodyNodes.empty()) { return; }

    traverseNodeWithParent(bodyNodes[0], parent);
}
} // namespace WebParser
