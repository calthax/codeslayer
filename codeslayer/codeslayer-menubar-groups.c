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

#include <codeslayer/codeslayer-menubar.h>
#include <codeslayer/codeslayer-menubar-groups.h>
#include <codeslayer/codeslayer-utils.h>
#include <codeslayer/codeslayer-group-properties.h>

/**
 * SECTION:codeslayer-menubar-groups
 * @short_description: The groups menu.
 * @title: CodeSlayerMenuBarGroups
 * @include: codeslayer/codeslayer-menu-groups.h
 */

static void codeslayer_menu_bar_groups_class_init  (CodeSlayerMenuBarGroupsClass *klass);
static void codeslayer_menu_bar_groups_init        (CodeSlayerMenuBarGroups      *menu_bar_groups);
static void codeslayer_menu_bar_groups_finalize    (CodeSlayerMenuBarGroups      *menu_bar_groups);

static void select_group_action                    (GtkCheckMenuItem             *radio_item, 
                                                    CodeSlayerMenuBarGroups      *menu_bar_groups);
static gint compare_groups                         (CodeSlayerGroup              *group1, 
                                                    CodeSlayerGroup              *group2);
static void new_group_action                       (CodeSlayerMenuBarGroups      *menu_bar_groups);
static void remove_group_action                    (CodeSlayerMenuBarGroups      *menu_bar_groups);
static void group_properties_action                (CodeSlayerMenuBarGroups      *menu_bar_groups);
static void add_groups                             (CodeSlayerMenuBarGroups      *menu_bar_groups,
                                                    CodeSlayerGroups             *groups);
static void add_menu_items                         (CodeSlayerMenuBarGroups      *menu_bar_groups, 
                                                    CodeSlayerGroups             *groups);
                            
#define CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CODESLAYER_MENU_BAR_GROUPS_TYPE, CodeSlayerMenuBarGroupsPrivate))

typedef struct _CodeSlayerMenuBarGroupsPrivate CodeSlayerMenuBarGroupsPrivate;

struct _CodeSlayerMenuBarGroupsPrivate
{
  GtkWidget        *window;
  CodeSlayerGroups *groups;
  GtkAccelGroup    *accel_group;
  GSList           *radio_group;
  GList            *radio_items;
  GtkWidget        *menu_bar;
  GtkWidget        *menu;
  GtkWidget        *remove_group_item;
};

G_DEFINE_TYPE (CodeSlayerMenuBarGroups, codeslayer_menu_bar_groups, GTK_TYPE_MENU_ITEM)

static void
codeslayer_menu_bar_groups_class_init (CodeSlayerMenuBarGroupsClass *klass)
{
  G_OBJECT_CLASS (klass)->finalize = (GObjectFinalizeFunc) codeslayer_menu_bar_groups_finalize;
  g_type_class_add_private (klass, sizeof (CodeSlayerMenuBarGroupsPrivate));
}

static void
codeslayer_menu_bar_groups_init (CodeSlayerMenuBarGroups *menu_bar_groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GtkWidget *menu;

  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);
  
  gtk_menu_item_set_label (GTK_MENU_ITEM (menu_bar_groups), _("Groups"));
  
  menu = gtk_menu_new ();
  priv->menu = menu;
  priv->radio_group = NULL;
  priv->radio_items = NULL;
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_bar_groups), menu);
}

static void
codeslayer_menu_bar_groups_finalize (CodeSlayerMenuBarGroups *menu_bar_groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);

  if (priv->radio_items != NULL)
    g_list_free (priv->radio_items);

  G_OBJECT_CLASS (codeslayer_menu_bar_groups_parent_class)->finalize (G_OBJECT (menu_bar_groups));
}

/**
 * codeslayer_menu_bar_groups_new:
 * @window: a #GtkWindow.
 * @menu_bar: a #CodeSlayerMenuBar.
 * @accel_group: a #GtkAccelGroup.
 * @groups: a #CodeSlayerGroups.
 *
 * Creates a new #CodeSlayerMenuBarGroups.
 *
 * Returns: a new #CodeSlayerMenuBarGroups. 
 */
GtkWidget*
codeslayer_menu_bar_groups_new (GtkWidget        *window, 
                                GtkWidget        *menu_bar, 
                                GtkAccelGroup    *accel_group,
                                CodeSlayerGroups *groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GtkWidget *menu_bar_groups;
  
  menu_bar_groups = g_object_new (codeslayer_menu_bar_groups_get_type (), NULL);
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);

  priv->window = window;
  priv->menu_bar = menu_bar;
  priv->accel_group = accel_group;
  priv->groups = groups;
  priv->remove_group_item = NULL;

  add_menu_items (CODESLAYER_MENU_BAR_GROUPS (menu_bar_groups), groups);

  return menu_bar_groups;
}

static void
add_menu_items (CodeSlayerMenuBarGroups *menu_bar_groups, 
                CodeSlayerGroups        *groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GtkWidget *separator_item;
  GtkWidget *new_group_item;
  GtkWidget *remove_group_item;
  GtkWidget *properties_item;
  GList *list;
  
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);

  gtk_menu_set_accel_group (GTK_MENU (priv->menu), priv->accel_group);

  add_groups (CODESLAYER_MENU_BAR_GROUPS (menu_bar_groups), groups);

  separator_item = gtk_separator_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), separator_item);

  new_group_item = gtk_menu_item_new_with_label (_("New Group"));
  remove_group_item = gtk_menu_item_new_with_label (_("Remove Group"));
  properties_item = gtk_menu_item_new_with_label (_("Group Properties"));

  priv->remove_group_item = remove_group_item;

  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), new_group_item);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), remove_group_item);
  
  separator_item = gtk_separator_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), separator_item);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), properties_item);
  
  list = codeslayer_groups_get_list (groups);
  gtk_widget_set_sensitive (priv->remove_group_item, g_list_length (list) != 1);

  g_signal_connect_swapped (G_OBJECT (new_group_item), "activate",
                            G_CALLBACK (new_group_action), menu_bar_groups);

  g_signal_connect_swapped (G_OBJECT (remove_group_item), "activate",
                            G_CALLBACK (remove_group_action), menu_bar_groups);

  g_signal_connect_swapped (G_OBJECT (properties_item), "activate",
                            G_CALLBACK (group_properties_action), menu_bar_groups);
}

static gint
compare_groups (CodeSlayerGroup *group1, 
                CodeSlayerGroup *group2)
{
  return g_strcmp0 (codeslayer_group_get_name (group1),
                    codeslayer_group_get_name (group2));
}

static void
add_groups (CodeSlayerMenuBarGroups *menu_bar_groups, 
            CodeSlayerGroups        *groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GList *list;
  int i;
  guint length;
  gboolean sensitive;
  
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);

  list = g_list_copy (codeslayer_groups_get_list (groups));
  list = g_list_sort (list, (GCompareFunc) compare_groups);
  length = g_list_length (list);
  sensitive = length != 1;

  for (i = 0; i < length; i++)
    {
      CodeSlayerGroup *group = g_list_nth_data (list, i);
      const gchar *group_name = codeslayer_group_get_name (group);

      GtkWidget *radio_item;
      radio_item = gtk_radio_menu_item_new_with_label (priv->radio_group, group_name);
      gtk_menu_shell_insert (GTK_MENU_SHELL (priv->menu), radio_item, i);
      priv->radio_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (radio_item));
      
      priv->radio_items = g_list_append (priv->radio_items, radio_item);

      if (group == codeslayer_groups_get_active_group (groups))
        {
          gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (radio_item), TRUE);
          if (priv->remove_group_item)
            gtk_widget_set_sensitive (priv->remove_group_item, sensitive);
        }

      g_signal_connect (G_OBJECT (radio_item), "toggled",
                        G_CALLBACK (select_group_action), menu_bar_groups);
    }

  g_list_free (list);

  gtk_widget_show_all (GTK_WIDGET (menu_bar_groups));
}

/**
 * codeslayer_menu_bar_groups_refresh_groups:
 * @menu_bar_groups: a #CodeSlayerMenuBarGroups.
 * @groups: a #CodeSlayerGroups.
 * 
 * Update the groups menu item to reflect the current #CodeSlayerGroup 
 * objects in the manager.
 */
void
codeslayer_menu_bar_groups_refresh_groups (CodeSlayerMenuBarGroups *menu_bar_groups,
                                           CodeSlayerGroups        *groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GList *radio_items;
  
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);
  
  radio_items = priv->radio_items;
  while (radio_items != NULL)
    {
      GtkWidget *radio_item = radio_items->data;
      gtk_container_remove (GTK_CONTAINER (priv->menu), radio_item);
      radio_items = g_list_next (radio_items);
    }

  if (priv->radio_items != NULL)
    g_list_free (priv->radio_items);
  
  priv->radio_items = NULL;
  priv->radio_group = NULL;

  add_groups (menu_bar_groups, groups);
}

static void
select_group_action (GtkCheckMenuItem        *radio_item, 
                     CodeSlayerMenuBarGroups *menu_bar_groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  const gchar *group_name;
  
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);

  if (!gtk_check_menu_item_get_active (radio_item))
    return;

  group_name = gtk_menu_item_get_label (GTK_MENU_ITEM (radio_item));
  codeslayer_menu_bar_group_changed (CODESLAYER_MENU_BAR (priv->menu_bar), group_name);
}

static void
new_group_action (CodeSlayerMenuBarGroups *menu_bar_groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GtkWidget *dialog;
  GtkWidget *content_area;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *entry;
  gint response;
  
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);
  
  dialog = gtk_dialog_new_with_buttons (_("New Group"), 
                                        GTK_WINDOW (priv->window),
                                        GTK_DIALOG_MODAL,
                                        GTK_STOCK_CANCEL,
                                        GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK,
                                        GTK_RESPONSE_OK,
                                        NULL);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (dialog), TRUE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (dialog), TRUE);
                                        
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  /*gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);*/

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_set_homogeneous (GTK_BOX (vbox), FALSE);
  
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX (hbox), FALSE);

  label = gtk_label_new (_("Name:"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 4);

  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  gtk_entry_set_width_chars (GTK_ENTRY (entry), 50);
  gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 4);

  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, FALSE);

  gtk_widget_show_all (vbox);

  gtk_container_add (GTK_CONTAINER (content_area), vbox);

  response = gtk_dialog_run (GTK_DIALOG (dialog));
  if (response == GTK_RESPONSE_OK)
    {
      const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
      gchar *group_name = g_strstrip (g_strdup (text));
      codeslayer_menu_bar_new_group (CODESLAYER_MENU_BAR (priv->menu_bar), group_name);
      g_free (group_name);
    }

  gtk_widget_destroy (dialog);
}

static void
remove_group_action (CodeSlayerMenuBarGroups *menu_bar_groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GtkWidget *dialog;
  gint response;
  CodeSlayerGroup *active_group;
  gchar *text;
  
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);
  
  active_group = codeslayer_groups_get_active_group (priv->groups);
  text = g_strdup_printf(_("Are you sure you want to remove the %s group?"), codeslayer_group_get_name (active_group));
  
  dialog = gtk_message_dialog_new (GTK_WINDOW (priv->window),
                                   GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_WARNING,
                                   GTK_BUTTONS_OK_CANCEL,
                                   text, NULL);
  g_free (text);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (dialog), TRUE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (dialog), TRUE);

  gtk_window_set_title (GTK_WINDOW (dialog), _("Remove Group"));
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

  response = gtk_dialog_run (GTK_DIALOG (dialog));
  if (response == GTK_RESPONSE_CANCEL)
    {
      gtk_widget_destroy (dialog);
      return;
    }
  gtk_widget_destroy (dialog);

  /* confirmed that will remove the group */
  
  codeslayer_menu_bar_remove_group (CODESLAYER_MENU_BAR (priv->menu_bar));
}

static void
group_properties_action (CodeSlayerMenuBarGroups *menu_bar_groups)
{
  CodeSlayerMenuBarGroupsPrivate *priv;
  GtkWidget *group_properties;
  CodeSlayerGroup *active_group;
  GtkWidget *dialog;
  gint response;
  GtkWidget *content_area;
  
  priv = CODESLAYER_MENU_BAR_GROUPS_GET_PRIVATE (menu_bar_groups);
  
  active_group = codeslayer_groups_get_active_group (priv->groups);
  group_properties = codeslayer_group_properties_new (active_group);

  dialog = gtk_dialog_new_with_buttons (_("Group Properties"), 
                                        GTK_WINDOW (priv->window),
                                        GTK_DIALOG_MODAL,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK, GTK_RESPONSE_OK,
                                        NULL);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (dialog), TRUE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (dialog), TRUE);
  /*gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);*/

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  gtk_container_add (GTK_CONTAINER (content_area), group_properties);
  gtk_widget_show_all (content_area);  

  response = gtk_dialog_run (GTK_DIALOG (dialog));
  if (response == GTK_RESPONSE_OK)
    {
      const gchar *name;
      gchar *rename;
      name = codeslayer_group_properties_get_name (CODESLAYER_GROUP_PROPERTIES (group_properties));

      rename = g_strstrip (g_strdup (name));

      if (g_strcmp0 (rename, codeslayer_group_get_name (active_group)) != 0)
        codeslayer_menu_bar_rename_group (CODESLAYER_MENU_BAR (priv->menu_bar), rename);

      g_free (rename);
    }
  
  gtk_widget_destroy (dialog);
}
