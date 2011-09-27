/*
 * Copyright (C) 2010 - Jeff Johnston
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.remove_group_item
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <gdk/gdkkeysyms.h>
#include <codeslayer/codeslayer-menubar.h>
#include <codeslayer/codeslayer-menubar-view.h>
#include <codeslayer/codeslayer-utils.h>

/**
 * SECTION:codeslayer-menubar-view
 * @short_description: The view menu.
 * @title: CodeSlayerMenuBarView
 * @include: codeslayer/codeslayer-menubar-view.h
 */

static void codeslayer_menubar_view_class_init  (CodeSlayerMenuBarViewClass *klass);
static void codeslayer_menubar_view_init        (CodeSlayerMenuBarView      *menubar_view);
static void codeslayer_menubar_view_finalize    (CodeSlayerMenuBarView      *menubar_view);

static void add_menu_items                      (CodeSlayerMenuBarView      *menubar_view);

static void fullscreen_window_action            (CodeSlayerMenuBarView      *menubar_view);
static void show_side_pane_action               (CodeSlayerMenuBarView      *menubar_view);
static void show_bottom_pane_action             (CodeSlayerMenuBarView      *menubar_view);
static void draw_spaces_action                  (CodeSlayerMenuBarView      *menubar_view);

#define CODESLAYER_MENUBAR_VIEW_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CODESLAYER_MENUBAR_VIEW_TYPE, CodeSlayerMenuBarViewPrivate))

typedef struct _CodeSlayerMenuBarViewPrivate CodeSlayerMenuBarViewPrivate;

struct _CodeSlayerMenuBarViewPrivate
{
  GtkAccelGroup *accel_group;
  GtkWidget     *menubar;
  GtkWidget     *menu;
  GtkWidget     *fullscreen_window_item;
  GtkWidget     *draw_spaces_item;
  GtkWidget     *show_side_pane_item;
  GtkWidget     *show_bottom_pane_item;
  gulong         show_side_pane_id;
  gulong         show_bottom_pane_id;
};

G_DEFINE_TYPE (CodeSlayerMenuBarView, codeslayer_menubar_view, GTK_TYPE_MENU_ITEM)

static void
codeslayer_menubar_view_class_init (CodeSlayerMenuBarViewClass *klass)
{
  G_OBJECT_CLASS (klass)->finalize = (GObjectFinalizeFunc) codeslayer_menubar_view_finalize;
  g_type_class_add_private (klass, sizeof (CodeSlayerMenuBarViewPrivate));
}

static void
codeslayer_menubar_view_init (CodeSlayerMenuBarView *menubar_view)
{
  CodeSlayerMenuBarViewPrivate *priv;
  GtkWidget *menu;

  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);
  
  gtk_menu_item_set_label (GTK_MENU_ITEM (menubar_view), _("View"));
  
  menu = gtk_menu_new ();
  priv->menu = menu;
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menubar_view), menu);
}

static void
codeslayer_menubar_view_finalize (CodeSlayerMenuBarView *menubar_view)
{
  G_OBJECT_CLASS (codeslayer_menubar_view_parent_class)->finalize (G_OBJECT (menubar_view));
}

/**
 * codeslayer_menubar_view_new:
 * @menubar: a #CodeSlayerMenuBar.
 * @accel_group: a #GtkAccelGroup.
 *
 * Creates a new #CodeSlayerMenuBarView.
 *
 * Returns: a new #CodeSlayerMenuBarView. 
 */
GtkWidget*
codeslayer_menubar_view_new (GtkWidget     *menubar, 
                             GtkAccelGroup *accel_group)
{
  CodeSlayerMenuBarViewPrivate *priv;
  GtkWidget *menubar_view;
  
  menubar_view = g_object_new (codeslayer_menubar_view_get_type (), NULL);
  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);

  priv->menubar = menubar;
  priv->accel_group = accel_group;

  add_menu_items (CODESLAYER_MENUBAR_VIEW (menubar_view));

  return menubar_view;
}

static void
add_menu_items (CodeSlayerMenuBarView *menubar_view)
{
  CodeSlayerMenuBarViewPrivate *priv;
  GtkWidget *fullscreen_window_item;
  GtkWidget *show_side_pane_item;
  GtkWidget *show_bottom_pane_item;
  GtkWidget *draw_spaces_item;
  
  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);

  fullscreen_window_item = gtk_check_menu_item_new_with_label (_("Full Screen"));
  priv->fullscreen_window_item = fullscreen_window_item;
  gtk_widget_add_accelerator (fullscreen_window_item, "activate", 
                              priv->accel_group, GDK_KEY_F11, 0, GTK_ACCEL_VISIBLE);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), fullscreen_window_item);

  show_bottom_pane_item = gtk_check_menu_item_new_with_label (_("Bottom Pane"));
  priv->show_bottom_pane_item = show_bottom_pane_item;
  gtk_widget_add_accelerator (show_bottom_pane_item, "activate", 
                              priv->accel_group, GDK_KEY_F12, 0, GTK_ACCEL_VISIBLE);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), show_bottom_pane_item);

  show_side_pane_item = gtk_check_menu_item_new_with_label (_("Side Pane"));
  priv->show_side_pane_item = show_side_pane_item;
  gtk_widget_add_accelerator (show_side_pane_item, "activate", 
                              priv->accel_group, GDK_KEY_F12, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), show_side_pane_item);

  draw_spaces_item = gtk_check_menu_item_new_with_label (_("Draw Spaces"));
  priv->draw_spaces_item = draw_spaces_item;
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), draw_spaces_item);

  g_signal_connect_swapped (G_OBJECT (fullscreen_window_item), "activate",
                            G_CALLBACK (fullscreen_window_action), menubar_view);

  priv->show_side_pane_id = g_signal_connect_swapped (G_OBJECT (show_side_pane_item), "activate",
                                                      G_CALLBACK (show_side_pane_action), menubar_view);

  priv->show_bottom_pane_id = g_signal_connect_swapped (G_OBJECT (show_bottom_pane_item), "activate",
                                                        G_CALLBACK (show_bottom_pane_action), menubar_view);

  g_signal_connect_swapped (G_OBJECT (draw_spaces_item), "activate",
                            G_CALLBACK (draw_spaces_action), menubar_view);
}

/**
 * codeslayer_menubar_view_sync_with_panes:
 * @menubar_view: a #CodeSlayerMenuBarView.
 * @show_side_pane: if TRUE then the side pane is shown
 * @show_bottom_pane: if TRUE then the bottom pane is shown
 * 
 * Update the sensitivity of view related menu items based on the current 
 * state of the bottom and side pane.
 */
void
codeslayer_menubar_view_sync_with_panes (CodeSlayerMenuBarView *menubar_view, 
                                         gboolean               show_side_pane, 
                                         gboolean               show_bottom_pane)
{
  CodeSlayerMenuBarViewPrivate *priv;
  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);
  
  g_signal_handler_block (priv->show_side_pane_item, priv->show_side_pane_id);
  g_signal_handler_block (priv->show_bottom_pane_item, priv->show_bottom_pane_id);
  
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (priv->show_side_pane_item),
                                  show_side_pane);

  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (priv->show_bottom_pane_item),
                                  show_bottom_pane);

  g_signal_handler_unblock (priv->show_side_pane_item, priv->show_side_pane_id);
  g_signal_handler_unblock (priv->show_bottom_pane_item, priv->show_bottom_pane_id);
}                                        

static void
fullscreen_window_action (CodeSlayerMenuBarView *menubar_view)
{
  CodeSlayerMenuBarViewPrivate *priv;
  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);
  codeslayer_menubar_fullscreen_window (CODESLAYER_MENUBAR (priv->menubar));
}

static void
show_side_pane_action (CodeSlayerMenuBarView *menubar_view)
{
  CodeSlayerMenuBarViewPrivate *priv;
  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);
  codeslayer_menubar_show_side_pane (CODESLAYER_MENUBAR (priv->menubar));
}

static void
show_bottom_pane_action (CodeSlayerMenuBarView *menubar_view)
{
  CodeSlayerMenuBarViewPrivate *priv;
  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);
  codeslayer_menubar_show_bottom_pane (CODESLAYER_MENUBAR (priv->menubar));
}

static void
draw_spaces_action (CodeSlayerMenuBarView *menubar_view)
{
  CodeSlayerMenuBarViewPrivate *priv;
  priv = CODESLAYER_MENUBAR_VIEW_GET_PRIVATE (menubar_view);
  codeslayer_menubar_draw_spaces (CODESLAYER_MENUBAR (priv->menubar));
}
