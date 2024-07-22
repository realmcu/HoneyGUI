#pragma once
#include "UIControls.h"
#include <map>
#include <string>
#include <functional>

class ControlFactory
{
public:
    using ControlCreator =
        std::function<ControlHandle(ControlHandle, const std::map<std::string, std::string>&)>;

    static ControlFactory &getInstance()
    {
        static ControlFactory instance;
        return instance;
    }

    void registerControl(const std::string &tag, ControlCreator creator)
    {
        controlMap_[tag] = creator;
    }


    ControlHandle createControl(ControlHandle parent, const std::string &tag,
                                const std::map<std::string, std::string> &attributes) const
    {
        auto it = controlMap_.find(tag);
        if (it != controlMap_.end())
        {
            return it->second(parent, attributes);
        }
        else
        {
            std::cout << "Unknown control type: " << tag << std::endl;
            return parent;
        }
    }
private:
    ControlFactory()
    {
        // Register controls here
        registerControl("button", createButton);
        registerControl("label", createLabel);
        registerControl("div", createDiv);
        registerControl("img", createImg);
        registerControl("canvas", createCanvas);
        // Add more controls as needed
    }

    std::map<std::string, ControlCreator> controlMap_;
};
