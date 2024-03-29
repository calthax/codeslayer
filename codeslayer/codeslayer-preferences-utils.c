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

#include <codeslayer/codeslayer-preferences-utils.h>
#include <codeslayer/codeslayer-preferences.h>

GtkBox*
codeslayer_preferences_utils_content_area (GtkBox *tab, 
                                           gchar  *title)
{
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *spacer;
  gchar *full_title;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  gtk_box_set_homogeneous (GTK_BOX (vbox), FALSE);
  
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX (hbox), FALSE);
  
  /* set the title */  
  full_title = g_strdup_printf("<b>%s</b>", title);
  label = gtk_label_new (full_title);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_misc_set_padding (GTK_MISC (label), 12, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 0, .50);
  g_free (full_title);

  /* put in a spacer */
  spacer = gtk_label_new (NULL);
  gtk_misc_set_padding (GTK_MISC (spacer), 15, 0);
  gtk_misc_set_alignment (GTK_MISC (spacer), 0, .50);

  gtk_box_pack_start (GTK_BOX (tab), GTK_WIDGET (vbox), FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), spacer, FALSE, FALSE, 0);

  return GTK_BOX (hbox);
}

void
codeslayer_preferences_utils_notify (CodeSlayerRegistry *registry)
{
  g_signal_emit_by_name ((gpointer) registry, "registry-changed");
}
