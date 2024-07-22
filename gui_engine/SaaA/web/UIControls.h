#pragma once
#include <string>
#include <map>
#include <iostream>
#include "gui_img.h"
#include "gui_text.h"
#include "gui_canvas.h"
#include "gui_win.h"
using ControlHandle = void*;  // Assuming generic control handle type; replace with actual type
ControlHandle createButton(ControlHandle parent,
                           const std::map<std::string, std::string> &attributes)
{
    std::cout << "Creating Button with parent " << parent << " and attributes:" << std::endl;
    for (const auto &attr : attributes)
    {
        std::cout << "  " << attr.first << " = " << attr.second << std::endl;
    }
    // Replace with actual control creation logic

    ControlHandle newButton = reinterpret_cast<void *>(1) ;
    return newButton;
}

ControlHandle createLabel(ControlHandle parent,
                          const std::map<std::string, std::string> &attributes)
{
    std::cout << "Creating Label with parent " << parent << " and attributes:" << std::endl;
    for (const auto &attr : attributes)
    {
        std::cout << "  " << attr.first << " = " << attr.second << std::endl;
    }
    // Replace with actual control creation logic
    ControlHandle newButton = reinterpret_cast<void *>(2) ;
    return newButton;
}
ControlHandle createDiv(ControlHandle parent, const std::map<std::string, std::string> &attributes)
{
    std::string name = attributes.at("id");
    int16_t x = std::stoi(attributes.at("x"));
    int16_t y = std::stoi(attributes.at("y"));
    int16_t w = std::stoi(attributes.at("width"));
    int16_t h = std::stoi(attributes.at("height"));
    const char *text = nullptr;


    if (attributes.find("text") != attributes.end() && !attributes.at("text").empty())
    {
        text = attributes.at("text").c_str();
    }
    std::cout << "Creating Div with parent " << parent
              << ", name: " << name << ", x: " << x << ", y: " << y
              << ", width: " << w << ", height: " << h
              << ", text: " << (text ? text : "none") << std::endl;

    gui_win_t *newDiv = gui_win_create(parent, name.c_str(), x, y, w, h);
    if (text != nullptr)
    {
        const char *txt = gui_strdup(text);
        int font_size = 16;
        gui_text_t *t = gui_text_create(newDiv, (name + "_text").c_str(),  0, 0,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, (void *)txt, GUI_FONT_SRC_BMP, APP_COLOR_RED, strlen(txt), font_size);
        extern const unsigned char arialbd_size16_bits4_font[47056];
        void *addr1 = (void *)(arialbd_size16_bits4_font);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);

    }


    return static_cast<ControlHandle>(newDiv);
}
ControlHandle createImg(ControlHandle parent, const std::map<std::string, std::string> &attributes)
{

    std::string name = attributes.at("id");
    std::string filePath = attributes.at("src");
    int16_t x = std::stoi(attributes.at("x"));
    int16_t y = std::stoi(attributes.at("y"));
    int16_t w = std::stoi(attributes.at("width"));
    int16_t h = std::stoi(attributes.at("height"));


    std::cout << "Creating Image with parent " << parent
              << ", name: " << name << ", src: " << filePath
              << ", x: " << x << ", y: " << y
              << ", width: " << w << ", height: " << h << std::endl;

    gui_img_t *newImg = gui_img_create_from_fs(parent, name.c_str(), gui_strdup(filePath.c_str()), x, y,
                                               w, h);

    return static_cast<ControlHandle>(newImg);
}

ControlHandle createCanvas(ControlHandle parent,
                           const std::map<std::string, std::string> &attributes)
{
    std::cout << "Creating Canvas with parent " << parent << " and attributes:" << std::endl;
    for (const auto &attr : attributes)
    {
        std::cout << "  " << attr.first << " = " << attr.second << std::endl;
    }
    // Replace with actual control creation logic
    ControlHandle newButton = reinterpret_cast<void *>(5);
    return newButton;
}

// Add more control creation functions as needed
