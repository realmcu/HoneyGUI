#include "web.h"

namespace WebParser
{

Web::Web(const std::string &html, int width, int height, gui_obj_t *parent)
    : index(html), parent(parent)
{
    GUI_ROOT_FOLDER = (char *)"";
    readFile(index);
    output_ = gumbo_parse(html_content.c_str());
    if (!output_)
    {
        throw std::runtime_error("Failed to parse HTML.");
    }
    extractCSSFromHTML();

    cssParser(cssString);
    traverseBodyWithParent(parent);
}


// void Web::parseAndRender()
// {
//     // Example: Find all <script> tags and execute their content
//     std::vector<GumboNode *> script_nodes = htmlParser_.findElementsByTag("script");
//     for (GumboNode *node : script_nodes)
//     {
//         GumboElement *element = &node->v.element;
//         for (unsigned int i = 0; i < element->children.length; ++i)
//         {
//             GumboNode *child = static_cast<GumboNode *>(element->children.data[i]);
//             if (child->type == GUMBO_NODE_TEXT)
//             {
//                 std::string script_content(child->v.text.text);
//                 jsExecutor_.executeScript(script_content);
//             }
//         }
//     }
// }
} // namespace WebParser


#include <iostream>
#include <fstream>
#include <sstream>
#include "gui_obj.h"




extern "C" {
    int gui_web_open_html(const char *html_file_path, gui_obj_t *parent)
    {

        const std::string html_file = html_file_path;
        WebParser::Web web(html_file, 454, 454, parent);




        return EXIT_SUCCESS;
    }
}
