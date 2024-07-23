#pragma once
#include <string>
#include <gumbo.h>
#include "JsExecutor.h"
#include "gui_obj.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "ControlFactory.h"
#include <vector>
#include <atomic>
#include <algorithm>
namespace WebParser
{

class Web
{
public:
    Web(const std::string &html, int width, int height, gui_obj_t *parent);
    void parseAndRender(const std::string &output_filename);
    void parseAndRender();
    const std::string index;
    gui_obj_t *parent;
    std::string html_content;
    std::string folder;
    GumboOutput *output_;
private:
    void readFile(const std::string &filename)
    {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        html_content = buffer.str();
        size_t last_slash_idx = filename.find_last_of("\\/");
        if (last_slash_idx == std::string::npos)
        {
            throw std::runtime_error("Path does not contain a directory separator.");
        }
        folder = filename.substr(0, last_slash_idx);
    }
    void traverseNodeWithParent(GumboNode *node, ControlHandle parent)
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
        ControlHandle newParent = ControlFactory::getInstance().createControl(parent, tag, attributes,
                                                                              folder);

        for (unsigned int i = 0; i < element->children.length; ++i)
        {
            traverseNodeWithParent(static_cast<GumboNode *>(element->children.data[i]), newParent);
        }
    }
    std::map<std::string, std::string> parseAttributes(const GumboElement *element)
    {
        std::map<std::string, std::string> attributes;
        for (unsigned int i = 0; i < element->attributes.length; ++i)
        {
            GumboAttribute *attribute = static_cast<GumboAttribute *>(element->attributes.data[i]);
            attributes[attribute->name] = attribute->value;
        }
        return attributes;
    }
    std::string extractTextContent(GumboNode *node)
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
    void traverseBodyWithParent(ControlHandle parent)
    {
        std::vector<GumboNode *> bodyNodes = findElementsByTag("body");
        if (bodyNodes.empty()) { return; }

        traverseNodeWithParent(bodyNodes[0], parent);
    }
    void searchForElements(GumboNode *node, GumboTag tag,
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

    std::vector<GumboNode *> findElementsByTag(const std::string &tag)
    {
        GumboTag gumboTag = gumbo_tag_enum(tag.c_str());
        std::vector<GumboNode *> results;
        searchForElements(output_->root, gumboTag, results);
        return results;
    }

    std::vector<GumboNode *> findChildElementsByTag(GumboNode *node,
                                                    const std::string &tag)
    {
        GumboTag gumboTag = gumbo_tag_enum(tag.c_str());
        std::vector<GumboNode *> results;
        searchForElements(node, gumboTag, results);
        return results;
    }
    static std::string generateUniqueId()
    {
        static std::atomic<int> counter(0);
        return "auto_id_" + std::to_string(counter++);
    }
    std::map<std::string, std::map<std::string, std::string>> styles;
    void cssParser(const std::string &cssContent)
    {
        std::istringstream stream(cssContent);
        std::string line;
        std::string selector;
        bool inBlock = false;

        while (std::getline(stream, line))
        {

            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

            if (line.empty()) { continue; }

            if (line.back() == '{')
            {
                selector = line.substr(0, line.size() - 1);
                inBlock = true;
            }
            else if (line.back() == '}')
            {
                inBlock = false;
            }
            else if (inBlock)
            {

                auto pos = line.find(':');
                if (pos != std::string::npos)
                {

                    std::string property = line.substr(0, pos);
                    std::string value = line.substr(pos + 1, line.size() - pos - 2);

                    styles[selector][property] = value;
                }
            }
        }
    }
    void extractCSSFromHTML()
    {
        GumboOutput *output = output_;

        // Function to recursively search for <style> tags and extract their content
        std::function<void(GumboNode *)> searchStyleTags = [&](GumboNode * node)
        {
            if (node->type != GUMBO_NODE_ELEMENT)
            {
                return;
            }
            GumboVector *children = &node->v.element.children;
            if (node->v.element.tag == GUMBO_TAG_STYLE)
            {
                for (unsigned int i = 0; i < children->length; ++i)
                {
                    GumboNode *child = static_cast<GumboNode *>(children->data[i]);
                    if (child->type == GUMBO_NODE_TEXT)
                    {
                        cssString += child->v.text.text;
                        cssString += "\n";
                    }
                }
            }
            for (unsigned int i = 0; i < children->length; ++i)
            {
                searchStyleTags(static_cast<GumboNode *>(children->data[i]));
            }
        };

        searchStyleTags(output->root);
    }
    std::string cssString;
};

} // namespace WebParser
