/*
 * Copyright (C) 2010 - Jeff Johnston
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __CODESLAYER_APPLICATION_H__
#define __CODESLAYER_APPLICATION_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CODESLAYER_APPLICATION_TYPE            (codeslayer_application_get_type ())
#define CODESLAYER_APPLICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CODESLAYER_APPLICATION_TYPE, CodeSlayerApplication))
#define CODESLAYER_APPLICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CODESLAYER_APPLICATION_TYPE, CodeSlayerApplicationClass))
#define IS_CODESLAYER_APPLICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CODESLAYER_APPLICATION_TYPE))
#define IS_CODESLAYER_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CODESLAYER_APPLICATION_TYPE))

typedef struct _CodeSlayerApplication CodeSlayerApplication;
typedef struct _CodeSlayerApplicationClass CodeSlayerApplicationClass;

struct _CodeSlayerApplication
{
  GtkApplication parent_instance;
};

struct _CodeSlayerApplicationClass
{
  GtkApplicationClass parent_class;
};

GType codeslayer_application_get_type (void) G_GNUC_CONST;

CodeSlayerApplication*  codeslayer_application_new  ();

G_END_DECLS

#endif /* __CODESLAYER_APPLICATION_H__ */
