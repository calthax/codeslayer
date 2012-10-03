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

#include <gtksourceview/gtksourceview.h>
#include <codeslayer/codeslayer-processes-page.h>
#include <codeslayer/codeslayer-process.h>

static void codeslayer_processes_page_class_init  (CodeSlayerProcessesPageClass *klass);
static void codeslayer_processes_page_init        (CodeSlayerProcessesPage      *page);
static void codeslayer_processes_page_finalize    (CodeSlayerProcessesPage      *page);

static void process_started_action                (CodeSlayerProcessesPage      *page, 
                                                   CodeSlayerProcess            *process);
static void process_finished_action               (CodeSlayerProcessesPage      *page, 
                                                   CodeSlayerProcess            *process);
                                                   
static gboolean remove_finished_process           (GtkTreeModel                 *model,
                                                   GtkTreePath                  *path,
                                                   GtkTreeIter                  *iter,
                                                   CodeSlayerProcess            *process);

#define CODESLAYER_PROCESSES_PAGE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CODESLAYER_PROCESSES_PAGE_TYPE, CodeSlayerProcessesPagePrivate))

typedef struct _CodeSlayerProcessesPagePrivate CodeSlayerProcessesPagePrivate;

struct _CodeSlayerProcessesPagePrivate
{
  CodeSlayerProcesses *processes;
  GtkWidget           *tree;
  GtkListStore        *store;
};

enum
{
  ICON,
  TEXT,
  PROCESS,
  COLUMNS
};

G_DEFINE_TYPE (CodeSlayerProcessesPage, codeslayer_processes_page, GTK_TYPE_VBOX)

static void
codeslayer_processes_page_class_init (CodeSlayerProcessesPageClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = (GObjectFinalizeFunc) codeslayer_processes_page_finalize;
  g_type_class_add_private (klass, sizeof (CodeSlayerProcessesPagePrivate));
}

static void
codeslayer_processes_page_init (CodeSlayerProcessesPage *page) 
{
  CodeSlayerProcessesPagePrivate *priv;
  GtkWidget *tree;
  GtkListStore *store;
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkTreeSelection *selection;
  GtkWidget *scrolled_window;
  
  priv = CODESLAYER_PROCESSES_PAGE_GET_PRIVATE (page);
  
  tree = gtk_tree_view_new ();
  priv->tree = tree;
  store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
  priv->store = store;
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree), FALSE);
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree), GTK_TREE_MODEL (store));
  g_object_unref (store);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);
  
  column = gtk_tree_view_column_new ();

  renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (column, renderer, FALSE);
  gtk_tree_view_column_set_attributes (column, renderer, "stock-id", ICON, NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_column_set_attributes (column, renderer, "text", TEXT, NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (scrolled_window), tree);

  gtk_box_pack_start (GTK_BOX (page), scrolled_window, TRUE, TRUE, 0);
}

static void
codeslayer_processes_page_finalize (CodeSlayerProcessesPage *page)
{
  G_OBJECT_CLASS (codeslayer_processes_page_parent_class)->finalize (G_OBJECT(page));
}

GtkWidget*
codeslayer_processes_page_new (CodeSlayerProcesses *processes)
{
  CodeSlayerProcessesPagePrivate *priv;
  GtkWidget *page;

  page = g_object_new (codeslayer_processes_page_get_type (), NULL);
  priv = CODESLAYER_PROCESSES_PAGE_GET_PRIVATE (page);
  priv->processes = processes;

  g_signal_connect_swapped (G_OBJECT (priv->processes), "process-started",
                            G_CALLBACK (process_started_action), CODESLAYER_PROCESSES_PAGE(page));

  g_signal_connect_swapped (G_OBJECT (priv->processes), "process-finished",
                            G_CALLBACK (process_finished_action), CODESLAYER_PROCESSES_PAGE(page));

  return page;
}

static void
process_started_action (CodeSlayerProcessesPage *page, 
                        CodeSlayerProcess       *process)
{
  CodeSlayerProcessesPagePrivate *priv;
  const gchar* name;
  GtkTreeIter iter;

  priv = CODESLAYER_PROCESSES_PAGE_GET_PRIVATE (page);
  
  name = codeslayer_process_get_name (process);
  
  gtk_list_store_append (priv->store, &iter);
  gtk_list_store_set (priv->store, &iter, ICON, GTK_STOCK_EXECUTE, TEXT, name, PROCESS, process, -1);  
}

static void
process_finished_action (CodeSlayerProcessesPage *page, 
                         CodeSlayerProcess       *process)
{
  CodeSlayerProcessesPagePrivate *priv;  
  priv = CODESLAYER_PROCESSES_PAGE_GET_PRIVATE (page);
  
  gtk_tree_model_foreach (GTK_TREE_MODEL (priv->store), 
                          (GtkTreeModelForeachFunc) remove_finished_process, process);
}

static gboolean 
remove_finished_process (GtkTreeModel      *model,
                         GtkTreePath       *path,
                         GtkTreeIter       *iter,
                         CodeSlayerProcess *process)
{
  CodeSlayerProcess *model_process;
  gtk_tree_model_get (model, iter, PROCESS, &model_process, -1);
  
  if (model_process == process)
    {
      gtk_list_store_remove (GTK_LIST_STORE (model), iter);
      return TRUE;
    }
  
  return FALSE;
}                         
