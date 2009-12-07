/*
 * peas-plugin-loader.c
 * This file is part of libpeas
 *
 * Copyright (C) 2008 - Jesse van den Kieboom
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "peas-plugin-loader.h"

G_DEFINE_ABSTRACT_TYPE (PeasPluginLoader, peas_plugin_loader, G_TYPE_OBJECT);

static void
peas_plugin_loader_init (PeasPluginLoader *loader)
{
}

static void
peas_plugin_loader_class_init (PeasPluginLoaderClass *klass)
{
}

void
peas_plugin_loader_add_module_directory (PeasPluginLoader *loader,
                                         const gchar      *module_dir)
{
  PeasPluginLoaderClass *klass;

  g_return_if_fail (PEAS_IS_PLUGIN_LOADER (loader));

  klass = PEAS_PLUGIN_LOADER_GET_CLASS (loader);
  g_return_if_fail (klass->add_module_directory != NULL);

  klass->add_module_directory (loader, module_dir);
}

PeasPlugin *
peas_plugin_loader_load (PeasPluginLoader *loader,
                         PeasPluginInfo   *info)
{
  PeasPluginLoaderClass *klass;

  g_return_val_if_fail (PEAS_IS_PLUGIN_LOADER (loader), NULL);

  klass = PEAS_PLUGIN_LOADER_GET_CLASS (loader);
  g_return_val_if_fail (klass->load != NULL, NULL);

  return klass->load (loader, info);
}

void
peas_plugin_loader_unload (PeasPluginLoader *loader,
                           PeasPluginInfo   *info)
{
  PeasPluginLoaderClass *klass;

  g_return_if_fail (PEAS_IS_PLUGIN_LOADER (loader));

  klass = PEAS_PLUGIN_LOADER_GET_CLASS (loader);
  g_return_if_fail (klass->unload != NULL);

  klass->unload (loader, info);
}

void
peas_plugin_loader_garbage_collect (PeasPluginLoader *loader)
{
  PeasPluginLoaderClass *klass;

  g_return_if_fail (PEAS_IS_PLUGIN_LOADER (loader));

  klass = PEAS_PLUGIN_LOADER_GET_CLASS (loader);

  if (klass->garbage_collect != NULL)
    klass->garbage_collect (loader);
}