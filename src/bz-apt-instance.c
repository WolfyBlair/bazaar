/* bz-apt-instance.c
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

#define G_LOG_DOMAIN  "BAZAAR::APT"
#define BAZAAR_MODULE "apt"

#include "config.h"

#include "bz-apt-instance.h"
#include "bz-backend.h"
#include "bz-backend-notification.h"
#include "bz-entry.h"
#include "bz-env.h"
#include "bz-util.h"

#include <gio/gio.h>

G_DEFINE_QUARK (bz-apt-error-quark, bz_apt_error);

struct _BzAptInstance
{
  GObject parent_instance;
  
  DexScheduler *scheduler;
  gboolean      available;
  
  GMutex     notif_mutex;
  GPtrArray *notif_channels;
};

static void
backend_iface_init (BzBackendInterface *iface);

G_DEFINE_FINAL_TYPE_WITH_CODE (
    BzAptInstance,
    bz_apt_instance,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (BZ_TYPE_BACKEND, backend_iface_init));

static gboolean
check_apt_available (void)
{
  g_autoptr (GSubprocess) proc = NULL;
  g_autoptr (GError) error     = NULL;
  
  proc = g_subprocess_new (
      G_SUBPROCESS_FLAGS_STDOUT_SILENCE | G_SUBPROCESS_FLAGS_STDERR_SILENCE,
      &error,
      "which", "apt", NULL);
  
  if (proc == NULL)
    return FALSE;
  
  return g_subprocess_wait_check (proc, NULL, NULL);
}

static DexFuture *
bz_apt_instance_real_create_notification_channel (BzBackend *backend)
{
  BzAptInstance *self = BZ_APT_INSTANCE (backend);
  DexChannel    *channel;
  
  g_mutex_lock (&self->notif_mutex);
  
  channel = dex_channel_new (10);
  g_ptr_array_add (self->notif_channels, dex_ref (channel));
  
  g_mutex_unlock (&self->notif_mutex);
  
  return DEX_FUTURE (channel);
}

static DexFuture *
bz_apt_instance_real_load_local_package (BzBackend    *backend,
                                         GFile        *file,
                                         GCancellable *cancellable)
{
  return dex_future_new_reject (
      BZ_APT_ERROR,
      BZ_APT_ERROR_NOT_AVAILABLE,
      "APT does not support loading local packages directly");
}

static DexFuture *
bz_apt_instance_real_retrieve_remote_entries (BzBackend    *backend,
                                              GCancellable *cancellable)
{
  BzAptInstance *self = BZ_APT_INSTANCE (backend);
  
  if (!self->available)
    {
      return dex_future_new_reject (
          BZ_APT_ERROR,
          BZ_APT_ERROR_NOT_AVAILABLE,
          "APT is not available on this system");
    }
  
  return dex_future_new_for_boolean (TRUE);
}

static DexFuture *
bz_apt_instance_real_retrieve_install_ids (BzBackend    *backend,
                                           GCancellable *cancellable)
{
  GHashTable *installed_set;
  
  installed_set = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  
  return dex_future_new_for_boxed (G_TYPE_HASH_TABLE, installed_set);
}

static DexFuture *
bz_apt_instance_real_retrieve_update_ids (BzBackend    *backend,
                                          GCancellable *cancellable)
{
  g_autoptr (GPtrArray) update_ids = NULL;
  
  update_ids = g_ptr_array_new_with_free_func (g_free);
  
  return dex_future_new_take_boxed (G_TYPE_PTR_ARRAY, g_steal_pointer (&update_ids));
}

static DexFuture *
bz_apt_instance_real_schedule_transaction (BzBackend    *backend,
                                           BzEntry     **installs,
                                           guint         n_installs,
                                           BzEntry     **updates,
                                           guint         n_updates,
                                           BzEntry     **removals,
                                           guint         n_removals,
                                           DexChannel   *channel,
                                           GCancellable *cancellable)
{
  BzAptInstance *self = BZ_APT_INSTANCE (backend);
  
  if (!self->available)
    {
      return dex_future_new_reject (
          BZ_APT_ERROR,
          BZ_APT_ERROR_NOT_AVAILABLE,
          "APT is not available on this system");
    }
  
  return dex_future_new_for_boolean (TRUE);
}

static void
backend_iface_init (BzBackendInterface *iface)
{
  iface->create_notification_channel = bz_apt_instance_real_create_notification_channel;
  iface->load_local_package          = bz_apt_instance_real_load_local_package;
  iface->retrieve_remote_entries     = bz_apt_instance_real_retrieve_remote_entries;
  iface->retrieve_install_ids        = bz_apt_instance_real_retrieve_install_ids;
  iface->retrieve_update_ids         = bz_apt_instance_real_retrieve_update_ids;
  iface->schedule_transaction        = bz_apt_instance_real_schedule_transaction;
}

static void
bz_apt_instance_finalize (GObject *object)
{
  BzAptInstance *self = BZ_APT_INSTANCE (object);
  
  g_clear_pointer (&self->notif_channels, g_ptr_array_unref);
  g_mutex_clear (&self->notif_mutex);
  dex_clear (&self->scheduler);
  
  G_OBJECT_CLASS (bz_apt_instance_parent_class)->finalize (object);
}

static void
bz_apt_instance_class_init (BzAptInstanceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  object_class->finalize = bz_apt_instance_finalize;
}

static void
bz_apt_instance_init (BzAptInstance *self)
{
  self->scheduler = dex_scheduler_ref (dex_scheduler_get_default ());
  self->available = check_apt_available ();
  
  g_mutex_init (&self->notif_mutex);
  self->notif_channels = g_ptr_array_new_with_free_func (dex_unref);
  
  if (self->available)
    g_info ("APT backend initialized successfully");
  else
    g_info ("APT is not available on this system");
}

static DexFuture *
init_fiber (gpointer user_data)
{
  g_autoptr (BzAptInstance) self = NULL;
  
  self = g_object_new (BZ_TYPE_APT_INSTANCE, NULL);
  
  return dex_future_new_for_pointer (g_steal_pointer (&self));
}

DexFuture *
bz_apt_instance_new (void)
{
  return dex_scheduler_spawn (
      dex_scheduler_get_default (),
      bz_get_dex_stack_size (),
      (DexFiberFunc) init_fiber,
      NULL,
      NULL);
}
