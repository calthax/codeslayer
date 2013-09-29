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

#ifndef __CODESLAYER_PROFILE_HANDLER_H__
#define	__CODESLAYER_PROFILE_HANDLER_H__

#include <gtk/gtk.h>
#include <codeslayer/codeslayer-profile.h>

G_BEGIN_DECLS

#define CODESLAYER_PROFILE_HANDLER_TYPE            (codeslayer_profile_handler_get_type ())
#define CODESLAYER_PROFILE_HANDLER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CODESLAYER_PROFILE_HANDLER_TYPE, CodeSlayerProfileHandler))
#define CODESLAYER_PROFILE_HANDLER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CODESLAYER_PROFILE_HANDLER_TYPE, CodeSlayerProfileHandlerClass))
#define IS_CODESLAYER_PROFILE_HANDLER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CODESLAYER_PROFILE_HANDLER_TYPE))
#define IS_CODESLAYER_PROFILE_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CODESLAYER_PROFILE_HANDLER_TYPE))

#define CODESLAYER_PROFILES_DIR "profiles"

typedef struct _CodeSlayerProfileHandler CodeSlayerProfileHandler;
typedef struct _CodeSlayerProfileHandlerClass CodeSlayerProfileHandlerClass;

struct _CodeSlayerProfileHandler
{
  GObject parent_instance;
};

struct _CodeSlayerProfileHandlerClass
{
  GObjectClass parent_class;
};

GType
codeslayer_profile_handler_get_type (void) G_GNUC_CONST;

CodeSlayerProfileHandler*  codeslayer_profile_handler_new                   (void);
                                               
CodeSlayerProfile*         codeslayer_profile_handler_get_profile           (CodeSlayerProfileHandler *profile_handler);
CodeSlayerProfile*         codeslayer_profile_handler_load_default_profile  (CodeSlayerProfileHandler *profile_handler);
CodeSlayerProfile*         codeslayer_profile_handler_load_new_profile      (CodeSlayerProfileHandler *profile_handler, 
                                                                             GFile                   *file);
CodeSlayerProfile*         codeslayer_profile_handler_load_file_profile     (CodeSlayerProfileHandler *profile_handler, 
                                                                             GFile                   *file);
void                       codeslayer_profile_handler_save_profile          (CodeSlayerProfileHandler *profile_handler);

G_END_DECLS

#endif /* __CODESLAYER_PROFILE_HANDLER_H__ */
