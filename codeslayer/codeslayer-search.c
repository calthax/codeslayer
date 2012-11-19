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

#include <glib/gprintf.h>
#include <string.h>
#include <codeslayer/codeslayer-search.h>
#include <codeslayer/codeslayer-project.h>
#include <codeslayer/codeslayer-document.h>
#include <codeslayer/codeslayer-utils.h>
#include <codeslayer/codeslayer-group.h>

/**
 * SECTION:codeslayer-search-page
 * @short_description: The global search page.
 * @title: CodeSlayerSearch
 * @include: codeslayer/codeslayer-search-page.h
 *
 * The global search will find text in the files under the active group.
 */

typedef struct
{
  CodeSlayerSearch  *search;
  gchar             *label_name;
  GList             *search_files;
  CodeSlayerProject *project;
} SearchContext;

static void codeslayer_search_class_init    (CodeSlayerSearchClass *klass);
static void codeslayer_search_init          (CodeSlayerSearch      *search);
static void codeslayer_search_finalize      (CodeSlayerSearch      *search);
static void codeslayer_search_get_property  (GObject               *object, 
                                             guint                  prop_id,
                                             GValue                *value,
                                             GParamSpec            *pspec);
static void codeslayer_search_set_property  (GObject               *object,
                                             guint                  prop_id,
                                             const GValue          *value,
                                             GParamSpec            *pspec);
                                                  
static void add_widgets                     (CodeSlayerSearch      *search);
static void add_close_button                (CodeSlayerSearch      *search);
static void add_find_entry                  (CodeSlayerSearch      *search);
static void add_file_entry                  (CodeSlayerSearch      *search);
static void add_stop_button                 (CodeSlayerSearch      *search);
static void add_find_button                 (CodeSlayerSearch      *search);
static void add_match_case_button           (CodeSlayerSearch      *search);
static void close_action                    (CodeSlayerSearch      *search);
static void find_action                     (CodeSlayerSearch      *search);                                             
static void stop_action                     (CodeSlayerSearch      *search);
static void match_case_action               (CodeSlayerSearch      *search);
static void execute                         (CodeSlayerSearch      *search);
static void search_projects                 (CodeSlayerSearch      *search,
                                             GPatternSpec          *find_pattern,
                                             GPatternSpec          *file_pattern,
                                             GList                 *include_types,
                                             GList                 *exclude_dirs);
static void create_search_files             (CodeSlayerSearch      *search,
                                             GPatternSpec          *find_pattern, 
                                             GPatternSpec          *file_pattern, 
                                             GFile                 *file,
                                             GList                 **search_files, 
                                             GList                 *include_types,
                                             GList                 *exclude_dirs);
static void create_search_results           (CodeSlayerSearch      *search,
                                             GPatternSpec          *find_pattern, 
                                             GPatternSpec          *file_pattern, 
                                             GFile                 *file, 
                                             GList                 **search_files);
static void add_project                     (CodeSlayerSearch      *search,
                                             CodeSlayerProject     *project,
                                             GtkTreeIter           *project_iter);
static gboolean select_document             (CodeSlayerSearch      *search,
                                             GtkTreeIter           *treeiter,
                                             GtkTreeViewColumn     *column);
static gchar* get_globbing                  (const gchar           *entry, 
                                             gboolean               to_lowercase);
static void get_text                        (gchar                 **text, 
                                             gboolean               to_lowercase);
static gboolean is_active                   (GtkWidget             *toggle_button);
static gint sort_iter_compare_func          (GtkTreeModel          *model, 
                                             GtkTreeIter           *a, 
                                             GtkTreeIter           *b, 
                                             gpointer               userdata);
                                                  
static gboolean set_stop_button_sensitive   (GtkWidget             *stop_button);
static gboolean create_search_tree          (SearchContext         *context);
static void destroy_search_tree             (SearchContext         *context);


#define CODESLAYER_SEARCH_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CODESLAYER_SEARCH_TYPE, CodeSlayerSearchPrivate))

typedef struct _CodeSlayerSearchPrivate CodeSlayerSearchPrivate;

struct _CodeSlayerSearchPrivate
{
  GtkWindow               *parent;
  CodeSlayerGroups        *groups;
  CodeSlayerPreferences   *preferences;
  GtkWidget               *vbox;
  GtkWidget               *grid;
  GtkWidget               *find_entry;
  GtkWidget               *file_entry;
  GtkWidget               *stop_button;
  GtkWidget               *find_button;
  GtkWidget               *match_case_button;
  GtkWidget               *treeview;
  GtkTreeStore            *treestore;
  GtkCellRenderer         *renderer;
  gchar                   *file_paths;
  gboolean                 stop_request;
  const gchar             *find_text;
  const gchar             *file_text;
};

enum
{
  FILE_PATH = 0,
  LINE_NUMBER,
  TEXT,
  PROJECT,
  COLUMNS
};

enum
{
  SELECT_DOCUMENT,
  CLOSE,
  LAST_SIGNAL
};

static guint codeslayer_search_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (CodeSlayerSearch, codeslayer_search, GTK_TYPE_WINDOW)

enum
{
  PROP_0,
  PROP_FILE_PATHS
};

typedef struct
{
  gchar *file_path;
  gint  line_number;
  gchar *text;

} SearchResult;

typedef struct
{
  gchar *file_name;
  gchar *file_path;
  GList *search_results;

} SearchFile;

static void 
codeslayer_search_class_init (CodeSlayerSearchClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /**
   * CodeSlayerSearch::select-document
   * @codeslayersearch: the search that received the signal
   *
   * Note: for internal use only.
   *
   * The ::select-document signal is a request to select the document in the 
   * tree (which in turn adds a page in the notebook).
   */
  codeslayer_search_signals[SELECT_DOCUMENT] =
    g_signal_new ("select-document", 
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET (CodeSlayerSearchClass, select_document),
                  NULL, NULL, 
                  g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
                  
  /**
   * CodeSlayerSearch::close
   * @codeslayersearch: the search that received the signal
   *
   * Note: for internal use only.
   *
   * The ::close signal is a request to close the search box.
   */
  codeslayer_search_signals[CLOSE] =
    g_signal_new ("close", 
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET (CodeSlayerSearchClass, close),
                  NULL, NULL, 
                  g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  G_OBJECT_CLASS (klass)->finalize = (GObjectFinalizeFunc) codeslayer_search_finalize;
  
  gobject_class->get_property = codeslayer_search_get_property;
  gobject_class->set_property = codeslayer_search_set_property;
  
  g_type_class_add_private (klass, sizeof (CodeSlayerSearchPrivate));
}

static void
codeslayer_search_init (CodeSlayerSearch *search)
{
  gtk_window_set_title (GTK_WINDOW (search), _("Search"));  
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (search), TRUE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (search), TRUE);
  gtk_container_set_border_width (GTK_CONTAINER (search), 3);
}

static void
codeslayer_search_finalize (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);  
  if (priv->file_paths)
    {
      g_free (priv->file_paths);
      priv->file_paths = NULL;
    }
  priv->stop_request = TRUE;
  G_OBJECT_CLASS (codeslayer_search_parent_class)-> finalize (G_OBJECT (search));
}

static void
codeslayer_search_get_property (GObject    *object, 
                                     guint       prop_id,
                                     GValue     *value, 
                                     GParamSpec *pspec)
{
  CodeSlayerSearchPrivate *priv;
  CodeSlayerSearch *search;
  
  search = CODESLAYER_SEARCH (object);
  priv =CODESLAYER_SEARCH_GET_PRIVATE (search);

  switch (prop_id)
    {
    case PROP_FILE_PATHS:
      g_value_set_string (value, priv->file_paths);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
codeslayer_search_set_property (GObject      *object, 
                                     guint         prop_id,
                                     const GValue *value, 
                                     GParamSpec   *pspec)
{
  CodeSlayerSearch *search;
  search = CODESLAYER_SEARCH (object);

  switch (prop_id)
    {
    case PROP_FILE_PATHS:
      codeslayer_search_set_file_paths (search, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/**
 * codeslayer_search_new:
 * @preferences: a #CodeSlayerPreferences.
 * @groups: a #CodeSlayerGroups.
 *
 * Creates a new #CodeSlayerSearch.
 *
 * Returns: a new #CodeSlayerSearch. 
 */
GtkWidget*
codeslayer_search_new (GtkWindow             *window, 
                            CodeSlayerPreferences *preferences, 
                            CodeSlayerGroups      *groups)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *search;
  
  search = g_object_new (codeslayer_search_get_type (), NULL);
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  priv->parent = window;
  priv->preferences = preferences;
  priv->groups = groups;
  priv->file_paths = NULL;
  
  gtk_window_set_transient_for (GTK_WINDOW (search), window);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (search), TRUE);

  add_widgets (CODESLAYER_SEARCH (search));
  
  add_close_button (CODESLAYER_SEARCH (search));
  
  return search;
}

/*static void
add_vbox (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *vbox;
  GtkWidget *button_box;
  GtkWidget *close_button;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

  gtk_box_set_homogeneous (GTK_BOX (vbox), FALSE);
  priv->vbox = vbox;
  
  gtk_box_pack_start (GTK_BOX(vbox), search, TRUE, TRUE, 2);
  
  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (button_box), GTK_BUTTONBOX_END);
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 4);
  close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_box_pack_start (GTK_BOX(button_box), close_button, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), button_box, FALSE, FALSE, 0);
  
  g_signal_connect_swapped (G_OBJECT (close_button), "clicked",
                            G_CALLBACK (close_action), search);

  g_signal_connect_swapped (G_OBJECT (search), "select-document",
                            G_CALLBACK (open_document_action), search);

  gtk_container_add (GTK_CONTAINER (search), vbox);
}*/

static void
add_widgets (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *vbox;
  GtkWidget *grid;
  GtkWidget *treeview;
  GtkTreeStore *treestore;
  GtkTreeSortable *sortable;
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkWidget *scrolled_window;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  priv->vbox = vbox;

  gtk_box_set_homogeneous (GTK_BOX (vbox), FALSE);
  gtk_box_set_spacing (GTK_BOX (vbox), 0);

  /* add the search page fields */

  grid = gtk_grid_new ();
  priv->grid = grid;

  add_find_entry (search);
  add_stop_button (search);
  add_match_case_button (search);
  add_file_entry (search);
  add_find_button (search);

  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (priv->grid), FALSE, FALSE, 2);

  /* add the search page results */

  treeview = gtk_tree_view_new ();
  priv->treeview = treeview;

  treestore = gtk_tree_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_INT, 
                                  G_TYPE_STRING, G_TYPE_POINTER);
  priv->treestore = treestore;

  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (treestore));
  g_object_unref (treestore);

  sortable = GTK_TREE_SORTABLE (treestore);
  gtk_tree_sortable_set_sort_func (sortable, TEXT, sort_iter_compare_func,
                                   GINT_TO_POINTER (TEXT), NULL);
  gtk_tree_sortable_set_sort_column_id (sortable, TEXT, GTK_SORT_ASCENDING);

  column = gtk_tree_view_column_new ();

  renderer = gtk_cell_renderer_text_new ();
  priv->renderer = renderer;

  gtk_tree_view_column_pack_start (column, renderer, FALSE);
  gtk_tree_view_column_set_attributes (column, renderer, "text", TEXT, NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (treeview));
  gtk_box_pack_start (GTK_BOX (priv->vbox), GTK_WIDGET (scrolled_window), 
                      TRUE, TRUE, 2);

  g_signal_connect_swapped (G_OBJECT (treeview), "row_activated",
                            G_CALLBACK (select_document), search);
                            
  gtk_container_add (GTK_CONTAINER (search), vbox);                            
}

void          
codeslayer_search_grab_focus (CodeSlayerSearch  *search)
{
  CodeSlayerSearchPrivate *priv;
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  gtk_widget_grab_focus (priv->find_entry);
}

/**
 * codeslayer_search_get_file_paths:
 * @search: a #CodeSlayerSearch.
 *
 * Returns: the file paths as a comma separated list. 
 *          Note, the memory must not be freed. 
 */
const gchar*         
codeslayer_search_get_file_paths (CodeSlayerSearch *search)
{
  return CODESLAYER_SEARCH_GET_PRIVATE (search)->file_paths;
}

/**
 * codeslayer_search_set_file_paths:
 * @search: a #CodeSlayerSearch.
 * @file_paths: the file paths as a comma separated list.
 */
void 
codeslayer_search_set_file_paths (CodeSlayerSearch *search, 
                                       const gchar          *file_paths)
{
  CodeSlayerSearchPrivate *priv;
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  if (priv->file_paths)
    {
      g_free (priv->file_paths);
      priv->file_paths = NULL;
    }
  priv->file_paths = g_strdup (file_paths);
}

void          
codeslayer_search_clear (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  gtk_tree_store_clear (priv->treestore);
  gtk_entry_set_text (GTK_ENTRY (priv->find_entry), ""); 
  gtk_entry_set_text (GTK_ENTRY (priv->file_entry), ""); 
}

static void
add_find_entry (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *find_label;
  GtkWidget *find_entry;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  find_label = gtk_label_new (_("Contains Text:"));
  gtk_misc_set_alignment (GTK_MISC (find_label), 1, .5);
  gtk_misc_set_padding (GTK_MISC (find_label), 4, 0);
  gtk_grid_attach (GTK_GRID (priv->grid), find_label, 0, 0, 1, 1);

  find_entry = gtk_entry_new ();
  gtk_entry_set_width_chars (GTK_ENTRY (find_entry), 40);
  priv->find_entry = find_entry;
  gtk_grid_attach_next_to (GTK_GRID (priv->grid), find_entry, find_label, 
                           GTK_POS_RIGHT, 1, 1);

  g_signal_connect_swapped (G_OBJECT (find_entry), "activate",
                            G_CALLBACK (find_action), search);
}

static void
add_stop_button (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *stop_button;
  GtkWidget *stop_image;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  stop_button = gtk_button_new ();
  priv->stop_button = stop_button;
  gtk_widget_set_sensitive (stop_button, FALSE);

  gtk_button_set_relief (GTK_BUTTON (stop_button), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click (GTK_BUTTON (stop_button), FALSE);
  stop_image = gtk_image_new_from_stock (GTK_STOCK_STOP, GTK_ICON_SIZE_MENU);
  gtk_container_add (GTK_CONTAINER (stop_button), stop_image);
  gtk_widget_set_can_focus (stop_button, FALSE);

  gtk_grid_attach_next_to (GTK_GRID (priv->grid), stop_button, priv->find_entry, 
                           GTK_POS_RIGHT, 1, 1);

  g_signal_connect_swapped (G_OBJECT (stop_button), "clicked",
                            G_CALLBACK (stop_action), search);
}

static void
add_match_case_button (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *match_case_button;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  match_case_button = gtk_check_button_new_with_label (_("Match Case"));
  gtk_widget_set_can_focus (match_case_button, FALSE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (match_case_button), TRUE);
  priv->match_case_button = match_case_button;

  gtk_grid_attach_next_to (GTK_GRID (priv->grid), match_case_button, priv->stop_button, 
                           GTK_POS_RIGHT, 1, 1);

  g_signal_connect_swapped (G_OBJECT (match_case_button), "clicked",
                            G_CALLBACK (match_case_action), search);
}

static void
add_file_entry (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *file_label;
  GtkWidget *file_entry;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  file_label = gtk_label_new (_("File Name:"));
  gtk_misc_set_alignment (GTK_MISC (file_label), 1, .5);
  gtk_misc_set_padding (GTK_MISC (file_label), 4, 0);
  gtk_grid_attach (GTK_GRID (priv->grid), file_label, 0, 1, 1, 1);

  file_entry = gtk_entry_new ();
  priv->file_entry = file_entry;
  gtk_entry_set_width_chars (GTK_ENTRY (file_entry), 40);
  gtk_grid_attach_next_to (GTK_GRID (priv->grid), file_entry, file_label, 
                           GTK_POS_RIGHT, 1, 1);

  g_signal_connect_swapped (G_OBJECT (file_entry), "activate",
                            G_CALLBACK (find_action), search);
}

static void
add_find_button (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *find_button;
  GtkWidget *find_label;
  GtkWidget *find_image;
  GtkWidget *hbox;
  GtkWidget *container;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  find_button = gtk_button_new ();
  priv->find_button = find_button;
  gtk_button_set_relief (GTK_BUTTON (find_button), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click (GTK_BUTTON (find_button), FALSE);

  find_label = gtk_label_new (_("Find"));
  find_image = gtk_image_new_from_stock (GTK_STOCK_FIND, GTK_ICON_SIZE_MENU);
  
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX (hbox), TRUE);

  container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX (container), FALSE);

  gtk_box_pack_start (GTK_BOX (hbox), find_image, FALSE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (hbox), find_label, FALSE, TRUE, 0);
  gtk_widget_set_can_focus (find_button, FALSE);

  gtk_container_add (GTK_CONTAINER (find_button), hbox);
  
  gtk_box_pack_start (GTK_BOX (container), find_button, FALSE, FALSE, 0);  
  
  gtk_grid_attach_next_to (GTK_GRID (priv->grid), container, priv->file_entry, 
                           GTK_POS_RIGHT, 2, 1);

  g_signal_connect_swapped (G_OBJECT (find_button), "clicked",
                            G_CALLBACK (find_action), search);
}

static void
add_close_button (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  GtkWidget *button_box;
  GtkWidget *close_button;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (button_box), GTK_BUTTONBOX_END);
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 4);
  close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_box_pack_start (GTK_BOX(button_box), close_button, FALSE, FALSE, 0);
  
  g_signal_connect_swapped (G_OBJECT (close_button), "clicked",
                            G_CALLBACK (close_action), search);

  gtk_box_pack_start (GTK_BOX(priv->vbox), button_box, FALSE, FALSE, 0);
}

static void
close_action (CodeSlayerSearch *search)
{
  g_signal_emit_by_name ((gpointer) search, "close");
}

static void
find_action (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  priv->stop_request = FALSE;
  gtk_widget_set_sensitive (priv->stop_button, TRUE);

  gtk_tree_store_clear (priv->treestore);
  priv->find_text = gtk_entry_get_text (GTK_ENTRY (priv->find_entry));
  priv->file_text = gtk_entry_get_text (GTK_ENTRY (priv->file_entry));
  
  g_thread_new ("find", (GThreadFunc) execute, search);
}

static void
stop_action (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  priv->stop_request = TRUE;
}

static void
match_case_action (CodeSlayerSearch *search)
{
  find_action (search);
}

static void
execute (CodeSlayerSearch *search)
{
  CodeSlayerSearchPrivate *priv;
  
  gchar *find_globbing = NULL;
  GPatternSpec *find_pattern = NULL;  
  
  gchar *file_globbing = NULL;
  GPatternSpec *file_pattern = NULL;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  if (codeslayer_utils_has_text (priv->find_text))
    {
      find_globbing = get_globbing (priv->find_text, is_active (priv->match_case_button));
      find_pattern = g_pattern_spec_new (find_globbing);
    }

  if (codeslayer_utils_has_text (priv->file_text))
    {
      file_globbing = get_globbing (priv->file_text, is_active (priv->match_case_button));
      file_pattern = g_pattern_spec_new (file_globbing);
    }
  
  if (find_pattern != NULL || file_pattern != NULL)
    {
      gchar *exclude_types_str;
      gchar *exclude_dirs_str;
      GList *exclude_types = NULL;
      GList *exclude_dirs = NULL;
      
      exclude_types_str = codeslayer_preferences_get_string (priv->preferences,
                                                             CODESLAYER_PREFERENCES_PROJECTS_EXCLUDE_TYPES);
      exclude_dirs_str = codeslayer_preferences_get_string (priv->preferences,
                                                            CODESLAYER_PREFERENCES_PROJECTS_EXCLUDE_DIRS);
      exclude_types = codeslayer_utils_string_to_list (exclude_types_str);
      exclude_dirs = codeslayer_utils_string_to_list (exclude_dirs_str);
                                                           
      search_projects (search, find_pattern, file_pattern, exclude_types, exclude_dirs);
      
      g_free (exclude_types_str);
      g_free (exclude_dirs_str);
      if (exclude_types)
        {
          g_list_foreach (exclude_types, (GFunc) g_free, NULL);
          g_list_free (exclude_types);
        }
      if (exclude_dirs)
        {
          g_list_foreach (exclude_dirs, (GFunc) g_free, NULL);
          g_list_free (exclude_dirs);
        }
      
      if (priv->file_paths)
        {
          SearchContext *context;
        
          gchar *label_name;
          label_name = g_strdup_printf (_("Find \"%s\""),
                                        find_pattern != NULL ? priv->find_text : priv->file_text);
          context = g_malloc (sizeof (SearchContext));
          context->search = search;
          context->label_name = label_name;
        }
    }
    
  if (find_globbing != NULL)
    g_free (find_globbing);
  if (find_pattern != NULL)
    g_pattern_spec_free (find_pattern);
  
  if (file_globbing != NULL)
    g_free (file_globbing);  
  if (file_pattern != NULL)
    g_pattern_spec_free (file_pattern);

  g_idle_add ((GSourceFunc) set_stop_button_sensitive, priv->stop_button);
}

static void
search_projects (CodeSlayerSearch *search,
                 GPatternSpec         *find_pattern,
                 GPatternSpec         *file_pattern,
                 GList                *exclude_types,
                 GList                *exclude_dirs)
{
  CodeSlayerSearchPrivate *priv;
  CodeSlayerGroup *group;
  GList *projects;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  
  group = codeslayer_groups_get_active_group (priv->groups);
  projects = codeslayer_group_get_projects (group);
  
  while (projects != NULL)
    {
      CodeSlayerProject *project;
      GList *search_files = NULL;
      const gchar *folder_path;
      gchar *folder_path_expanded;
      
      project = projects->data;
      folder_path = codeslayer_project_get_folder_path (project);
      folder_path_expanded = g_strconcat (folder_path, G_DIR_SEPARATOR_S, NULL);
      
      if (priv->file_paths)
        {
          gchar **split, **tmp;
          split = g_strsplit (priv->file_paths, ";", -1);
          tmp = split;

          while (*tmp != NULL)
            {
              if (g_str_has_prefix (*tmp, folder_path_expanded))
                {
                  GFile *file;
                  file = g_file_new_for_path (*tmp);
                  create_search_files (search, find_pattern, file_pattern, file, 
                                       &search_files, exclude_types, exclude_dirs);
                  g_object_unref (file);
                }
              tmp++;
            }
          g_strfreev (split);
        }
      else
        {
          GFile *file;
          file = g_file_new_for_path (folder_path);
          create_search_files (search, find_pattern, file_pattern, file, 
                               &search_files, exclude_types, exclude_dirs);
          g_object_unref (file);
        }
        
      if (search_files != NULL)
        {
          SearchContext *context;
          context = g_malloc (sizeof (SearchContext));
          context->search = search;
          context->search_files = search_files;
          context->project = project;
          g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, (GSourceFunc) create_search_tree, context, (GDestroyNotify)destroy_search_tree);
        }

      g_free (folder_path_expanded);
      projects = g_list_next (projects);
    }
}

static void
create_search_files (CodeSlayerSearch *search,
                     GPatternSpec         *find_pattern,
                     GPatternSpec         *file_pattern,
                     GFile                *file,
                     GList                **search_files, 
                     GList                *exclude_types,
                     GList                *exclude_dirs)
{
  GFileEnumerator *enumerator;

  enumerator = g_file_enumerate_children (file, "standard::*",
                                          G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, 
                                          NULL, NULL);
  if (enumerator != NULL)
    {
      GFileInfo *file_info;
      while ((file_info = g_file_enumerator_next_file (enumerator, NULL, NULL)) != NULL)
        {
          GFile *child;

          const char *file_name = g_file_info_get_name (file_info);
          child = g_file_get_child (file, file_name);
          
          if (g_file_info_get_file_type (file_info) == G_FILE_TYPE_DIRECTORY 
                && !codeslayer_utils_contains_element (exclude_dirs, file_name))
            {
              create_search_files (search, find_pattern, file_pattern, 
                                   child, search_files, exclude_types, exclude_dirs);
            }

          if (g_file_info_get_file_type (file_info) == G_FILE_TYPE_REGULAR
                && !codeslayer_utils_contains_element_with_suffix (exclude_types, file_name))
            {
              create_search_results (search, find_pattern, file_pattern, 
                                     child, search_files);
            }
 
          g_object_unref (child);
          g_object_unref (file_info);
        }
      g_object_unref (enumerator);
    }
}

static void
create_search_results (CodeSlayerSearch *search,
                       GPatternSpec         *find_pattern,
                       GPatternSpec         *file_pattern,
                       GFile                *file,
                       GList                **search_files)
{
  CodeSlayerSearchPrivate *priv;
  gchar *file_path;
  gchar *base_name;
  gchar *file_name;

  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);
  
  base_name = g_file_get_basename (file);
  file_name = g_strdup (base_name);
  get_text (&file_name, is_active (priv->match_case_button));
  if (file_pattern != NULL && !g_pattern_match_string (file_pattern, file_name))
    {
      g_free (file_name);
      g_free (base_name);
      return;
    }
    
  file_path = g_file_get_path (file);    

  if (find_pattern == NULL)
    {
      SearchFile *search_file = NULL;
      search_file = g_malloc (sizeof (SearchFile));
      search_file->file_name = g_strdup (base_name);
      search_file->file_path = g_strdup (file_path);
      search_file->search_results = NULL;
      *search_files = g_list_append (*search_files, search_file);
    }
  else
    {
      SearchFile *search_file = NULL;
      GIOChannel *channel;
      gint line_number;
      gchar *text;

      channel = g_io_channel_new_file (file_path, "r", NULL);

      for (line_number = 1; g_io_channel_read_line (channel, &text, NULL, NULL, NULL) == G_IO_STATUS_NORMAL; line_number++)
        {
          if (text == NULL)
            continue;
            
          get_text (&text, is_active (priv->match_case_button));
          
          if (g_pattern_match_string (find_pattern, text))
            {
              SearchResult *search_result;
              search_result = g_malloc (sizeof (SearchResult));
              search_result->file_path = g_strdup (file_path);
              search_result->line_number = line_number;
              search_result->text = g_strstrip (g_strdup (text));

              if (search_file == NULL)
                {
                  search_file = g_malloc (sizeof (SearchFile));
                  search_file->file_name = g_strdup (base_name);
                  search_file->file_path = g_strdup (file_path);
                  search_file->search_results = NULL;
                  *search_files = g_list_append (*search_files, search_file);
                }
            
              search_file->search_results = g_list_append (search_file->search_results, search_result);
            }
          
          g_free (text);
        }    
    
      g_io_channel_shutdown(channel, FALSE, NULL);
      g_io_channel_unref (channel);
    }
  
  g_free (file_path);
  g_free (file_name);
  g_free (base_name);
}

static gboolean
create_search_tree (SearchContext *context)
{
  CodeSlayerSearchPrivate *priv;
  const gchar *project_folder_path;
  GtkTreeIter project_iter;

  priv = CODESLAYER_SEARCH_GET_PRIVATE (context->search);

  project_folder_path = codeslayer_project_get_folder_path (context->project);

  add_project (context->search, context->project, &project_iter);

  while (priv->stop_request == FALSE && context->search_files != NULL)
    {
      SearchFile *search_file = context->search_files->data;
      gchar *search_file_name;
      gchar *search_file_path;
      GtkTreeIter file_iter;
      GtkTreeIter text_iter;
      GList *tmp;

      search_file_path = codeslayer_utils_substr (search_file->file_path,
                                          strlen (project_folder_path) + 1,
                                          strlen (search_file->file_path));
                                          
      search_file_name = g_strconcat (search_file->file_name, 
                                      " - ", 
                                      search_file_path, NULL);
      
      gtk_tree_store_append (priv->treestore, &file_iter, &project_iter);
      
      tmp = search_file->search_results;
      if (tmp != NULL)
        {
          gtk_tree_store_set (priv->treestore, &file_iter, 
                              FILE_PATH, NULL, 
                              LINE_NUMBER, 0, 
                              TEXT, search_file_name,
                              PROJECT, context->project, -1);
        }
      else
        {
          gtk_tree_store_set (priv->treestore, &file_iter, 
                              FILE_PATH, search_file->file_path, 
                              LINE_NUMBER, 0, 
                              TEXT, search_file_name,
                              PROJECT, context->project, -1);
        }      
      
      while (tmp != NULL)
        {
          SearchResult *search_result = tmp->data;
          gchar *line_text;
          gchar *full_text;
          
          line_text = g_malloc (sizeof (gchar) * 10);
          g_sprintf (line_text, "%d", search_result->line_number);
          full_text = g_strconcat ("(", line_text, ") ", search_result->text, NULL);
                                       
          gtk_tree_store_append (priv->treestore, &text_iter, &file_iter);
          gtk_tree_store_set (priv->treestore, &text_iter,
                              FILE_PATH, search_result->file_path,
                              LINE_NUMBER, search_result->line_number,
                              TEXT, full_text,
                              PROJECT, context->project, -1);

          g_free (line_text);
          g_free (full_text);
          g_free (search_result->file_path);
          g_free (search_result->text);
          g_free (search_result);
          tmp = g_list_next (tmp);
        }
      
      g_free (search_file_path);
      g_free (search_file_name);
      g_free (search_file->file_path);
      g_free (search_file->file_name);
      g_list_free (search_file->search_results);
      g_free (search_file);
      context->search_files = g_list_next (context->search_files);
    }
  
  return FALSE;    
}

static void 
add_project (CodeSlayerSearch *search, 
             CodeSlayerProject    *project,
             GtkTreeIter          *project_iter)
{
  CodeSlayerSearchPrivate *priv;
  const gchar *project_name;

  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  gtk_tree_store_append (priv->treestore, project_iter, NULL);
  project_name = codeslayer_project_get_name (project);
  gtk_tree_store_set (priv->treestore, project_iter, 
                      FILE_PATH, NULL,
                      LINE_NUMBER, 0, 
                      TEXT, project_name, 
                      PROJECT, project, -1);
}

static gboolean
select_document (CodeSlayerSearch *search, 
                 GtkTreeIter          *treeiter, 
                 GtkTreeViewColumn    *column)
{
  CodeSlayerSearchPrivate *priv;
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *treeselection;
  
  priv = CODESLAYER_SEARCH_GET_PRIVATE (search);

  treeselection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  if (gtk_tree_selection_get_selected (treeselection, &model, &iter))
    {
      gchar *file_path = NULL;
      gint line_number;
      CodeSlayerProject *project;
      CodeSlayerDocument *document;

      gtk_tree_model_get (GTK_TREE_MODEL (priv->treestore), &iter,
                          FILE_PATH, &file_path,
                          LINE_NUMBER, &line_number, 
                          PROJECT, &project, -1);

      if (file_path == NULL)
        {
          GtkTreePath *tree_path;
          tree_path = gtk_tree_model_get_path (model, &iter);
          if (gtk_tree_view_row_expanded (GTK_TREE_VIEW (priv->treeview), tree_path))
            gtk_tree_view_collapse_row (GTK_TREE_VIEW (priv->treeview), tree_path);
          else
            gtk_tree_view_expand_row (GTK_TREE_VIEW (priv->treeview), tree_path, FALSE);
          return FALSE;
        }

      document = codeslayer_document_new ();
      codeslayer_document_set_file_path (document, file_path);
      codeslayer_document_set_line_number (document, line_number);
      codeslayer_document_set_project (document, project);
      
      g_signal_emit_by_name ((gpointer) search, "select-document", document);

      g_object_unref (document);
      g_free (file_path);
    }

  return FALSE;
}

static gchar*
get_globbing (const gchar *entry, 
              gboolean     match_case)
{
  gchar *entry_text;
  gchar *result;
  
  entry_text = g_strconcat ("*", entry, "*", NULL);
  
  if (match_case)
    {
      result = entry_text;
    }
  else
    {
      result = codeslayer_utils_to_lowercase (entry_text);
      g_free (entry_text);
    }
  
  return result;
}

static void
get_text (gchar    **text, 
          gboolean  match_case)
{
  gchar *result;

  if (match_case)
    {
      result = *text;
    }
  else
    {
      result = codeslayer_utils_to_lowercase (*text);
      g_free (*text);
    }
    
  *text = result;
}

static gboolean
is_active (GtkWidget *toggle_button)
{
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle_button));
}

static gint
sort_iter_compare_func (GtkTreeModel *model, 
                        GtkTreeIter  *a,
                        GtkTreeIter  *b, 
                        gpointer      userdata)
{
  gint ret = 0;

  gint sortcol;
  sortcol = GPOINTER_TO_INT (userdata);

  switch (sortcol)
    {
    case TEXT:
      {
        gchar *text1, *text2;
        gint line_number1, line_number2;

        gtk_tree_model_get (model, a, 
                            LINE_NUMBER, &line_number1, 
                            TEXT, &text1, -1);
        gtk_tree_model_get (model, b, 
                            LINE_NUMBER, &line_number2, 
                            TEXT, &text2, -1);

        if (line_number1 < line_number2)
          ret = -1;
        else if (line_number1 > line_number2)
          ret = 1;
        else
          ret = g_strcmp0 (text1, text2);

        g_free (text1);
        g_free (text2);
      }
      break;
    }

  return ret;
}

static gboolean 
set_stop_button_sensitive (GtkWidget *stop_button)
{
  gtk_widget_set_sensitive (stop_button, FALSE);
  return FALSE;
}

static void 
destroy_search_tree (SearchContext *context)
{
  g_list_free (context->search_files);
  g_free (context);
}
