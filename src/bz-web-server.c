/* bz-web-server.c
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

#define G_LOG_DOMAIN "BAZAAR::WEB_SERVER"

#include "config.h"

#include <libsoup/soup.h>
#include <gio/gio.h>
#include <json-glib/json-glib.h>

#include "bz-web-server.h"

struct _BzWebServer
{
  GObject      parent_instance;
  
  SoupServer  *server;
  char        *web_root;
  guint        port;
};

G_DEFINE_FINAL_TYPE (BzWebServer, bz_web_server, G_TYPE_OBJECT)

static const char *
get_mime_type (const char *path)
{
  if (g_str_has_suffix (path, ".html"))
    return "text/html";
  else if (g_str_has_suffix (path, ".css"))
    return "text/css";
  else if (g_str_has_suffix (path, ".js"))
    return "application/javascript";
  else if (g_str_has_suffix (path, ".json"))
    return "application/json";
  else if (g_str_has_suffix (path, ".png"))
    return "image/png";
  else if (g_str_has_suffix (path, ".jpg") || g_str_has_suffix (path, ".jpeg"))
    return "image/jpeg";
  else if (g_str_has_suffix (path, ".svg"))
    return "image/svg+xml";
  else if (g_str_has_suffix (path, ".ico"))
    return "image/x-icon";
  
  return "application/octet-stream";
}

static void
handle_static_file (BzWebServer       *self,
                    SoupServerMessage *msg,
                    const char        *path)
{
  g_autofree char *file_path = NULL;
  g_autoptr(GFile) file = NULL;
  g_autoptr(GBytes) bytes = NULL;
  g_autoptr(GError) error = NULL;
  const char *mime_type;
  
  if (g_strcmp0 (path, "/") == 0)
    path = "/index.html";
  
  file_path = g_build_filename (self->web_root, path, NULL);
  file = g_file_new_for_path (file_path);
  
  bytes = g_file_load_bytes (file, NULL, NULL, &error);
  if (bytes == NULL)
    {
      g_warning ("Failed to load file %s: %s", file_path, error->message);
      soup_server_message_set_status (msg, SOUP_STATUS_NOT_FOUND, NULL);
      return;
    }
  
  mime_type = get_mime_type (path);
  
  soup_server_message_set_status (msg, SOUP_STATUS_OK, NULL);
  soup_message_headers_set_content_type (soup_server_message_get_response_headers (msg),
                                         mime_type, NULL);
  
  soup_message_body_append_bytes (soup_server_message_get_response_body (msg), bytes);
}

static void
handle_api_apps (BzWebServer       *self,
                 SoupServerMessage *msg)
{
  g_autoptr(JsonBuilder) builder = json_builder_new ();
  g_autoptr(JsonGenerator) gen = json_generator_new ();
  g_autoptr(JsonNode) root = NULL;
  g_autofree char *json_str = NULL;
  
  json_builder_begin_object (builder);
  json_builder_set_member_name (builder, "apps");
  json_builder_begin_array (builder);
  
  const char *sample_apps[] = {
    "org.gnome.Builder", "GNOME Builder", "An IDE for GNOME", "🔨",
    "org.gimp.GIMP", "GIMP", "Create images and edit photographs", "🎨",
    "org.inkscape.Inkscape", "Inkscape", "Vector Graphics Editor", "✏️",
    "org.blender.Blender", "Blender", "3D Creation Suite", "🎬",
    NULL
  };
  
  for (int i = 0; sample_apps[i] != NULL; i += 4)
    {
      json_builder_begin_object (builder);
      json_builder_set_member_name (builder, "id");
      json_builder_add_string_value (builder, sample_apps[i]);
      json_builder_set_member_name (builder, "name");
      json_builder_add_string_value (builder, sample_apps[i + 1]);
      json_builder_set_member_name (builder, "summary");
      json_builder_add_string_value (builder, sample_apps[i + 2]);
      json_builder_set_member_name (builder, "icon");
      json_builder_add_string_value (builder, sample_apps[i + 3]);
      json_builder_end_object (builder);
    }
  
  json_builder_end_array (builder);
  json_builder_end_object (builder);
  
  root = json_builder_get_root (builder);
  json_generator_set_root (gen, root);
  json_str = json_generator_to_data (gen, NULL);
  
  soup_server_message_set_status (msg, SOUP_STATUS_OK, NULL);
  soup_message_headers_set_content_type (soup_server_message_get_response_headers (msg),
                                         "application/json", NULL);
  soup_message_body_append (soup_server_message_get_response_body (msg),
                           SOUP_MEMORY_COPY,
                           json_str,
                           strlen (json_str));
}

static void
handle_api_installed (BzWebServer       *self,
                      SoupServerMessage *msg)
{
  g_autoptr(JsonBuilder) builder = json_builder_new ();
  g_autoptr(JsonGenerator) gen = json_generator_new ();
  g_autoptr(JsonNode) root = NULL;
  g_autofree char *json_str = NULL;
  
  json_builder_begin_object (builder);
  json_builder_set_member_name (builder, "apps");
  json_builder_begin_array (builder);
  json_builder_end_array (builder);
  json_builder_end_object (builder);
  
  root = json_builder_get_root (builder);
  json_generator_set_root (gen, root);
  json_str = json_generator_to_data (gen, NULL);
  
  soup_server_message_set_status (msg, SOUP_STATUS_OK, NULL);
  soup_message_headers_set_content_type (soup_server_message_get_response_headers (msg),
                                         "application/json", NULL);
  soup_message_body_append (soup_server_message_get_response_body (msg),
                           SOUP_MEMORY_COPY,
                           json_str,
                           strlen (json_str));
}

static void
handle_api_updates (BzWebServer       *self,
                    SoupServerMessage *msg)
{
  g_autoptr(JsonBuilder) builder = json_builder_new ();
  g_autoptr(JsonGenerator) gen = json_generator_new ();
  g_autoptr(JsonNode) root = NULL;
  g_autofree char *json_str = NULL;
  
  json_builder_begin_object (builder);
  json_builder_set_member_name (builder, "apps");
  json_builder_begin_array (builder);
  json_builder_end_array (builder);
  json_builder_end_object (builder);
  
  root = json_builder_get_root (builder);
  json_generator_set_root (gen, root);
  json_str = json_generator_to_data (gen, NULL);
  
  soup_server_message_set_status (msg, SOUP_STATUS_OK, NULL);
  soup_message_headers_set_content_type (soup_server_message_get_response_headers (msg),
                                         "application/json", NULL);
  soup_message_body_append (soup_server_message_get_response_body (msg),
                           SOUP_MEMORY_COPY,
                           json_str,
                           strlen (json_str));
}

static void
handle_api_install (BzWebServer       *self,
                    SoupServerMessage *msg)
{
  soup_server_message_set_status (msg, SOUP_STATUS_OK, NULL);
  soup_message_headers_set_content_type (soup_server_message_get_response_headers (msg),
                                         "application/json", NULL);
  soup_message_body_append (soup_server_message_get_response_body (msg),
                           SOUP_MEMORY_STATIC,
                           "{\"success\":true}",
                           strlen ("{\"success\":true}"));
}

static void
handle_api_remove (BzWebServer       *self,
                   SoupServerMessage *msg)
{
  soup_server_message_set_status (msg, SOUP_STATUS_OK, NULL);
  soup_message_headers_set_content_type (soup_server_message_get_response_headers (msg),
                                         "application/json", NULL);
  soup_message_body_append (soup_server_message_get_response_body (msg),
                           SOUP_MEMORY_STATIC,
                           "{\"success\":true}",
                           strlen ("{\"success\":true}"));
}

static void
on_request (SoupServer        *server,
            SoupServerMessage *msg,
            const char        *path,
            GHashTable        *query,
            gpointer           user_data)
{
  BzWebServer *self = BZ_WEB_SERVER (user_data);
  const char *method = soup_server_message_get_method (msg);
  
  soup_message_headers_append (soup_server_message_get_response_headers (msg),
                               "Access-Control-Allow-Origin", "*");
  soup_message_headers_append (soup_server_message_get_response_headers (msg),
                               "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  soup_message_headers_append (soup_server_message_get_response_headers (msg),
                               "Access-Control-Allow-Headers", "Content-Type");
  
  if (g_strcmp0 (method, "OPTIONS") == 0)
    {
      soup_server_message_set_status (msg, SOUP_STATUS_OK, NULL);
      return;
    }
  
  g_message ("Request: %s %s", method, path);
  
  if (g_str_has_prefix (path, "/api/"))
    {
      if (g_strcmp0 (path, "/api/apps") == 0 && g_strcmp0 (method, "GET") == 0)
        handle_api_apps (self, msg);
      else if (g_strcmp0 (path, "/api/installed") == 0 && g_strcmp0 (method, "GET") == 0)
        handle_api_installed (self, msg);
      else if (g_strcmp0 (path, "/api/updates") == 0 && g_strcmp0 (method, "GET") == 0)
        handle_api_updates (self, msg);
      else if (g_strcmp0 (path, "/api/install") == 0 && g_strcmp0 (method, "POST") == 0)
        handle_api_install (self, msg);
      else if (g_strcmp0 (path, "/api/remove") == 0 && g_strcmp0 (method, "POST") == 0)
        handle_api_remove (self, msg);
      else
        soup_server_message_set_status (msg, SOUP_STATUS_NOT_FOUND, NULL);
    }
  else
    {
      handle_static_file (self, msg, path);
    }
}

static void
bz_web_server_finalize (GObject *object)
{
  BzWebServer *self = BZ_WEB_SERVER (object);
  
  g_clear_pointer (&self->web_root, g_free);
  g_clear_object (&self->server);
  
  G_OBJECT_CLASS (bz_web_server_parent_class)->finalize (object);
}

static void
bz_web_server_class_init (BzWebServerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  object_class->finalize = bz_web_server_finalize;
}

static void
bz_web_server_init (BzWebServer *self)
{
}

BzWebServer *
bz_web_server_new (const char *web_root,
                   guint       port)
{
  BzWebServer *self;
  g_autoptr(GError) error = NULL;
  
  self = g_object_new (BZ_TYPE_WEB_SERVER, NULL);
  
  self->web_root = g_strdup (web_root);
  self->port = port;
  
  self->server = soup_server_new (NULL, NULL);
  if (self->server == NULL)
    {
      g_warning ("Failed to create soup server");
      g_object_unref (self);
      return NULL;
    }
  
  soup_server_add_handler (self->server, NULL, on_request, self, NULL);
  
  return self;
}

void
bz_web_server_start (BzWebServer *self)
{
  g_autoptr(GError) error = NULL;
  
  g_return_if_fail (BZ_IS_WEB_SERVER (self));
  
  if (!soup_server_listen_all (self->server, self->port, 0, &error))
    {
      g_warning ("Failed to start web server on port %u: %s", 
                 self->port, error->message);
      return;
    }
  
  g_message ("Web server started on http://localhost:%u", self->port);
}

void
bz_web_server_stop (BzWebServer *self)
{
  g_return_if_fail (BZ_IS_WEB_SERVER (self));
  
  if (self->server)
    soup_server_disconnect (self->server);
  
  g_message ("Web server stopped");
}

guint
bz_web_server_get_port (BzWebServer *self)
{
  g_return_val_if_fail (BZ_IS_WEB_SERVER (self), 0);
  
  return self->port;
}
