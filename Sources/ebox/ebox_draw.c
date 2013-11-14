/*
 * PdEnhanced - Pure Data Enhanced 
 *
 * An add-on for Pure Data
 *
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 *
 * Website  : http://www.mshparisnord.fr/HoaLibrary/
 * Contacts : cicm.mshparisnord@gmail.com
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "ebox.h"

void ebox_draw_border(t_ebox* x, t_glist* glist)
{
	float bdsize, bdcorner;
	t_elayer* g = NULL;
    bdcorner = pd_clip_max(x->e_boxparameters.d_cornersize, x->e_boxparameters.d_borderthickness - 1);
    bdsize = x->e_boxparameters.d_borderthickness;
    g = ebox_start_layer((t_object *)x, (t_object *)glist, gensym("eboxbd"), x->e_rect.width, x->e_rect.height);
    
    if(g)
    {
        if(x->e_selected_item == EITEM_OBJ)
        {
            egraphics_set_color_rgba(g, &rgba_blue);
        }
        else
        {
            egraphics_set_color_rgba(g, &x->e_boxparameters.d_bordercolor);
        }
        egraphics_set_line_width(g, bdsize*2);
        egraphics_rectangle_rounded(g, 0, 0, x->e_rect.width+bdsize*2, x->e_rect.height+bdsize*2, bdcorner);
        egraphics_stroke(g);
        
        ebox_end_layer((t_object *)x, (t_object *)glist, gensym("eboxbd"));
    }
    ebox_paint_layer((t_object *)x, (t_object *)glist, gensym("eboxbd"), -bdsize, -bdsize);
}

void ebox_draw_iolets(t_ebox* x, t_glist* glist)
{
    int i;
	float bdsize;
	t_elayer* g = NULL;
    bdsize = x->e_boxparameters.d_borderthickness;
    g = ebox_start_layer((t_object *)x, (t_object *)glist, gensym("eboxio"), x->e_rect.width, x->e_rect.height);
    
    if(g)
    {
        egraphics_set_line_width(g, 1);
        for(i = 0; i < obj_ninlets((t_object *)x); i++)
        {
            int pos_x_inlet = 0;
            if(obj_ninlets((t_object *)x) != 1)
                pos_x_inlet = (int)(i / (float)(obj_ninlets((t_object *)x) - 1) * (x->e_rect.width - 8));
            egraphics_rectangle(g, pos_x_inlet, 0, 7, 3);
            if(x->e_selected_inlet == i)
            {
                egraphics_set_color_rgba(g, &rgba_blue);
                egraphics_fill(g);
            }
            else if (obj_issignalinlet((t_object *)x, i))
            {
                egraphics_set_color_rgba(g, &rgba_inletsig);
                egraphics_fill(g);
            }
            else if(obj_isfloatinlet((t_object *)x, i))
            {
                egraphics_set_color_rgba(g, &rgba_black);
                egraphics_fill(g);
            }
            else
            {
                egraphics_set_color_rgba(g, &rgba_white);
                egraphics_fill(g);
                egraphics_set_color_rgba(g, &rgba_black);
                egraphics_stroke(g);
            }
        }
        
        for(i = 0; i < obj_noutlets((t_object *)x); i++)
        {
            int pos_x_outlet = 0;
            if(obj_noutlets((t_object *)x) != 1)
                pos_x_outlet = (int)(i / (float)(obj_noutlets((t_object *)x) - 1) * (x->e_rect.width - 8));
            egraphics_rectangle(g, pos_x_outlet, x->e_rect.height - 3 + bdsize*2, 7, 2);
            if(x->e_selected_outlet == i)
            {
                egraphics_set_color_rgba(g, &rgba_blue);
                egraphics_fill(g);
            }
            else if (obj_issignaloutlet((t_object *)x, i))
            {
                egraphics_set_color_rgba(g, &rgba_inletsig);
                egraphics_fill(g);
            }
            else if(obj_isfloatoutlet((t_object *)x, i))
            {
                egraphics_set_color_rgba(g, &rgba_black);
                egraphics_fill(g);
            }
            else
            {
                egraphics_set_color_rgba(g, &rgba_white);
                egraphics_fill(g);
                egraphics_set_color_rgba(g, &rgba_black);
                egraphics_stroke(g);
            }
        }
        ebox_end_layer((t_object *)x, (t_object *)glist, gensym("eboxio"));
    }
    ebox_paint_layer((t_object *)x, (t_object *)glist, gensym("eboxio"), 0, -bdsize);
}

void ebox_select(t_ebox* x, t_glist* glist)
{
    if(glist_isvisible(glist))
    {
        if(x->e_selected_item == EITEM_OBJ)
        {
            sys_vgui("%s itemconfigure eboxbd%ld -fill %s\n", x->e_drawing_id->s_name, x,rgba_to_hex(rgba_blue));
        }
        else
        {
            sys_vgui("%s itemconfigure eboxbd%ld -fill %s\n", x->e_drawing_id->s_name, x,rgba_to_hex(x->e_boxparameters.d_bordercolor));
        }
    }
}

void ebox_move(t_ebox* x, t_glist* glist)
{
    if(glist_isvisible(glist))
    {
        sys_vgui("%s coords %s %d %d\n", x->e_canvas_id->s_name, x->e_window_id->s_name, (int)(x->e_rect.x - x->e_boxparameters.d_borderthickness), (int)(x->e_rect.y - x->e_boxparameters.d_borderthickness));
    }
    canvas_fixlinesfor(glist_getcanvas(glist), (t_text*)x);
}

void ebox_invalidate_all(t_ebox *x, t_glist *glist)
{   
	int i;
    for(i = 0; i < x->e_number_of_layers; i++)
    {
        x->e_layers[i].e_state = EGRAPHICS_INVALID;
    }
}

void ebox_update(t_ebox *x, t_glist *glist)
{
	int i;
    if(glist_isvisible(x->e_canvas))
    {
        for(i = 0; i < x->e_number_of_layers; i++)
        {
            if(x->e_layers[i].e_state == EGRAPHICS_INVALID)
            {
                sys_vgui("%s delete %s\n", x->e_drawing_id->s_name, x->e_layers[i].e_id->s_name);
            }
        }
    }
}

void ebox_erase(t_ebox* x, t_glist* glist)
{
    if(glist_isvisible(x->e_canvas))
    {
        sys_vgui("destroy %s \n", x->e_drawing_id->s_name);
    }
    
    free(x->e_layers);
    x->e_number_of_layers = 0;
}

t_elayer* ebox_start_layer(t_object *b, t_object *view, t_symbol *name, float width, float height)
{
	int i;
    char text[256];
    t_ebox* x = (t_ebox*)b;   
    for(i = 0; i < x->e_number_of_layers; i++)
    {
        t_elayer* graphic = &x->e_layers[i];
        if(graphic->e_name == name)
        {
            if(graphic->e_state == EGRAPHICS_INVALID)
            {
                graphic->e_owner        = b;
                
                egraphics_matrix_init(&graphic->e_matrix, 1., 0., 0., 1., 0., 0.);
                graphic->e_width        = 1.f;
                graphic->e_color        = gensym("#000000");
                graphic->e_rect.x       = 0.f;
                graphic->e_rect.y       = 0.f;
                graphic->e_rect.height  = pd_clip_min(height, 0.);
                graphic->e_rect.width   = pd_clip_min(width, 0.);
                
                graphic->e_number_objects  = 0;
                if(graphic->e_new_objects.e_points)
                    free(graphic->e_new_objects.e_points);
                graphic->e_new_objects.e_points = NULL;
                graphic->e_new_objects.e_npoints = 0;
                graphic->e_new_objects.e_roundness = 0.;
                graphic->e_objects      = NULL;
                sprintf(text, "%s%ld", name->s_name, (long)x);
                graphic->e_id          = gensym(text);
                
                graphic->e_state        = EGRAPHICS_OPEN;
                return &x->e_layers[i];
            }
            else
            {
                return NULL;
            }
        }
    }
    if(x->e_layers == NULL)
        x->e_layers = (t_elayer*)calloc(1, sizeof(t_elayer));
    else
        x->e_layers = (t_elayer*)realloc(x->e_layers, (x->e_number_of_layers + 1) * sizeof(t_elayer));
    if(x->e_layers)
    {
        t_elayer* graphic = x->e_layers+x->e_number_of_layers;
        x->e_number_of_layers++;
        
        graphic->e_owner        = b;

        egraphics_matrix_init(&graphic->e_matrix, 1., 0., 0., 1., 0., 0.);
        graphic->e_width    = 1.f;
        graphic->e_color        = gensym("#000000");
        graphic->e_rect.x       = 0.f;
        graphic->e_rect.y       = 0.f;
        graphic->e_rect.height  = pd_clip_min(height, 0.);
        graphic->e_rect.width   = pd_clip_min(width, 0.);
        
        
        graphic->e_number_objects  = 0;
        graphic->e_new_objects.e_points = NULL;
        graphic->e_new_objects.e_npoints = 0;
        graphic->e_new_objects.e_roundness = 0.;
        graphic->e_objects      = NULL;
        
        graphic->e_state        = EGRAPHICS_OPEN;
        graphic->e_name         = name;
        sprintf(text, "%s%ld", name->s_name, (long)x);
        graphic->e_id          = gensym(text);
        return graphic;
    }
    else
    {
        return NULL;
    }
}

t_pd_err ebox_end_layer(t_object *b, t_object *view, t_symbol *name)
{
	int i;
    t_ebox* x = (t_ebox*)b;
    for(i = 0; i < x->e_number_of_layers; i++)
    {
        if(x->e_layers[i].e_name == name)
        {
            x->e_layers[i].e_state = EGRAPHICS_TODRAW;
            return 0;
        }
    }
    
    return 0;
}

t_pd_err ebox_invalidate_layer(t_object *b, t_object *view, t_symbol *name)
{
	int i;
    t_ebox* x = (t_ebox*)b;
    for(i = 0; i < x->e_number_of_layers; i++)
    {
        if(x->e_layers[i].e_name == name)
        {
            x->e_layers[i].e_state = EGRAPHICS_INVALID;
            return 0;
        }
    }
    return -1;
}

t_pd_err ebox_paint_layer(t_object *b, t_object *view, t_symbol *name, float x_p, float y_p)
{
	int i, j;
    float bdsize, start, extent;
    t_ebox* x = (t_ebox *)b;
    t_elayer* g = NULL;
    bdsize = x->e_boxparameters.d_borderthickness;
    sys_vgui("%s configure -bg %s\n", x->e_drawing_id->s_name, rgba_to_hex(x->e_boxparameters.d_boxfillcolor));
    sys_vgui("%s itemconfigure %s -width %d -height %d\n", x->e_canvas_id->s_name, x->e_window_id->s_name, (int)(x->e_rect.width + bdsize * 2.), (int)(x->e_rect.height + bdsize * 2.));
    canvas_fixlinesfor(x->e_canvas, (t_text *)x);
    for(i = 0; i < x->e_number_of_layers; i++)
    {
        if(x->e_layers[i].e_name == name)
        {
            g = &x->e_layers[i];
            if(g->e_state != EGRAPHICS_TODRAW)
            {
                return -1;
            }
        }
    }
    if(g)
    {
        
        for(i = 0; i < g->e_number_objects; i++)
        {
            t_egobj* gobj = g->e_objects+i;
            ////////////// PATH, LINE AND RECT ///////////////////////////
            if(gobj->e_type == E_GOBJ_PATH || gobj->e_type == E_GOBJ_RECT)
            {
                if(gobj->e_filled)
                    sys_vgui("%s create polygon ", x->e_drawing_id->s_name);
                else
                    sys_vgui("%s create line ", x->e_drawing_id->s_name);
                
                for(j = 0; j < gobj->e_npoints; j ++)
                    sys_vgui("%d %d ", (int)(gobj->e_points[j].x + x_p + bdsize), (int)(gobj->e_points[j].y + y_p + bdsize));
                
                if(gobj->e_filled)
                    sys_vgui("-fill %s -width 0 -tags { %s %s }\n", gobj->e_color->s_name,  g->e_id->s_name, x->e_all_id->s_name);
                else
                    sys_vgui("-fill %s -width %f -tags { %s %s }\n", gobj->e_color->s_name, gobj->e_width, g->e_id->s_name, x->e_all_id->s_name);
                
                g->e_state = EGRAPHICS_CLOSE;
            }
            ////////////// OVAL /////////////////
            else if (gobj->e_type == E_GOBJ_OVAL)
            {
                sys_vgui("%s create oval %d %d %d %d ",
                        x->e_drawing_id->s_name,
                        (int)(gobj->e_points[0].x + x_p + bdsize),
                        (int)(gobj->e_points[0].y + y_p + bdsize),
                        (int)(gobj->e_points[1].x + x_p + bdsize),
                        (int)(gobj->e_points[1].y + y_p + bdsize));
                if(gobj->e_filled)
                     sys_vgui("-fill %s -width 0 -tags { %s %s }\n", gobj->e_color->s_name,  g->e_id->s_name, x->e_all_id->s_name);
                else
                    sys_vgui("-outline %s -width %f -tags { %s %s }\n", gobj->e_color->s_name, gobj->e_width, g->e_id->s_name, x->e_all_id->s_name);
                
                g->e_state = EGRAPHICS_CLOSE;
            }
            ////////////// ARC /////////////////
            else if (gobj->e_type == E_GOBJ_ARC)
            {
                start = pd_angle(gobj->e_points[2].x,  gobj->e_points[2].y);
                extent = pd_angle(gobj->e_points[3].x,  gobj->e_points[3].y);
                sys_vgui("%s create arc %d %d %d %d -start %f -extent %f ",
                         x->e_drawing_id->s_name,
                         (int)(gobj->e_points[0].x + x_p + bdsize),
                         (int)(gobj->e_points[0].y + y_p + bdsize),
                         (int)(gobj->e_points[1].x + x_p + bdsize),
                         (int)(gobj->e_points[1].y + y_p + bdsize),
                         (float)start / EPD_2PI * 360.,
                         (float)extent / EPD_2PI * 360.);
                
                if(gobj->e_filled)
                    sys_vgui("-style pieslice -fill %s -width 0 -tags { %s %s }\n", gobj->e_color->s_name,  g->e_id->s_name, x->e_all_id->s_name);
                else
                    sys_vgui("-style arc -outline %s -width %f -tags { %s %s }\n", gobj->e_color->s_name, gobj->e_width, g->e_id->s_name, x->e_all_id->s_name);

                g->e_state = EGRAPHICS_CLOSE;
            }
            ////////////// TEXT ////////////////
            else if(gobj->e_type == E_GOBJ_TEXT)
            {
                
                sys_vgui("%s create text %d %d -text {%s} -anchor %s -font {%s %d %s} -fill %s -width %d -tags { %s %s }\n",
                         x->e_drawing_id->s_name,
                         (int)(gobj->e_points[0].x + x_p + bdsize),
                         (int)(gobj->e_points[0].y + y_p + bdsize),
                         gobj->e_text->s_name,
                         gobj->e_justify->s_name,
                         gobj->e_font.c_family->s_name, (int)gobj->e_font.c_size, gobj->e_font.c_weight->s_name,
                         gobj->e_color->s_name,
                         (int)(gobj->e_points[1].x),
                         g->e_id->s_name,
                         x->e_all_id->s_name);
                
                g->e_state = EGRAPHICS_CLOSE;
            }
            else
            {
                error("Invalid layer object %s : %i", x->e_layers[i].e_name->s_name, i);
                return -1;
            }
        }
    }
    else
    {
        error("Invalid layer name %s", name->s_name);
        return -1;
    }
    
    
    return 0;
}


