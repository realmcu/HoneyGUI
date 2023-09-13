
#include <gui_obj.h>
#include <gui_svg.h>
#include "svg.txt"



void page_tb_svg(void *parent)
{
    // gui_svg_t *svg_file = gui_svg_create_from_file(parent, "test_svg_widget", \
    // "./example/root_image/root_image_sim_vscode/root/svg/activity.svg", 200, 200, 454, 454);

    gui_svg_t *svg_mem = gui_svg_create_from_mem(parent, "test_svg_widget", (uint8_t *)_acactivity,
                                                 sizeof(_acactivity), 0, 0, 454, 454);
    gui_svg_scale(svg_mem, 5.0f, 5.0f);
}
