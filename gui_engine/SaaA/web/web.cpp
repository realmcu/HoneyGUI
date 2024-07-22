#include "web.h"

namespace WebParser
{

Web::Web(const std::string &html, int width, int height)
    : htmlParser_(html) {}

void Web::parseAndRender(const std::string &output_filename)
{
    // Example: Find all <script> tags and execute their content
    std::vector<GumboNode *> script_nodes = htmlParser_.findElementsByTag("script");
    for (GumboNode *node : script_nodes)
    {
        GumboElement *element = &node->v.element;
        for (unsigned int i = 0; i < element->children.length; ++i)
        {
            GumboNode *child = static_cast<GumboNode *>(element->children.data[i]);
            if (child->type == GUMBO_NODE_TEXT)
            {
                std::string script_content(child->v.text.text);
                jsExecutor_.executeScript(script_content);
            }
        }
    }
}
void Web::parseAndRender()
{
    // Example: Find all <script> tags and execute their content
    std::vector<GumboNode *> script_nodes = htmlParser_.findElementsByTag("script");
    for (GumboNode *node : script_nodes)
    {
        GumboElement *element = &node->v.element;
        for (unsigned int i = 0; i < element->children.length; ++i)
        {
            GumboNode *child = static_cast<GumboNode *>(element->children.data[i]);
            if (child->type == GUMBO_NODE_TEXT)
            {
                std::string script_content(child->v.text.text);
                jsExecutor_.executeScript(script_content);
            }
        }
    }
}
} // namespace WebParser

#include "HtmlParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "gui_obj.h"
std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}



extern "C" {
    int gui_web_open_html(const char *html_file_path, gui_obj_t *parent)
    {

        const std::string html_file = html_file_path;
        std::string html_content = readFile(html_file);

        WebParser::HtmlParser htmlParser(html_content);

        // Assuming the root parent is nullptr
        ControlHandle rootParent = parent;

        std::cout << "Creating controls from <body> tag:" << std::endl;
        htmlParser.traverseBodyWithParent(rootParent);

        WebParser::Web watchFace(html_content, 200, 200);
        watchFace.parseAndRender();

        return EXIT_SUCCESS;
    }
}
