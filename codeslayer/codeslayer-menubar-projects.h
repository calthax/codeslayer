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

#ifndef __CODESLAYER_MENUBAR_PROJECTS_H__
#define	__CODESLAYER_MENUBAR_PROJECTS_H__

#include <gtk/gtk.h>
#include <codeslayer/codeslayer-settings.h>

G_BEGIN_DECLS

#define CODESLAYER_MENUBAR_PROJECTS_TYPE            (codeslayer_menubar_projects_get_type ())
#define CODESLAYER_MENUBAR_PROJECTS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CODESLAYER_MENUBAR_PROJECTS_TYPE, CodeSlayerMenuBarProjects))
#define CODESLAYER_MENUBAR_PROJECTS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CODESLAYER_MENUBAR_PROJECTS_TYPE, CodeSlayerMenuBarProjectsClass))
#define IS_CODESLAYER_MENUBAR_PROJECTS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CODESLAYER_MENUBAR_PROJECTS_TYPE))
#define IS_CODESLAYER_MENUBAR_PROJECTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CODESLAYER_MENUBAR_PROJECTS_TYPE))

typedef struct _CodeSlayerMenuBarProjects CodeSlayerMenuBarProjects;
typedef struct _CodeSlayerMenuBarProjectsClass CodeSlayerMenuBarProjectsClass;

struct _CodeSlayerMenuBarProjects
{
  GtkMenuItem parent_instance;
};

struct _CodeSlayerMenuBarProjectsClass
{
  GtkMenuItemClass parent_class;
};

GType codeslayer_menubar_projects_get_type (void) G_GNUC_CONST;
  
GtkWidget*  codeslayer_menubar_projects_new  (GtkWidget          *window, 
                                              GtkWidget          *menubar, 
                                              GtkAccelGroup      *accel_group, 
                                              CodeSlayerSettings *settings);

G_END_DECLS

#endif /* __CODESLAYER_MENUBAR_PROJECTS_H__ */
