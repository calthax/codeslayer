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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <codeslayer/codeslayer-registry.h>
#include <codeslayer/codeslayer-utils.h>

/**
 * SECTION:codeslayer-registry
 * @short_description: The application properties.
 * @title: CodeSlayerRegistry
 * @include: codeslayer/codeslayer-registry.h
 */

static void codeslayer_registry_class_init  (CodeSlayerRegistryClass *klass);

static void codeslayer_registry_class_init  (CodeSlayerRegistryClass *klass);
static void codeslayer_registry_init        (CodeSlayerRegistry      *registry);
static void codeslayer_registry_finalize    (CodeSlayerRegistry      *registry);

#define CODESLAYER_REGISTRY_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CODESLAYER_REGISTRY_TYPE, CodeSlayerRegistryPrivate))

typedef struct _CodeSlayerRegistryPrivate CodeSlayerRegistryPrivate;

struct _CodeSlayerRegistryPrivate
{
  CodeSlayerProfiles *profiles;
};

enum
{
  REGISTRY_CHANGED,
  REGISTRY_INITIALIZED,
  LAST_SIGNAL
};

static guint codeslayer_registry_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (CodeSlayerRegistry, codeslayer_registry, G_TYPE_OBJECT)

static void 
codeslayer_registry_class_init (CodeSlayerRegistryClass *klass)
{
  /**
   * CodeSlayerRegistry::registry-changed
   *
   * The ::registry-changed signal lets all observers know that 
   * something in the registry changed.
   */
  codeslayer_registry_signals[REGISTRY_CHANGED] =
    g_signal_new ("registry-changed", 
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET (CodeSlayerRegistryClass, registry_changed), 
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  /**
   * CodeSlayerRegistry::registry-initialized
   *
   * The ::registry-initialized signal lets all observers know that 
   * the registry is initialized.
   */
  codeslayer_registry_signals[REGISTRY_INITIALIZED] =
    g_signal_new ("registry-initialized", 
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET (CodeSlayerRegistryClass, registry_initialized), 
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  G_OBJECT_CLASS (klass)->finalize = (GObjectFinalizeFunc) codeslayer_registry_finalize;
  g_type_class_add_private (klass, sizeof (CodeSlayerRegistryPrivate));
}

static void
codeslayer_registry_init (CodeSlayerRegistry *registry)
{
}

static void
codeslayer_registry_finalize (CodeSlayerRegistry *registry)
{
  G_OBJECT_CLASS (codeslayer_registry_parent_class)->finalize (G_OBJECT (registry));
}

/**
 * codeslayer_registry_new:
 *
 * Creates a new #CodeSlayerRegistry.
 *
 * Returns: a new #CodeSlayerRegistry. 
 */
CodeSlayerRegistry*
codeslayer_registry_new (CodeSlayerProfiles *profiles)
{
  CodeSlayerRegistry *registry;
  CodeSlayerRegistryPrivate *priv;

  registry = g_object_new (codeslayer_registry_get_type (), NULL);
  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  priv->profiles = profiles;

  return registry;
}

/**
 * codeslayer_registry_get_integer:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 *
 * Returns: the value as an integer for the given key.
 */
gint
codeslayer_registry_get_integer (CodeSlayerRegistry *registry,
                                 gchar              *key)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  const gchar *value;

  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  profile = codeslayer_profiles_get_profile (priv->profiles);

  value = codeslayer_profile_get_setting (profile, key);
  if (value != NULL)
    return atoi (value);

  return -1;
}

/**
 * codeslayer_registry_set_integer:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 * @value: a property value as a gint.
 */
void
codeslayer_registry_set_integer (CodeSlayerRegistry *registry,
                                 gchar              *key, 
                                 gint                value)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  gchar *val;

  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  profile = codeslayer_profiles_get_profile (priv->profiles);

  val = g_strdup_printf ("%d", value);
  codeslayer_profile_set_setting (profile, key, val);
  g_free (val);
}

/**
 * codeslayer_registry_get_double:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 *
 * Returns: the value as a double for the given key.
 */
gdouble
codeslayer_registry_get_double (CodeSlayerRegistry *registry,
                                gchar                 *key)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  const gchar *value;

  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  
  profile = codeslayer_profiles_get_profile (priv->profiles);

  value = codeslayer_profile_get_setting (profile, key);
  if (value != NULL)
    return atof (value);

  return -1;
}

/**
 * codeslayer_registry_set_double:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 * @value: a property value as a gdouble.
 */
void
codeslayer_registry_set_double (CodeSlayerRegistry *registry,
                                gchar              *key, 
                                gdouble             value)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  gchar *val;
  
  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  profile = codeslayer_profiles_get_profile (priv->profiles);
  
  val = g_strdup_printf ("%f", value);
  codeslayer_profile_set_setting (profile, key, val);
  g_free (val);
}

/**
 * codeslayer_registry_get_boolean:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 *
 * Returns: the value as a boolean for the given key.
 */
gboolean
codeslayer_registry_get_boolean (CodeSlayerRegistry *registry,
                                 gchar              *key)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  const gchar *value;

  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  profile = codeslayer_profiles_get_profile (priv->profiles);

  value = codeslayer_profile_get_setting (profile, key);
  if (value != NULL)
    {
      if (g_strcmp0 (value, "true") == 0)
        return TRUE;
      else
        return FALSE;
    }
  
  return FALSE;
}

/**
 * codeslayer_registry_set_boolean:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 * @value: a property value as a gboolean.
 */
void
codeslayer_registry_set_boolean (CodeSlayerRegistry *registry,
                                 gchar                 *key, 
                                 gboolean               value)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  
  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  profile = codeslayer_profiles_get_profile (priv->profiles);
  
  if (value == TRUE)  
    codeslayer_profile_set_setting (profile, key, "true");
  else
    codeslayer_profile_set_setting (profile, key, "false");
}

/**
 * codeslayer_registry_get_string:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 *
 * Returns: the value as a string for the given key.
 */
gchar*
codeslayer_registry_get_string (CodeSlayerRegistry *registry,
                                gchar              *key)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  const gchar *value;

  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  profile = codeslayer_profiles_get_profile (priv->profiles);

  value = codeslayer_profile_get_setting (profile, key);
  if (value != NULL)
    return g_strdup (value);

  return g_strdup ("");
}

/**
 * codeslayer_registry_set_string:
 * @registry: a #CodeSlayerRegistry.
 * @key: a property key.
 * @value: a property value as a gchar pointer.
 */
void
codeslayer_registry_set_string (CodeSlayerRegistry *registry,
                                gchar              *key, 
                                gchar              *value)
{
  CodeSlayerRegistryPrivate *priv;
  CodeSlayerProfile *profile;
  
  priv = CODESLAYER_REGISTRY_GET_PRIVATE (registry);
  profile = codeslayer_profiles_get_profile (priv->profiles);

  codeslayer_profile_set_setting (profile, key, value);
}
