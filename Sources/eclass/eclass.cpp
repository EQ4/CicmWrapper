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

#include "eclass.h"

static int puclicite = 0;

t_eclass* eclass_new(char *name, method newmethod, method freemethod, size_t size, int flags, t_atomtype arg1, int arg2)
{
    t_class *pd  = class_new(gensym(name), (t_newmethod)newmethod, (t_method)freemethod, size, flags, arg1, arg2);
    t_eclass* c;
    c = (t_eclass *)resizebytes(pd, sizeof(*pd), sizeof(*c));

#ifdef NO_PUB
    puclicite = 1;
#endif
    if(!puclicite)
    {
        post(".________________________________________________.");
        post("|                                                |");
        post("|                  Pd Enhanced                   |");
        post("| Pure Data Enhanced : Version 0.1 at sept. 2013 |");
        post("|  Organization : Université Paris 8 | CICM      |");
        post("|          Author : Pierre Guillot               |");
        post("|________________________________________________|");
        puclicite = 1;
    }
    return c;
}

void eclass_init(t_eclass* c, long flags)
{
    ewidget_init(c);
    class_setsavefn((t_class *)c, ewidget_save);
    class_setpropertiesfn((t_class *)c, (t_propertiesfn)ebox_properties);
    class_setwidget((t_class *)c, (t_widgetbehavior *)&c->c_widget);
    
    CLASS_ATTR_DOUBLE_ARRAY (c, "patching_rect", 0, t_ebox, e_rect, 4);
    CLASS_ATTR_DEFAULT      (c, "patching_rect", 0, "0 0 200 200");
    CLASS_ATTR_FILTER_MIN   (c, "patching_rect", 0);
    CLASS_ATTR_SAVE         (c, "patching_rect", 0);
    CLASS_ATTR_PAINT        (c, "patching_rect", 0);
    CLASS_ATTR_CATEGORY		(c, "patching_rect", 0, "Basic");
    CLASS_ATTR_LABEL		(c, "patching_rect", 0, "Patching rectangle");
    
}

void eclass_dspinit(t_eclass* c)
{
    CLASS_MAINSIGNALIN((t_class *)c, t_ebox, e_float);
    class_addmethod((t_class *)c, (t_method)ebox_dsp, gensym("dsp"), A_CANT, 0);
    class_addmethod((t_class *)c, (t_method)ebox_dsp_add, gensym("dsp_add"), A_NULL, 0);
    class_addmethod((t_class *)c, (t_method)ebox_dsp_add, gensym("dsp_add64"), A_NULL, 0);
}




