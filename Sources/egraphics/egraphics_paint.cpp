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

#include "egraphics.h"

void cicm_graphics_line_to(t_egraphics *g, double x, double y)
{
    if(g->c_state == CICM_GRAPHICS_OPEN)
    {
        g->c_new_obj_coords.push_back(x);
        g->c_new_obj_coords.push_back(y);
    }
}

void cicm_graphics_move_to(t_egraphics *g, double x, double y)
{
    if(g->c_state == CICM_GRAPHICS_OPEN)
    {
        g->c_new_obj_type.assign("polygon ");
        
        g->c_new_obj_coords.clear();
        g->c_new_obj_coords.push_back(x);
        g->c_new_obj_coords.push_back(y);
        
        g->c_new_obj_options.clear();
    }
}

void cicm_graphics_rectangle(t_egraphics *g, double x, double y, double width, double height)
{
    if(g->c_state == CICM_GRAPHICS_OPEN)
    {        
        g->c_new_obj_type.assign("rectangle ");
        
        g->c_new_obj_coords.clear();
        g->c_new_obj_coords.push_back(x);
        g->c_new_obj_coords.push_back(y);
        g->c_new_obj_coords.push_back(x + width);
        g->c_new_obj_coords.push_back(y + height);
        
        g->c_new_obj_options.clear();
    }
}

void cicm_graphics_arc(t_egraphics *g, double xc, double yc, double radius, double angle1, double angle2)
{
    if(g->c_state == CICM_GRAPHICS_OPEN)
    {
        
        g->c_new_obj_coords.clear();
        g->c_new_obj_options.clear();
        g->c_new_obj_coords.push_back(xc - radius);
        g->c_new_obj_coords.push_back(yc - radius);
        g->c_new_obj_coords.push_back(xc + radius);
        g->c_new_obj_coords.push_back(yc + radius);
        
        int start = (angle1 + g->c_rotation) * 360. / EPD_2PI;
        int extent = (angle2 - angle1)  * 360. / EPD_2PI;
        
        if(extent % 360 == 0)
        {
            g->c_new_obj_type.assign("oval ");            
        }
        else
        {
            char text[256];
            g->c_new_obj_type.assign("arc ");
            sprintf(text, "-start %d -extent %d -style arc ", (int)(start), (int)(extent));
            g->c_new_obj_options.assign(text);

        }
    }
}

void cicm_graphics_arc_negative(t_egraphics *g, double xc, double yc, double radius, double angle1, double angle2)
{
    if(g->c_state == CICM_GRAPHICS_OPEN)
    {
        g->c_new_obj_coords.clear();
        g->c_new_obj_options.clear();
        g->c_new_obj_coords.push_back(xc - radius);
        g->c_new_obj_coords.push_back(yc - radius);
        g->c_new_obj_coords.push_back(xc + radius);
        g->c_new_obj_coords.push_back(yc + radius);
        
        int start = (angle1 + g->c_rotation) / EPD_2PI * 360.;
        int extent = -(angle2 + g->c_rotation - angle1) / EPD_2PI * 360.;
        
        if(extent % 360 == 0)
        {
            g->c_new_obj_type.assign("oval ");
        }
        else
        {
            char text[256];
            g->c_new_obj_type.assign("arc ");
            sprintf(text, "-start %d -extent %d -style arc ", (int)(start), (int)(extent));
            g->c_new_obj_options.assign(text);
            
        }
    }
}

