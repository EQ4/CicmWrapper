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

void *ebox_alloc(t_eclass *c)
{
    t_pd *x;
    if (!c)
        bug ("pd_new: apparently called before setup routine");
    x = (t_pd *)t_getbytes(c->c_class.c_size);
    *x = (t_pd)c;
    if (c->c_class.c_patchable)
    {
        ((t_object *)x)->ob_inlet = 0;
        ((t_object *)x)->ob_outlet = 0;
    }
    return (x);
}

void ebox_new(t_ebox *x, long flags, long argc, t_atom *argv)
{
    x->e_ready_to_draw = 0;
    x->e_glist = (t_glist *)canvas_getcurrent();
    x->e_classname = class_getname(x->e_obj.te_g.g_pd);
    if (flags == 1)
    {
        x->e_no_redraw_box = 1;
    }
    
    // A METTRE EN ATTR //
    x->e_font.c_family = gensym(sys_font);
    x->e_font.c_weight = gensym(sys_fontweight);
    x->e_font.c_slant = gensym("regular");
    x->e_font.c_size = sys_nearestfontsize(sys_defaultfont);
}

void ebox_dspsetup(t_ebox *x, long nins, long nout)
{
    nins = pd_clip_min(nins, 1);
    nout = pd_clip_min(nout, 0);
    for (int i = 1; i < nins; i++)
    {
        x->e_inlets.push_back(signalinlet_new(&x->e_obj, x->e_float));
    }
    for (int i = 0; i < nout; i++)
    {
        x->e_outlets.push_back(outlet_new(&x->e_obj, &s_signal));
        
    }
    x->e_dsp_vectors = NULL;
    x->e_nins   = obj_ninlets(&x->e_obj);
    x->e_nouts  = obj_noutlets(&x->e_obj);
    x->e_dsp_size      = x->e_nins + x->e_nouts + 5;
    x->e_dsp_vectors   = (t_int*)calloc(x->e_dsp_size , sizeof(t_int));
}


void ebox_ready(t_ebox *x)
{
    t_eclass* c = (t_eclass *)x->e_obj.te_g.g_pd;
    x->e_mouse_down = 0;
    c->c_widget.w_getdrawparameters(x, NULL, &x->e_boxparameters);
    x->e_ready_to_draw = 1;
    x->e_nins   = obj_ninlets(&x->e_obj);
    x->e_nouts  = obj_noutlets(&x->e_obj);
}

void ebox_dspfree(t_ebox *x)
{
    if(x->e_dsp_vectors != NULL)
    {
        free(x->e_dsp_vectors);
        x->e_dsp_vectors = NULL;
        x->e_dsp_size = 0;
    }
    for(int k = x->e_inlets.size(); k >= 1; k--)
    {
        canvas_deletelinesforio(x->e_glist, (t_text *)x, x->e_inlets[k-1], NULL);
        inlet_free(x->e_inlets[k-1]);
        x->e_inlets.pop_back();
    }
    for(int k = x->e_outlets.size(); k >= 1; k--)
    {
        canvas_deletelinesforio(x->e_glist, (t_text *)x, NULL, x->e_outlets[k-1]);
        outlet_free(x->e_outlets[k-1]);
        x->e_outlets.pop_back();
    }
    
    ebox_free(x);
}

void ebox_free(t_ebox* x)
{
     gfxstub_deleteforkey(x);
}

void ebox_redraw(t_ebox *x)
{
    if(x->e_ready_to_draw)
    {
        ewidget_paint(x, x->e_glist, 0);
    }
}


void ebox_resize_inputs(t_ebox *x, long nins)
{
    nins = pd_clip_min(nins, long(1));
    
    if(nins > x->e_nins)
    {
        for (int i = x->e_nins; i < nins; i++)
        {
            x->e_inlets.push_back(signalinlet_new(&x->e_obj, x->e_float));
        }
    }
    else if (nins < x->e_nins)
    {
        for(int k = x->e_inlets.size(); k >= nins; k--)
        {
            canvas_deletelinesforio(x->e_glist, (t_text *)x, x->e_inlets[k-1], NULL);
            inlet_free(x->e_inlets[k-1]);
            x->e_inlets.pop_back();
        }
    }
    
    x->e_nins = obj_ninlets(&x->e_obj);
    
    if (x->e_dsp_vectors != NULL)
    {
        free(x->e_dsp_vectors);
        x->e_dsp_vectors = NULL;
        x->e_dsp_size = 0;
    }
    x->e_dsp_size      = x->e_nins + x->e_nouts + 5;
    x->e_dsp_vectors   = (t_int*)calloc(x->e_dsp_size , sizeof(t_int));
    
    ewidget_vis((t_gobj *)x, x->e_glist, 1);
}

void ebox_dsp(t_ebox *x, t_signal **sp, short *count)
{
    t_eclass *c         = (t_eclass *)x->e_obj.te_g.g_pd;
    
    c->c_widget.w_dsp(x, x, count, sp[0]->s_sr, sp[0]->s_n, 0);
    
    x->e_dsp_vectors[0] = (t_int)x;
    x->e_dsp_vectors[1] = (t_int)c;
    x->e_dsp_vectors[2] = (t_int)sp[0]->s_n;
    x->e_dsp_vectors[3] = (t_int)x->e_dsp_flag;
    x->e_dsp_vectors[4] = (t_int)x->e_dsp_user_param;
    for (int i = 5; i < x->e_dsp_size; i++)
    {
        x->e_dsp_vectors[i] = (t_int)(sp[i - 5]->s_vec);
    }
    
    dsp_addv(ebox_perform, x->e_dsp_size, x->e_dsp_vectors);
}

t_int* ebox_perform(t_int* w)
{
    t_ebox* x               = (t_ebox *)(w[1]);
    t_eclass* c             = (t_eclass *)(w[2]);
    
    c->c_widget.w_perform(x, NULL, (t_float **)(&w[6]), x->e_nins, (t_float **)(&w[6 + x->e_nins]), x->e_nouts, (long)(w[3]), (long)(w[4]), (void *)(w[5]));
    
    return w + (x->e_dsp_size + 1);
}


void ebox_dsp_add(t_ebox *x, t_symbol* s, t_object* obj, method m, long flags, void *userparam)
{
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;
   
    x->e_dsp_flag = flags;
    x->e_dsp_user_param = userparam;
    c->c_widget.w_perform = m;
}

void ebox_get_rect_for_view(t_object *z, t_object *patcherview, t_rect *rect)
{
    t_ebox* x = (t_ebox *)z;
    rect->x = x->e_obj.te_xpix;
    rect->y = x->e_obj.te_ypix;
    rect->width = x->e_rect.width;
    rect->height = x->e_rect.height;
}

void eclass_addmethod(t_eclass* c, method m, char* name, t_atomtype type, long anything)
{
    if(gensym(name) == gensym("mousemove"))
    {
        c->c_widget.w_mousemove = m;
    }
    else if(gensym(name) == gensym("mousedown"))
    {
        c->c_widget.w_mousedown = m;
    }
    else if(gensym(name) == gensym("mousedrag"))
    {
        c->c_widget.w_mousedrag = m;
    }
    else if(gensym(name) == gensym("mouseup"))
    {
        c->c_widget.w_mouseup = m;
    }
    else if(gensym(name) == gensym("paint"))
    {
        c->c_widget.w_paint = m;
    }
    else if(gensym(name) == gensym("assist"))
    {
        ;
    }
    else if(gensym(name) == gensym("notify"))
    {
        c->c_widget.w_notify = (t_err_method)m;
    }
    else if(gensym(name) == gensym("anything"))
    {
        class_addanything((t_class *)c, m);
    }
    else if(gensym(name) == gensym("getdrawparams"))
    {
        c->c_widget.w_getdrawparameters = m;
    }
    else if(gensym(name) == gensym("bang"))
    {
        class_addbang((t_class *)c, m);
    }
    else if(gensym(name) == gensym("save") || gensym(name) == gensym("jsave"))
    {
        c->c_widget.w_save = m;
    }
    else if(gensym(name) == gensym("popup"))
    {
        c->c_widget.w_popup = m;
    }
    else if(gensym(name) == gensym("dsp") || gensym(name) == gensym("dsp64"))
    {
        c->c_widget.w_dsp = m;
    }
    else
    {
        class_addmethod((t_class *)c, (t_method)m, gensym(name), type, anything);
    }
}

void ebox_properties(t_gobj *z, t_glist *glist)
{
    ;
}

t_pd_err ebox_notify(t_ebox *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    if(msg == gensym("patching_rect"))
    {
        for(long i = 0; i < x->e_graphics.size(); i++)
        {
            x->e_graphics[i].c_state = CICM_GRAPHICS_INVALID;
        }
        ewidget_vis((t_gobj *)x, x->e_glist, 1);
    }
    return 0;
}

t_pd_err ebox_end_layer(t_object *b, t_object *view, t_symbol *name)
{
    t_ebox* x = (t_ebox*)b;
    for(long i = 0; i < x->e_graphics.size(); i++)
    {
        if(x->e_graphics[i].c_name == name)
        {
            x->e_graphics[i].c_state = CICM_GRAPHICS_TO_DRAW;
            x->e_graphics[i].c_new_obj_type.clear();
            x->e_graphics[i].c_new_obj_coords.clear();
            x->e_graphics[i].c_new_obj_options.clear();
            return 0;
        }
    }
    
    return 0;
}

t_pd_err ebox_invalidate_layer(t_object *b, t_object *view, t_symbol *name)
{
    t_ebox* x = (t_ebox*)b;
    for(long i = 0; i < x->e_graphics.size(); i++)
    {
        if(x->e_graphics[i].c_name == name)
        {
            x->e_graphics[i].c_state = CICM_GRAPHICS_INVALID;
            return 0;
        }
    }
    return -1;
}

t_pd_err ebox_paint_layer(t_object *b, t_object *view, t_symbol *name, double x, double y)
{
    t_ebox* obj = (t_ebox *)b;
    t_canvas* canvas = glist_getcanvas((t_glist *)view);
    t_egraphics* g = NULL;
    for(int i = 0; i < obj->e_graphics.size(); i++)
    {
        if(obj->e_graphics[i].c_name == name && obj->e_graphics[i].c_canvas == canvas && obj->e_graphics[i].c_state == CICM_GRAPHICS_TO_DRAW)
        {
            g = &obj->e_graphics[i];
            g->c_offset_x = x;
            g->c_offset_y = y;
        }
    }
    if(g)
    {
        char temp[256];
        std::string text;
        for(int i = 0; i < g->c_obj_types.size(); i++)
        {
            text.assign(g->c_canvas_text);
            text.append(" create ");
            text.append(g->c_obj_types[i]);
            
            for(int j = 0; j < g->c_obj_coords[i].size(); j += 2)
            {
                sprintf(temp, "%d %d ", (int)(g->c_obj_coords[i][j] + g->c_offset_x + obj->e_obj.te_xpix), (int)(g->c_obj_coords[i][j+1] + g->c_offset_y + obj->e_obj.te_ypix));
                text.append(temp);
            }
            text.append(g->c_obj_options[i]);
            sys_gui((char *)text.c_str());
            g->c_state = CICM_GRAPHICS_CLOSE;
        }
    }
    else
        return -1;
    
    return 0;
}

t_binbuf* object_cicm_dictionaryarg(long ac, t_atom *av)
{
    t_binbuf* dico = binbuf_new();
    binbuf_add(dico, ac, av);
    return dico;
}

void attr_cicm_dictionary_process(void *x, t_binbuf *d)
{
    long defc = 0;
    t_atom* defv = NULL;
    t_ebox* z = (t_ebox *)x;
    t_eclass* c = (t_eclass *)z->e_obj.te_g.g_pd;
    // DEFAULT c->c_attr VALUES //
    for(int i = 0; i < c->c_attr.size(); i++)
    {
        if(c->c_attr[i].defvals)
        {
            defc = c->c_attr[i].size;
            defv = new t_atom[defc];
            if(defc && defv)
            {
                char* str_start = c->c_attr[i].defvals->s_name;
                for(int j = 0; j < defc; j++)
                {
                    double val = (double)strtod(str_start, &str_start);
                    atom_setfloat(defv+j, (float)val);
                }
                object_attr_setvalueof((t_object *)x, c->c_attr[i].name, defc, defv);
                defc = 0;
                free(defv);
                defv = NULL;
            }
        }
    }
    char attr_name[256];
    for(int i = 0; i < c->c_attr.size(); i++)
    {
        sprintf(attr_name, "@%s", c->c_attr[i].name->s_name);
        dictionary_copyatoms(d, gensym(attr_name), &defc, &defv);
        if(defc && defv)
        {
            object_attr_setvalueof((t_object *)x, c->c_attr[i].name, defc, defv);
            defc = 0;
            free(defv);
            defv = NULL;
        }
    }
}

t_symbol* cicm_obj_get_fontname(t_ebox* x)
{
    return x->e_font.c_family;
}

t_symbol* cicm_obj_font_slant(t_ebox* x)
{
    return x->e_font.c_slant;
}

t_symbol* cicm_obj_font_weight(t_ebox* x)
{
    return x->e_font.c_weight;
}

double cicm_obj_fontsize(t_ebox* x)
{
    return x->e_font.c_size;
}
