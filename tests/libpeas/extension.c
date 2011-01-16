/*
 * extension.c
 * This file is part of libpeas
 *
 * Copyright (C) 2010 - Garrett Regier
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <glib.h>
#include <libpeas/peas.h>

#include "testing/testing.h"

#include "introspection/introspection-callable.h"
#include "introspection/introspection-properties.h"
#include "introspection/introspection-unimplemented.h"

typedef struct _TestFixture TestFixture;

struct _TestFixture {
  PeasEngine *engine;
};

static void
test_setup (TestFixture   *fixture,
            gconstpointer  data)
{
  fixture->engine = testing_engine_new ();
}

static void
test_teardown (TestFixture   *fixture,
               gconstpointer  data)
{
  testing_engine_free (fixture->engine);
}

static void
test_runner (TestFixture   *fixture,
             gconstpointer  data)
{
  ((void (*) (PeasEngine *engine)) data) (fixture->engine);
}

static void
test_extension_create_valid (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;

  info = peas_engine_get_plugin_info (engine, "loadable");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            PEAS_TYPE_ACTIVATABLE,
                                            "object", NULL,
                                            NULL);

  g_assert (PEAS_IS_EXTENSION (extension));
  g_assert (PEAS_IS_ACTIVATABLE (extension));

  g_object_unref (extension);
}

static void
test_extension_create_invalid (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;

  info = peas_engine_get_plugin_info (engine, "loadable");

  /* Not loaded */
  if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
    {
      extension = peas_engine_create_extension (engine, info,
                                                PEAS_TYPE_ACTIVATABLE,
                                                "object", NULL,
                                                NULL);
      /* Resident Modules */
      g_object_unref (extension);
      exit (0);
    }
  /* Resident modules cause this to fail?
  g_test_trap_assert_failed ();*/

  g_assert (peas_engine_load_plugin (engine, info));

  /* Invalid GType */
  if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
    {
      extension = peas_engine_create_extension (engine, info,
                                                G_TYPE_INVALID,
                                                NULL);
      exit (0);
    }
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*CRITICAL*");


  /* GObject but not a GInterface */
  extension = peas_engine_create_extension (engine, info,
                                            PEAS_TYPE_ENGINE,
                                            NULL);
  g_assert (!PEAS_IS_EXTENSION (extension));


  /* Does not implement this GType */
  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_UNIMPLEMENTED,
                                            NULL);
  g_assert (!PEAS_IS_EXTENSION (extension));
}

static void
test_extension_properties_construct_only (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;
  gchar *construct_only;

  info = peas_engine_get_plugin_info (engine, "properties");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_PROPERTIES,
                                            "construct-only", "my-construct-only",
                                            NULL);


  g_object_get (extension, "construct-only", &construct_only, NULL);
  g_assert_cmpstr (construct_only, ==, "my-construct-only");
  g_free (construct_only);

  if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
    {
      g_object_set (extension, "construct-only", "other-construct-only", NULL);
      exit (0);
    }
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*WARNING*");

  g_object_unref (extension);
}

static void
test_extension_properties_read_only (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;
  gchar *read_only;

  info = peas_engine_get_plugin_info (engine, "properties");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_PROPERTIES,
                                            NULL);

  g_object_get (extension, "read-only", &read_only, NULL);
  g_assert_cmpstr (read_only, ==, "read-only");
  g_free (read_only);

  if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
    {
      g_object_set (extension, "read-only", "my-read-only", NULL);
      exit (0);
    }
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*WARNING*");

  g_object_unref (extension);
}

static void
test_extension_properties_write_only (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;

  info = peas_engine_get_plugin_info (engine, "properties");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_PROPERTIES,
                                            NULL);

  if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
    {
      gchar *write_only = NULL;

      g_object_get (extension, "write-only", &write_only, NULL);
      exit (0);
    }
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*WARNING*");

  g_object_set (extension, "write-only", "my-write-only", NULL);

  g_object_unref (extension);
}

static void
test_extension_properties_readwrite (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;
  gchar *readwrite;

  info = peas_engine_get_plugin_info (engine, "properties");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_PROPERTIES,
                                            NULL);

  g_object_get (extension, "readwrite", &readwrite, NULL);
  g_assert_cmpstr (readwrite, ==, "readwrite");
  g_free (readwrite);

  g_object_set (extension, "readwrite", "my-readwrite", NULL);

  g_object_get (extension, "readwrite", &readwrite, NULL);
  g_assert_cmpstr (readwrite, ==, "my-readwrite");
  g_free (readwrite);

  g_object_unref (extension);
}

static void
test_extension_call_invalid (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;

  info = peas_engine_get_plugin_info (engine, "loadable");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            PEAS_TYPE_ACTIVATABLE,
                                            "object", NULL,
                                            NULL);

  g_assert (PEAS_IS_ACTIVATABLE (extension));

  if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
    {
      peas_extension_call (extension, "invalid", NULL);
      exit (0);
    }
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Method 'PeasActivatable.invalid' not found*");

  g_object_unref (extension);
}

static void
test_extension_call_no_args (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;

  info = peas_engine_get_plugin_info (engine, "loadable");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            PEAS_TYPE_ACTIVATABLE,
                                            "object", NULL,
                                            NULL);

  g_assert (PEAS_IS_ACTIVATABLE (extension));

  g_assert (peas_extension_call (extension, "activate"));

  g_object_unref (extension);
}

static void
test_extension_call_with_return (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;
  const gchar *return_val = NULL;

  info = peas_engine_get_plugin_info (engine, "callable");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_CALLABLE,
                                            NULL);

  g_assert (INTROSPECTION_IS_CALLABLE (extension));

  g_assert (peas_extension_call (extension, "call_with_return", &return_val));
  g_assert_cmpstr (return_val, ==, "Hello, World!");

  g_object_unref (extension);
}

static void
test_extension_call_single_arg (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;
  gboolean called = FALSE;

  info = peas_engine_get_plugin_info (engine, "callable");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_CALLABLE,
                                            NULL);

  g_assert (INTROSPECTION_IS_CALLABLE (extension));

  g_assert (peas_extension_call (extension, "call_single_arg", &called));
  g_assert (called);

  g_object_unref (extension);
}

static void
test_extension_call_multi_args (PeasEngine *engine)
{
  PeasPluginInfo *info;
  PeasExtension *extension;
  gboolean params[3] = { FALSE, FALSE, FALSE };

  info = peas_engine_get_plugin_info (engine, "callable");

  g_assert (peas_engine_load_plugin (engine, info));

  extension = peas_engine_create_extension (engine, info,
                                            INTROSPECTION_TYPE_CALLABLE,
                                            NULL);

  g_assert (INTROSPECTION_IS_CALLABLE (extension));

  g_assert (peas_extension_call (extension, "call_multi_args",
                                 &params[0], &params[1], &params[2]));
  g_assert (params[0] && params[1] && params[2]);

  g_object_unref (extension);
}

int
main (int    argc,
      char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_type_init ();

#define TEST(path, ftest) \
  g_test_add ("/extension/" path, TestFixture, \
              (gpointer) test_extension_##ftest, \
              test_setup, test_runner, test_teardown)

  TEST ("create-valid", create_valid);
  TEST ("create-invalid", create_invalid);

  TEST ("properties-construct-only", properties_construct_only);
  TEST ("properties-read-only", properties_read_only);
  TEST ("properties-write-only", properties_write_only);
  TEST ("properties-readwrite", properties_readwrite);

  TEST ("call-invalid", call_invalid);
  TEST ("call-no-args", call_no_args);
  TEST ("call-with-return", call_with_return);
  TEST ("call-single-arg", call_single_arg);
  TEST ("call-multi-args", call_multi_args);

#undef TEST

  g_object_unref (peas_engine_get_default ());

  return g_test_run ();
}
