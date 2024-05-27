#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>

#include "gui_server.h"
#include "gui_components_init.h"

#include "gui_px.h"

static px_void PX_Object_3DModel_PixelShader(px_surface *psurface, px_int x, px_int y,
                                             px_point position, px_float u, px_float v, px_point4D normal, px_texture *pTexture, px_color color)
{
    px_float alpha;
    px_float cosv = PX_Point4DDot(PX_Point4DUnit(normal), PX_POINT4D(0, 0, 1));

    cosv = -cosv;
    if (cosv > 0)
    {
        alpha = (1 - cosv) * 128;
        //PX_SurfaceDrawPixel(psurface, x, y, PX_COLOR(255, (px_uchar)(color._argb.r + alpha), (px_uchar)(color._argb.g + alpha), (px_uchar)(color._argb.b + alpha)));
        PX_SurfaceDrawPixel(psurface, x, y, PX_COLOR(255, (px_uchar)(128 + alpha), (px_uchar)(0 + alpha),
                                                     (px_uchar)(128 + alpha)));
    }
}

static px_float rotY = 0;
static PX_OBJECT_RENDER_FUNCTION(PX_Object_3DModelRender)
{
    PX_Object_3DModel *pDesc = PX_ObjectGetDesc(PX_Object_3DModel, pObject);
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
    objx = (pObject->x + inheritX);
    objy = (pObject->y + inheritY);
    objWidth = pObject->Width;
    objHeight = pObject->Height;

    PX_SurfaceClearAll(& pDesc->renderSurface, PX_COLOR(255, 0, 0, 0));
    PX_Object_3DModelSetWorld(pObject, 0, 0, 1.2f, 0, rotY++, 0, 1);
    PX_3D_Scene(&pDesc->renderlist, &pDesc->world, &pDesc->camera);
    PX_3D_Present(&pDesc->renderSurface, &pDesc->renderlist, &pDesc->camera);
    PX_SurfaceRender(psurface, &pDesc->renderSurface, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER,
                     PX_NULL);
}

static void px_main(gui_px_t *this)
{
    gui_log("line = %d, func = %s \n", __LINE__, __FUNCTION__);

    // PX_Object_Firework01Create(this->mp_dynamic, this->px_root, 100, 300);
    // PX_Object_Firework01Create(this->mp_dynamic, this->px_root, 200, 300);
    // PX_Object_Firework01Create(this->mp_dynamic, this->px_root, 300, 300);
    // return;

    PX_3D_ObjectData data;
    PX_IO_Data io;
    PX_Object *pObject;

#if defined _WIN32
    io = PX_LoadFileToIOData("win32_sim/assets/bunny.obj");
#else
    io = PX_LoadFileToIOData("/win32_sim/assets/bunny.obj");
#endif
    if (io.size == 0)
    {
        gui_log("line = %d, func = %s \n", __LINE__, __FUNCTION__);
        return;
    }
    PX_3D_ObjectDataInitialize(this->mp_static, &data);
    if (!PX_3D_ObjectDataLoad(&data, io.buffer, io.size))
    {
        gui_log("line = %d, func = %s \n", __LINE__, __FUNCTION__);
        return;
    }

    pObject = PX_Object_3DModelCreate(this->mp_dynamic, \
                                      this->px_root, \
                                      gui_get_screen_width() / 2, \
                                      gui_get_screen_height() / 2, \
                                      gui_get_screen_width(), gui_get_screen_height(),
                                      &data);

    pObject->Func_ObjectRender = PX_Object_3DModelRender;

    PX_Object_3DModelSetWorld(pObject, 0, 0, 1.2f, 0, 0, 0, 1);
    PX_Object_3DModel *pdesc = PX_ObjectGetDesc(PX_Object_3DModel, pObject);
    PX_3D_RenderListSetPixelShader(&pdesc->renderlist, PX_Object_3DModel_PixelShader);
}


static void app_dialing_ui_design(gui_app_t *app)
{

    gui_px_t *px = gui_px_create(&app->screen, "px-test", px_main, 0, 0, gui_get_screen_width(),
                                 gui_get_screen_height());

}


static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "rtk_gui_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_dialing_ui_design,
    .active_ms = 1000 * 60 * 60,
};


static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);







