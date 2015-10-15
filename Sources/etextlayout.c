/*
 * CicmWrapper
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 * Website  : https://github.com/CICM/CicmWrapper
 * Contacts : cicm.mshparisnord@gmail.com
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#include "etextlayout.h"

struct _etextlayout
{
    char*           c_text;     /*!< The text. */
    char            c_wrap;     /*!< If the text should be wrapped. */
    t_rgba          c_color;    /*!< The color of the text. */
    t_efont         c_font;     /*!< The font of the text. */
    t_rect          c_rect;     /*!< The rectangle of the text. */
    int             c_justify;  /*!< The justification of the graphical object. */
};

t_etextlayout* etextlayout_new(void)
{
    t_etextlayout* txt = (t_etextlayout *)malloc(sizeof(t_etextlayout));
    if(txt)
    {
        txt->c_text = (char *)malloc(MAXPDSTRING * sizeof(char));
        if(txt->c_text)
        {
            memset(txt->c_text, 0, MAXPDSTRING * sizeof(char));
            txt->c_color.red    = 0.;
            txt->c_color.green  = 0.;
            txt->c_color.blue   = 0.;
            txt->c_color.alpha  = 1.;
            return txt;
        }
        else
        {
            free(txt);
            return NULL;
        }
    }
    return NULL;
}

void etextlayout_destroy(t_etextlayout* textlayout)
{
    free(textlayout->c_text);
    free(textlayout);
}

void etextlayout_set(t_etextlayout* textlayout, const char* text, t_efont *font,  float x, float y, float width,  float height, etextjustify_flags justify, etextwrap_flags wrap)
{
    strncpy(textlayout->c_text, text, MAXPDSTRING);
    textlayout->c_font          = font[0];
    textlayout->c_rect.x        = (float)x;
    textlayout->c_rect.y        = (float)y;
    textlayout->c_rect.width    = (float)width;
    textlayout->c_rect.height   = (float)height;
    textlayout->c_wrap          = wrap;
    textlayout->c_justify       = justify;
}

void etextlayout_settextcolor(t_etextlayout* textlayout, t_rgba const* color)
{
    textlayout->c_color = color[0];
}

