/* web-main.c
 *
 * Copyright 2025 Adam Masciola
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config.h"

#include <glib.h>
#include <glib/gprintf.h>
#include <signal.h>

#include "bz-web-server.h"

static BzWebServer *server = NULL;
static GMainLoop *loop = NULL;

static void
signal_handler (int signum)
{
  if (loop != NULL)
    {
      g_message ("Shutting down...");
      g_main_loop_quit (loop);
    }
}

int
main (int   argc,
      char *argv[])
{
  g_autofree char *web_root = NULL;
  guint port = 8080;
  
  if (argc > 1)
    web_root = g_strdup (argv[1]);
  else
    web_root = g_build_filename (g_get_current_dir (), "web", NULL);
  
  if (argc > 2)
    port = atoi (argv[2]);
  
  g_message ("Starting Bazaar Web Server");
  g_message ("Web root: %s", web_root);
  g_message ("Port: %u", port);
  
  server = bz_web_server_new (web_root, port);
  if (server == NULL)
    {
      g_critical ("Failed to create web server");
      return 1;
    }
  
  bz_web_server_start (server);
  
  signal (SIGINT, signal_handler);
  signal (SIGTERM, signal_handler);
  
  g_message ("Press Ctrl+C to stop the server");
  g_message ("Open http://localhost:%u in your browser", port);
  
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);
  
  bz_web_server_stop (server);
  g_object_unref (server);
  g_main_loop_unref (loop);
  
  return 0;
}
