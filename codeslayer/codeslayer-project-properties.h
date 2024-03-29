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

#ifndef __CODESLAYER_PROJECT_PROPERTIES_H__
#define	__CODESLAYER_PROJECT_PROPERTIES_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CODESLAYER_PROJECT_PROPERTIES_TYPE            (codeslayer_project_properties_get_type ())
#define CODESLAYER_PROJECT_PROPERTIES(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CODESLAYER_PROJECT_PROPERTIES_TYPE, CodeSlayerProjectProperties))
#define CODESLAYER_PROJECT_PROPERTIES_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CODESLAYER_PROJECT_PROPERTIES_TYPE, CodeSlayerProjectPropertiesClass))
#define IS_CODESLAYER_PROJECT_PROPERTIES(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CODESLAYER_PROJECT_PROPERTIES_TYPE))
#define IS_CODESLAYER_PROJECT_PROPERTIES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CODESLAYER_PROJECT_PROPERTIES_TYPE))

typedef struct _CodeSlayerProjectProperties CodeSlayerProjectProperties;
typedef struct _CodeSlayerProjectPropertiesClass CodeSlayerProjectPropertiesClass;

struct _CodeSlayerProjectProperties
{
  GtkVBox parent_instance;
};

struct _CodeSlayerProjectPropertiesClass
{
  GtkVBoxClass parent_class;
};

GType codeslayer_project_properties_get_type (void) G_GNUC_CONST;

GtkWidget*  codeslayer_project_properties_new     (void);

void        codeslayer_project_properties_add     (CodeSlayerProjectProperties *project_properties, 
                                                   GtkWidget                   *widget,
                                                   const gchar                 *title);

void        codeslayer_project_properties_remove  (CodeSlayerProjectProperties *project_properties, 
                                                   GtkWidget                   *widget);

G_END_DECLS

#endif /* __CODESLAYER_PROJECT_PROPERTIES_H__ */
