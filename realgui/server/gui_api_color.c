/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_color.c
  * @brief Application Programming Interface for UI
  * @details Color Define
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <ctype.h>
#include "guidef.h"
#include "gui_api_color.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/



/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

// Function to convert a string to lowercase
static void to_lowercase(char *str)
{
    if (str)
    {
        for (; *str; ++str) { *str = tolower(*str); }
    }
}

static void hsl_to_rgb(int h, double s, double l, unsigned char *r, unsigned char *g,
                       unsigned char *b)
{
    double c = (1.0 - fabs(2.0 * l - 1.0)) * s;
    double x = c * (1.0 - fabs(fmod((double)h / 60.0, 2) - 1));
    double m = l - c / 2.0;

    double r_temp, g_temp, b_temp;

    if (h >= 0 && h < 60)
    {
        r_temp = c;
        g_temp = x;
        b_temp = 0;
    }
    else if (h >= 60 && h < 120)
    {
        r_temp = x;
        g_temp = c;
        b_temp = 0;
    }
    else if (h >= 120 && h < 180)
    {
        r_temp = 0;
        g_temp = c;
        b_temp = x;
    }
    else if (h >= 180 && h < 240)
    {
        r_temp = 0;
        g_temp = x;
        b_temp = c;
    }
    else if (h >= 240 && h < 300)
    {
        r_temp = x;
        g_temp = 0;
        b_temp = c;
    }
    else
    {
        r_temp = c;
        g_temp = 0;
        b_temp = x;
    }

    *r = (unsigned char)((r_temp + m) * 255.0);
    *g = (unsigned char)((g_temp + m) * 255.0);
    *b = (unsigned char)((b_temp + m) * 255.0);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_color_t gui_rgba(unsigned char red,
                     unsigned char green,
                     unsigned char blue,
                     unsigned char opacity)
{
    gui_color_t color;

    // Use longer initialization to suppress warning.
    color.color.rgba.r = red;
    color.color.rgba.g = green;
    color.color.rgba.b = blue;
    color.color.rgba.a = opacity;

    return color;
}

gui_color_t gui_rgb(unsigned char red, unsigned char green, unsigned char blue)
{
    gui_color_t color;

    // Use longer initialization to suppress warning.
    color.color.rgba.r = red;
    color.color.rgba.g = green;
    color.color.rgba.b = blue;
    color.color.rgba.a = UINT8_MAX;

    return color;
}

gui_color_t gui_color_css(const char *color_str)
{
    gui_color_t color = { .color.argb_full = 0 };

    if (color_str == NULL)
    {
        return color;
    }

    // Check for hex format (#RRGGBB or #RRGGBBAA)
    if (color_str[0] == '#')
    {
        size_t len = strlen(color_str);
        unsigned int red, green, blue, opacity;

        if (len == 7)
        {
            // Parse the #RRGGBB format
            if (sscanf(color_str, "#%2x%2x%2x", &red, &green, &blue) != 3)
            {
                return color;
            }
            opacity = 0xFF;  // Default to full opacity
        }
        else if (len == 9)
        {
            // Parse the #RRGGBBAA format
            if (sscanf(color_str, "#%2x%2x%2x%2x", &red, &green, &blue, &opacity) != 4)
            {
                return color;
            }
        }
        else
        {
            return color;
        }

        return gui_rgba((unsigned char)red,
                        (unsigned char)green,
                        (unsigned char)blue,
                        (unsigned char)opacity);
    }

    // Check for rgb/rgba format
    if (strncmp(color_str, "rgb", 3) == 0)
    {
        int red, green, blue;
        float alpha = 1.0f;
        if (color_str[3] == 'a')
        {
            if (sscanf(color_str, "rgba(%d, %d, %d, %f)", &red, &green, &blue, &alpha) == 4)
            {
                return gui_rgba((unsigned char)red,
                                (unsigned char)green,
                                (unsigned char)blue,
                                (unsigned char)roundf(alpha * 255));
            }
        }
        else
        {
            if (sscanf(color_str, "rgb(%d, %d, %d)", &red, &green, &blue) == 3)
            {
                return gui_rgba((unsigned char)red,
                                (unsigned char)green,
                                (unsigned char)blue,
                                0xFF);
            }
        }
    }

    // Check for hsl/hsla format
    if (strncmp(color_str, "hsl", 3) == 0)
    {
        int hue, saturation, lightness;
        float alpha = 1.0f;
        if (color_str[3] == 'a')
        {
            if (sscanf(color_str, "hsla(%d, %d%%, %d%%, %f)", &hue, &saturation, &lightness, &alpha) == 4)
            {
                unsigned char r, g, b;
                hsl_to_rgb(hue, saturation / 100.0, lightness / 100.0, &r, &g, &b);
                return gui_rgba(r, g, b, (unsigned char)roundf(alpha * 255));
            }
        }
        else
        {
            if (sscanf(color_str, "hsl(%d, %d%%, %d%%)", &hue, &saturation, &lightness) == 3)
            {
                unsigned char r, g, b;
                hsl_to_rgb(hue, saturation / 100.0, lightness / 100.0, &r, &g, &b);
                return gui_rgba(r, g, b, 0xFF);
            }
        }
    }

    // If no other format matches, check for named colors
    typedef struct
    {
        const char *name; /**< Color name */
        const char *hex;  /**< Hex string representation of the color */
    } color_name_t;

    // Define the color name mapping table
    static const color_name_t color_names[] =
    {
        { "aliceblue", "#F0F8FFFF" },
        { "antiquewhite", "#FAEBD7FF" },
        { "aqua", "#00FFFFFF" },
        { "aquamarine", "#7FFFD4FF" },
        { "azure", "#F0FFFFFF" },
        { "beige", "#F5F5DCFF" },
        { "bisque", "#FFE4C4FF" },
        { "black", "#000000FF" },
        { "blanchedalmond", "#FFEBCDFF" },
        { "blue", "#0000FFFF" },
        { "blueviolet", "#8A2BE2FF" },
        { "brown", "#A52A2AFF" },
        { "burlywood", "#DEB887FF" },
        { "cadetblue", "#5F9EA0FF" },
        { "chartreuse", "#7FFF00FF" },
        { "chocolate", "#D2691EFF" },
        { "coral", "#FF7F50FF" },
        { "cornflowerblue", "#6495EDFF" },
        { "cornsilk", "#FFF8DCFF" },
        { "crimson", "#DC143CFF" },
        { "cyan", "#00FFFFFF" },
        { "darkblue", "#00008BFF" },
        { "darkcyan", "#008B8BFF" },
        { "darkgoldenrod", "#B8860BFF" },
        { "darkgray", "#A9A9A9FF" },
        { "darkgreen", "#006400FF" },
        { "darkgrey", "#A9A9A9FF" },
        { "darkkhaki", "#BDB76BFF" },
        { "darkmagenta", "#8B008BFF" },
        { "darkolivegreen", "#556B2FFF" },
        { "darkorange", "#FF8C00FF" },
        { "darkorchid", "#9932CCFF" },
        { "darkred", "#8B0000FF" },
        { "darksalmon", "#E9967AFF" },
        { "darkseagreen", "#8FBC8FFF" },
        { "darkslateblue", "#483D8BFF" },
        { "darkslategray", "#2F4F4FFF" },
        { "darkslategrey", "#2F4F4FFF" },
        { "darkturquoise", "#00CED1FF" },
        { "darkviolet", "#9400D3FF" },
        { "deeppink", "#FF1493FF" },
        { "deepskyblue", "#00BFFFFF" },
        { "dimgray", "#696969FF" },
        { "dimgrey", "#696969FF" },
        { "dodgerblue", "#1E90FFFF" },
        { "firebrick", "#B22222FF" },
        { "floralwhite", "#FFFAF0FF" },
        { "forestgreen", "#228B22FF" },
        { "fuchsia", "#FF00FFFF" },
        { "gainsboro", "#DCDCDCFF" },
        { "ghostwhite", "#F8F8FFFF" },
        { "gold", "#FFD700FF" },
        { "goldenrod", "#DAA520FF" },
        { "gray", "#808080FF" },
        { "green", "#00FF00FF" },
        { "greenyellow", "#ADFF2FFF" },
        { "grey", "#808080FF" },
        { "honeydew", "#F0FFF0FF" },
        { "hotpink", "#FF69B4FF" },
        { "indianred", "#CD5C5CFF" },
        { "indigo", "#4B0082FF" },
        { "ivory", "#FFFFF0FF" },
        { "khaki", "#F0E68CFF" },
        { "lavender", "#E6E6FAFF" },
        { "lavenderblush", "#FFF0F5FF" },
        { "lawngreen", "#7CFC00FF" },
        { "lemonchiffon", "#FFFACDFF" },
        { "lightblue", "#ADD8E6FF" },
        { "lightcoral", "#F08080FF" },
        { "lightcyan", "#E0FFFFFF" },
        { "lightgoldenrodyellow", "#FAFAD2FF" },
        { "lightgray", "#D3D3D3FF" },
        { "lightgreen", "#90EE90FF" },
        { "lightgrey", "#D3D3D3FF" },
        { "lightpink", "#FFB6C1FF" },
        { "lightsalmon", "#FFA07AFF" },
        { "lightseagreen", "#20B2AAFF" },
        { "lightskyblue", "#87CEFAFF" },
        { "lightslategray", "#778899FF" },
        { "lightslategrey", "#778899FF" },
        { "lightsteelblue", "#B0C4DEFF" },
        { "lightyellow", "#FFFFE0FF" },
        { "lime", "#00FF00FF" },
        { "limegreen", "#32CD32FF" },
        { "linen", "#FAF0E6FF" },
        { "magenta", "#FF00FFFF" },
        { "maroon", "#800000FF" },
        { "mediumaquamarine", "#66CDAAFF" },
        { "mediumblue", "#0000CDFF" },
        { "mediumorchid", "#BA55D3FF" },
        { "mediumpurple", "#9370DBFF" },
        { "mediumseagreen", "#3CB371FF" },
        { "mediumslateblue", "#7B68EEFF" },
        { "mediumspringgreen", "#00FA9AFF" },
        { "mediumturquoise", "#48D1CCFF" },
        { "mediumvioletred", "#C71585FF" },
        { "midnightblue", "#191970FF" },
        { "mintcream", "#F5FFFAFF" },
        { "mistyrose", "#FFE4E1FF" },
        { "moccasin", "#FFE4B5FF" },
        { "navajowhite", "#FFDEADFF" },
        { "navy", "#000080FF" },
        { "oldlace", "#FDF5E6FF" },
        { "olive", "#808000FF" },
        { "olivedrab", "#6B8E23FF" },
        { "orange", "#FFA500FF" },
        { "orangered", "#FF4500FF" },
        { "orchid", "#DA70D6FF" },
        { "palegoldenrod", "#EEE8AAFF" },
        { "palegreen", "#98FB98FF" },
        { "paleturquoise", "#AFEEEEFF" },
        { "palevioletred", "#DB7093FF" },
        { "papayawhip", "#FFEFD5FF" },
        { "peachpuff", "#FFDAB9FF" },
        { "peru", "#CD853FFF" },
        { "pink", "#FFC0CBFF" },
        { "plum", "#DDA0DDFF" },
        { "powderblue", "#B0E0E6FF" },
        { "purple", "#800080FF" },
        { "rebeccapurple", "#663399FF" },
        { "red", "#FF0000FF" },
        { "rosybrown", "#BC8F8FFF" },
        { "royalblue", "#4169E1FF" },
        { "saddlebrown", "#8B4513FF" },
        { "salmon", "#FA8072FF" },
        { "sandybrown", "#F4A460FF" },
        { "seagreen", "#2E8B57FF" },
        { "seashell", "#FFF5EEFF" },
        { "sienna", "#A0522DFF" },
        { "silver", "#C0C0C0FF" },
        { "skyblue", "#87CEEBFF" },
        { "slateblue", "#6A5ACDFF" },
        { "slategray", "#708090FF" },
        { "slategrey", "#708090FF" },
        { "snow", "#FFFAFAFF" },
        { "springgreen", "#00FF7FFF" },
        { "steelblue", "#4682B4FF" },
        { "tan", "#D2B48CFF" },
        { "teal", "#008080FF" },
        { "thistle", "#D8BFD8FF" },
        { "tomato", "#FF6347FF" },
        { "turquoise", "#40E0D0FF" },
        { "violet", "#EE82EEFF" },
        { "wheat", "#F5DEB3FF" },
        { "white", "#FFFFFFFF" },
        { "whitesmoke", "#F5F5F5FF" },
        { "yellow", "#FFFF00FF" },
        { "yellowgreen", "#9ACD32FF" },
    };


#define COLOR_NAME_COUNT (sizeof(color_names) / sizeof(color_names[0]))

    // Create a mutable copy of the input string
    char color_str_copy[100];
    strncpy(color_str_copy, color_str, sizeof(color_str_copy) - 1);
    color_str_copy[sizeof(color_str_copy) - 1] = '\0';

    // Convert the copy to lowercase
    to_lowercase(color_str_copy);

    for (size_t i = 0; i < COLOR_NAME_COUNT; ++i)
    {
        if (strcmp(color_names[i].name, color_str_copy) == 0)
        {
            color_str = color_names[i].hex;
            break;
        }
    }

    // Check for hex format again after potential named color matching
    if (color_str[0] == '#')
    {
        size_t len = strlen(color_str);
        unsigned int red, green, blue, opacity;

        if (len == 7)
        {
            // Parse the #RRGGBB format
            if (sscanf(color_str, "#%2x%2x%2x", &red, &green, &blue) != 3)
            {
                return color;
            }
            opacity = 0xFF;  // Default to full opacity
        }
        else if (len == 9)
        {
            // Parse the #RRGGBBAA format
            if (sscanf(color_str, "#%2x%2x%2x%2x", &red, &green, &blue, &opacity) != 4)
            {
                return color;
            }
        }
        else
        {
            return color;
        }

        return gui_rgba((unsigned char)red,
                        (unsigned char)green,
                        (unsigned char)blue,
                        (unsigned char)opacity);
    }

    // Error case, return color with all components set to 0
    return color;
}

#ifdef  GUI_USE_ARM_MATH
#include "arm_math.h"
void gui_memset16(uint16_t *addr, uint16_t pixel, uint32_t len) //rgb565
{
    arm_fill_q15(pixel, (int16_t *)addr, len);
}
void gui_memset32(uint32_t *addr, uint32_t pixel, uint32_t len)  //argb8888
{
    arm_fill_q31(pixel, (int32_t *)addr, len);
}
#else
void gui_memset16(uint16_t *addr, uint16_t pixel, uint32_t len) //rgb565
{
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
}
void gui_memset32(uint32_t *addr, uint32_t pixel, uint32_t len)  //argb8888
{
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
}
#endif

void gui_fb_clear(uint8_t *addr, gui_color_t color, uint32_t len/*pixel count*/)  //argb8888
{
    extern gui_dispdev_t *gui_get_dc(void);
    gui_dispdev_t *dc = gui_get_dc();
    if (dc->bit_depth == 16)
    {
        uint16_t pixel = 0;
        color_rgb565_t *p = (color_rgb565_t *)(uintptr_t)&pixel;
        p->b = (color.color.rgba.b >> 3) & 0x1F;
        p->g = (color.color.rgba.g >> 2) & 0x3F;
        p->r = (color.color.rgba.r >> 3) & 0x1F;
        gui_memset16((uint16_t *)addr, (uint16_t)pixel, len);
    }
    else if (dc->bit_depth == 32)
    {
        uint32_t pixel = 0;
        color_argb8888_t *p = (color_argb8888_t *)(uintptr_t)&pixel;
        p->a = color.color.rgba.a;
        p->r = color.color.rgba.r;
        p->g = color.color.rgba.g;
        p->b = color.color.rgba.b;
        gui_memset32((uint32_t *)addr, pixel, len);
    }
}


