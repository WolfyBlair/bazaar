/* bz-deb-instance.h
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

#include <libdex.h>

G_BEGIN_DECLS

#define BZ_DEB_ERROR (bz_deb_error_quark ())
GQuark bz_deb_error_quark (void);

typedef enum
{
  BZ_DEB_ERROR_CANNOT_INITIALIZE = 0,
  BZ_DEB_ERROR_INVALID_PACKAGE,
  BZ_DEB_ERROR_TRANSACTION_FAILURE,
  BZ_DEB_ERROR_NOT_AVAILABLE,
} BzDebError;

#define BZ_TYPE_DEB_INSTANCE (bz_deb_instance_get_type ())
G_DECLARE_FINAL_TYPE (BzDebInstance, bz_deb_instance, BZ, DEB_INSTANCE, GObject)

DexFuture *
bz_deb_instance_new (void);

G_END_DECLS
