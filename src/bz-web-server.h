/* bz-web-server.h
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

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define BZ_TYPE_WEB_SERVER (bz_web_server_get_type())

G_DECLARE_FINAL_TYPE (BzWebServer, bz_web_server, BZ, WEB_SERVER, GObject)

BzWebServer *bz_web_server_new        (const char  *web_root,
                                       guint        port);
void         bz_web_server_start      (BzWebServer *self);
void         bz_web_server_stop       (BzWebServer *self);
guint        bz_web_server_get_port   (BzWebServer *self);

G_END_DECLS
