#pragma once
#include <string>
#include "HtmlParser.h"
#include "JsExecutor.h"
#include "Renderer.h"

namespace WebParser
{

class Web
{
public:
    Web(const std::string &html, int width, int height);
    void parseAndRender(const std::string &output_filename);
    void parseAndRender();
private:
    HtmlParser htmlParser_;
    JsExecutor jsExecutor_;
    Renderer renderer_;
};

} // namespace WebParser
