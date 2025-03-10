#include "PX_Object_CheckBox.h"

PX_Object_CheckBox *PX_Object_GetCheckBox(PX_Object *pObject)
{
    PX_Object_CheckBox *pcb = (PX_Object_CheckBox *)pObject->pObjectDesc;
    if (pObject->Type == PX_OBJECT_TYPE_CHECKBOX)
    {
        return pcb;
    }
    return PX_NULL;
}

px_void PX_Object_CheckBoxOnMouseMove(PX_Object *pObject, PX_Object_Event e, px_void *user_ptr)
{
    PX_Object_CheckBox *pcb = PX_Object_GetCheckBox(pObject);

    px_float x = (PX_Object_Event_GetCursorX(e));
    px_float y = (PX_Object_Event_GetCursorY(e));

    if (pcb)
    {
        if (PX_ObjectIsPointInRegion(pObject, (px_float)x, (px_float)y))
        {
            if (pcb->state != PX_OBJECT_BUTTON_STATE_ONPUSH)
            {
                if (pcb->state != PX_OBJECT_BUTTON_STATE_ONCURSOR)
                {
                    PX_Object_Event e;
                    e.Event = PX_OBJECT_EVENT_CURSOROVER;
                    e.Param_uint[0] = 0;
                    e.Param_uint[1] = 0;
                    e.Param_uint[2] = 0;
                    e.Param_uint[3] = 0;
                    e.Param_ptr[0] = PX_NULL;
                    PX_ObjectExecuteEvent(pObject, e);
                }
                pcb->state = PX_OBJECT_CHECKBOX_STATE_ONCURSOR;
            }
        }
        else
        {
            if (pcb->state != PX_OBJECT_BUTTON_STATE_NORMAL)
            {
                PX_Object_Event e;
                e.Event = PX_OBJECT_EVENT_CURSOROUT;
                e.Param_uint[0] = 0;
                e.Param_uint[1] = 0;
                e.Param_uint[2] = 0;
                e.Param_uint[3] = 0;
                PX_ObjectExecuteEvent(pObject, e);
            }
            pcb->state = PX_OBJECT_CHECKBOX_STATE_NORMAL;
        }
    }
}

px_void PX_Object_CheckBoxOnMouseLButtonDown(PX_Object *pObject, PX_Object_Event e,
                                             px_void *user_ptr)
{
    PX_Object_CheckBox *pcb = PX_Object_GetCheckBox(pObject);
    px_float x = (PX_Object_Event_GetCursorX(e));
    px_float y = (PX_Object_Event_GetCursorY(e));

    if (pcb)
    {
        if (PX_ObjectIsPointInRegion(pObject, (px_float)x, (px_float)y))
        {
            pcb->state = PX_OBJECT_CHECKBOX_STATE_ONPUSH;
        }
        else
        {
            if (pcb->state != PX_OBJECT_BUTTON_STATE_NORMAL)
            {
                PX_Object_Event e;
                e.Event = PX_OBJECT_EVENT_CURSOROUT;
                e.Param_uint[0] = 0;
                e.Param_uint[1] = 0;
                e.Param_uint[2] = 0;
                e.Param_uint[3] = 0;
                PX_ObjectExecuteEvent(pObject, e);
            }
            pcb->state = PX_OBJECT_CHECKBOX_STATE_NORMAL;
        }
    }
}

px_void PX_Object_CheckBoxOnMouseLButtonUp(PX_Object *pObject, PX_Object_Event e, px_void *user_ptr)
{
    PX_Object_CheckBox *pcb = PX_Object_GetCheckBox(pObject);

    px_float x, y;

    x = PX_Object_Event_GetCursorX(e);
    y = PX_Object_Event_GetCursorY(e);

    if (pcb)
    {
        if (PX_ObjectIsPointInRegion(pObject, (px_float)x, (px_float)y))
            if (pcb->state == PX_OBJECT_CHECKBOX_STATE_ONPUSH)
            {
                PX_Object_Event e;
                pcb->state = PX_OBJECT_CHECKBOX_STATE_ONCURSOR;
                pcb->bCheck = !pcb->bCheck;

                e.Event = PX_OBJECT_EVENT_VALUECHANGED;
                e.Param_uint[0] = 0;
                e.Param_uint[1] = 0;
                e.Param_uint[2] = 0;
                e.Param_uint[3] = 0;
                PX_ObjectExecuteEvent(pObject, e);
            }
    }
}

px_void PX_Object_CheckBoxRender(px_surface *psurface, PX_Object *pObject, px_uint elapsed)
{
    PX_Object_CheckBox *pcb = PX_Object_GetCheckBox(pObject);
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;

    PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

    objx = (pObject->x + inheritX);
    objy = (pObject->y + inheritY);
    objWidth = pObject->Width;
    objHeight = pObject->Height;


    if (pcb == PX_NULL)
    {
        return;
    }

    if (!pObject->Visible)
    {
        return;
    }

    PX_GeoDrawRect(psurface, (px_int)objx, (px_int)objy, (px_int)objx + (px_int)objWidth - 1,
                   (px_int)objy + (px_int)objHeight - 1, pcb->BackgroundColor);
    switch (pcb->state)
    {
    case PX_OBJECT_BUTTON_STATE_NORMAL:
        PX_GeoDrawRect(psurface, (px_int)objx, (px_int)objy, (px_int)objx + (px_int)objWidth - 1,
                       (px_int)objy + (px_int)objHeight - 1, pcb->BackgroundColor);
        break;
    case PX_OBJECT_BUTTON_STATE_ONPUSH:
        PX_GeoDrawRect(psurface, (px_int)objx, (px_int)objy, (px_int)objx + (px_int)objWidth - 1,
                       (px_int)objy + (px_int)objHeight - 1, pcb->PushColor);
        break;
    case PX_OBJECT_BUTTON_STATE_ONCURSOR:
        PX_GeoDrawRect(psurface, (px_int)objx, (px_int)objy, (px_int)objx + (px_int)objWidth - 1,
                       (px_int)objy + (px_int)objHeight - 1, pcb->CursorColor);
        break;
    }

    if (pcb->Border)
    {
        PX_GeoDrawBorder(psurface, (px_int)objx, (px_int)objy, (px_int)objx + (px_int)objWidth - 1,
                         (px_int)objy + (px_int)objHeight - 1, 1, pcb->BorderColor);
    }

    PX_FontModuleDrawText(psurface, pcb->fm, (px_int)objx + 18, (px_int)(objy + objHeight / 2),
                          PX_ALIGN_LEFTMID, pcb->Text, pcb->TextColor);

    //draw CheckState
    PX_GeoDrawBorder(psurface, (px_int)objx, (px_int)(objy + objHeight / 2 - 7), (px_int)objx + 14,
                     (px_int)(objy + objHeight / 2 + 7), 1, pcb->BorderColor);
    if (pcb->bCheck)
    {
        PX_GeoDrawRect(psurface, (px_int)objx + 2, (px_int)(objy + objHeight / 2 - 5), (px_int)objx + 12,
                       (px_int)(objy + objHeight / 2 + 5), pcb->BorderColor);
    }

}

PX_Object *PX_Object_CheckBoxCreate(px_memorypool *mp, PX_Object *Parent, px_int x, px_int y,
                                    px_int Width, px_int Height, const char text[], PX_FontModule *fm)
{
    PX_Object *pObject;
    PX_Object_CheckBox cbx;
    cbx.Align = PX_ALIGN_LEFTMID;
    cbx.BackgroundColor = PX_COLOR(0, 0, 0, 0);
    cbx.bCheck = PX_FALSE;
    cbx.Border = PX_FALSE;
    cbx.BorderColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
    cbx.CursorColor = PX_OBJECT_UI_DEFAULT_CURSORCOLOR;
    cbx.PushColor = PX_OBJECT_UI_DEFAULT_PUSHCOLOR;
    cbx.fm = fm;
    cbx.state = PX_OBJECT_CHECKBOX_STATE_NORMAL;
    PX_strset(cbx.Text, text);
    cbx.TextColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
    pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)Width,
                                (px_float)Height, 0, PX_OBJECT_TYPE_CHECKBOX, PX_NULL, PX_Object_CheckBoxRender, PX_NULL, &cbx,
                                sizeof(cbx));
    if (!pObject)
    {
        return PX_NULL;
    }
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_CheckBoxOnMouseMove, PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_CheckBoxOnMouseLButtonDown,
                           PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_CheckBoxOnMouseLButtonUp,
                           PX_NULL);
    return pObject;
}

px_bool PX_Object_CheckBoxGetCheck(PX_Object *pObject)
{
    return PX_Object_GetCheckBox(pObject)->bCheck;
}

px_void PX_Object_CheckBoxSetBackgroundColor(PX_Object *pObject, px_color clr)
{
    PX_Object_GetCheckBox(pObject)->BackgroundColor = clr;
}

px_void PX_Object_CheckBoxSetBorderColor(PX_Object *pObject, px_color clr)
{
    PX_Object_GetCheckBox(pObject)->BorderColor = clr;
}

px_void PX_Object_CheckBoxSetPushColor(PX_Object *pObject, px_color clr)
{
    PX_Object_GetCheckBox(pObject)->PushColor = clr;
}

px_void PX_Object_CheckBoxSetCursorColor(PX_Object *pObject, px_color clr)
{
    PX_Object_GetCheckBox(pObject)->CursorColor = clr;
}

px_void PX_Object_CheckBoxSetText(PX_Object *pObject, const px_char text[])
{
    PX_strset(PX_Object_GetCheckBox(pObject)->Text, text);
}

px_char *PX_Object_CheckBoxGetText(PX_Object *pObject)
{
    PX_Object_CheckBox *pCheckBox = PX_Object_GetCheckBox(pObject);
    if (pCheckBox)
    {
        return pCheckBox->Text;
    }
    else
    {
        PX_ASSERT();
        return PX_NULL;
    }
}

px_void PX_Object_CheckBoxSetTextColor(PX_Object *pObject, px_color clr)
{
    PX_Object_GetCheckBox(pObject)->TextColor = clr;
}

px_void PX_Object_CheckBoxSetCheck(PX_Object *pObject, px_bool check)
{
    PX_Object_GetCheckBox(pObject)->bCheck = check;
}

//////////////////////////////////////////////////////////////////////////
//checkbox
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_CheckBoxCreate(px_memorypool *mp, PX_Object *pparent, px_float x, px_float y,
                                      px_float width, px_float height, px_void *ptr)
{
    PX_FontModule *fm = (PX_FontModule *)ptr;
    return PX_Object_CheckBoxCreate(mp, pparent, (px_int)x, (px_int)y, 96, 28, "CheckBox", fm);
}

px_void PX_Designer_CheckBoxSetText(PX_Object *pobject, const px_char text[])
{
    PX_Object_CheckBoxSetText(pobject, text);
}

px_bool PX_Designer_CheckBoxGetText(PX_Object *pobject, px_string *str)
{
    return PX_StringSet(str, PX_Object_CheckBoxGetText(pobject));
}

px_void PX_Designer_CheckBoxSetCheck(PX_Object *pobject, px_bool b)
{
    PX_Object_CheckBoxSetCheck(pobject, b);
}

px_bool PX_Designer_CheckBoxGetCheck(PX_Object *pobject)
{
    return PX_Object_CheckBoxGetCheck(pobject);
}

PX_Designer_ObjectDesc PX_Object_CheckBoxDesignerInstall()
{
    PX_Designer_ObjectDesc scheckbox;
    px_int i = 0;
    PX_memset(&scheckbox, 0, sizeof(scheckbox));
    PX_strcat(scheckbox.Name, "checkbox");

    scheckbox.createfunc = PX_Designer_CheckBoxCreate;
    scheckbox.type = PX_DESIGNER_OBJECT_TYPE_UI;

    PX_strcat(scheckbox.properties[i].Name, "id");
    scheckbox.properties[i].getstring = PX_Designer_GetID;
    scheckbox.properties[i].setstring = PX_Designer_SetID;
    i++;

    PX_strcat(scheckbox.properties[i].Name, "x");
    scheckbox.properties[i].getfloat = PX_Designer_GetX;
    scheckbox.properties[i].setfloat = PX_Designer_SetX;
    i++;

    PX_strcat(scheckbox.properties[i].Name, "y");
    scheckbox.properties[i].getfloat = PX_Designer_GetY;
    scheckbox.properties[i].setfloat = PX_Designer_SetY;
    i++;

    PX_strcat(scheckbox.properties[i].Name, "width");
    scheckbox.properties[i].getfloat = PX_Designer_GetWidth;
    scheckbox.properties[i].setfloat = PX_Designer_SetWidth;
    i++;

    PX_strcat(scheckbox.properties[i].Name, "height");
    scheckbox.properties[i].getfloat = PX_Designer_GetHeight;
    scheckbox.properties[i].setfloat = PX_Designer_SetHeight;
    i++;

    PX_strcat(scheckbox.properties[i].Name, "text");
    scheckbox.properties[i].setstring = PX_Designer_CheckBoxSetText;
    scheckbox.properties[i].getstring = PX_Designer_CheckBoxGetText;
    i++;

    PX_strcat(scheckbox.properties[i].Name, "check");
    scheckbox.properties[i].setbool = PX_Designer_CheckBoxSetCheck;
    scheckbox.properties[i].getbool = PX_Designer_CheckBoxGetCheck;
    i++;

    return scheckbox;
}
