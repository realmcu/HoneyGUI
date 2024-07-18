#pragma once
#include "gui_img.h"
#include "gui_canvas.h"
#include <string>
namespace WebParser
{

class Renderer
{
public:
    Renderer(int width, int height);
    ~Renderer();

    void drawWeb();
    void saveToFile(const std::string &filename);

private:

};

} // namespace WebParser
