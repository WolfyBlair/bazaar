/* bz-snap-instance.h
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

#define BZ_SNAP_ERROR (bz_snap_error_quark ())
GQuark bz_snap_error_quark (void);

typedef enum
{
  BZ_SNAP_ERROR_CANNOT_INITIALIZE = 0,
  BZ_SNAP_ERROR_SYNCHRONIZATION_FAILURE,
  BZ_SNAP_ERROR_TRANSACTION_FAILURE,
  BZ_SNAP_ERROR_NOT_AVAILABLE,
} BzSnapError;

#define BZ_TYPE_SNAP_INSTANCE (bz_snap_instance_get_type ())
G_DECLARE_FINAL_TYPE (BzSnapInstance, bz_snap_instance, BZ, SNAP_INSTANCE, GObject)

DexFuture *
bz_snap_instance_new (void);

G_END_DECLS
