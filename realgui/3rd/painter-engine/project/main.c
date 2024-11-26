#include "PainterEngine.h"


static px_void PX_Object_3DModel_PixelShader(px_surface *psurface, px_int x, px_int y,
                                             px_point position, px_float u, px_float v, px_point4D normal, px_texture *pTexture, px_color color)
{
    px_float alpha;
    px_float cosv = PX_Point4DDot(PX_Point4DUnit(normal), PX_POINT4D(0, 0, 1));

    cosv = -cosv;
    if (cosv > 0)
    {
        alpha = (1 - cosv) * 128;
        PX_SurfaceDrawPixel(psurface, x, y, PX_COLOR(255, (px_uchar)(128 + alpha), (px_uchar)(0 + alpha),
                                                     (px_uchar)(128 + alpha)));
    }
}

static px_float angle = 0;
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

    PX_SurfaceClearAll(& pDesc->renderSurface, PX_COLOR(0, 255, 255, 255));

    PX_Object_3DModelSetWorld(pObject, 0, 0, 5.0f, 0, 0, angle++, 1);
    PX_3D_Scene(&pDesc->renderlist, &pDesc->world, &pDesc->camera);
    PX_3D_Present(&pDesc->renderSurface, &pDesc->renderlist, &pDesc->camera);
    PX_SurfaceRender(psurface, &pDesc->renderSurface, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER,
                     PX_NULL);
}

int main()
{
    PX_3D_ObjectData data;
    PX_IO_Data io;
    PX_Object *pObject;
    PainterEngine_Initialize(600, 600);
    io = PX_LoadFileToIOData("assets/20255_Triangular_Prism_V1.obj");
    if (io.size == 0) { return PX_FALSE; }
    PX_3D_ObjectDataInitialize(mp_static, &data);
    if (!PX_3D_ObjectDataLoad(&data, io.buffer, io.size)) { return PX_FALSE; }

    pObject = PX_Object_3DModelCreate(mp, root, 300, 300, 600, 600, &data);
    PX_Object_3DModelSetWorld(pObject, 0, 0, 2.0f, 0, 0, 0, 1);//no change this

    PX_Object_3DModel *pDesc = PX_ObjectGetDesc(PX_Object_3DModel, pObject);
    PX_3D_CameraUVNInitialize(mp, &pDesc->camera, PX_POINT4D(0, 0, 0), PX_POINT4D(0, 0, 1), 1, 1000, 90,
                              400, 400);
    pObject->Func_ObjectRender = PX_Object_3DModelRender;

    PX_Object_3DModel *pdesc = PX_ObjectGetDesc(PX_Object_3DModel, pObject);
    PX_3D_RenderListSetPixelShader(&pdesc->renderlist, PX_Object_3DModel_PixelShader);

    return 0;
}