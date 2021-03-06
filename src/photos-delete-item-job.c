/*
 * Photos - access, organize and share your photos on GNOME
 * Copyright © 2013, 2014 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/* Based on code from:
 *   + Documents
 */


#include "config.h"

#include <gio/gio.h>
#include <glib.h>
#include <tracker-sparql.h>

#include "photos-delete-item-job.h"
#include "photos-query.h"
#include "photos-query-builder.h"
#include "photos-search-context.h"
#include "photos-tracker-queue.h"


struct _PhotosDeleteItemJobPrivate
{
  PhotosDeleteItemJobCallback callback;
  PhotosTrackerQueue *queue;
  gchar *urn;
  gpointer user_data;
};

enum
{
  PROP_0,
  PROP_URN
};


G_DEFINE_TYPE_WITH_PRIVATE (PhotosDeleteItemJob, photos_delete_item_job, G_TYPE_OBJECT);


static void
photos_delete_item_job_query_executed (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  PhotosDeleteItemJob *self = PHOTOS_DELETE_ITEM_JOB (user_data);
  PhotosDeleteItemJobPrivate *priv = self->priv;
  TrackerSparqlConnection *connection = TRACKER_SPARQL_CONNECTION (source_object);
  GError *error;

  error = NULL;
  tracker_sparql_connection_update_finish (connection, res, &error);
  if (error != NULL)
    {
      g_warning ("Unable to delete item: %s", error->message);
      g_error_free (error);
      goto out;
    }

 out:
  if (priv->callback != NULL)
    (*priv->callback) (priv->user_data);
}


static void
photos_delete_item_job_dispose (GObject *object)
{
  PhotosDeleteItemJob *self = PHOTOS_DELETE_ITEM_JOB (object);

  g_clear_object (&self->priv->queue);

  G_OBJECT_CLASS (photos_delete_item_job_parent_class)->dispose (object);
}


static void
photos_delete_item_job_finalize (GObject *object)
{
  PhotosDeleteItemJob *self = PHOTOS_DELETE_ITEM_JOB (object);

  g_free (self->priv->urn);

  G_OBJECT_CLASS (photos_delete_item_job_parent_class)->finalize (object);
}


static void
photos_delete_item_job_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  PhotosDeleteItemJob *self = PHOTOS_DELETE_ITEM_JOB (object);

  switch (prop_id)
    {
    case PROP_URN:
      self->priv->urn = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
photos_delete_item_job_init (PhotosDeleteItemJob *self)
{
  PhotosDeleteItemJobPrivate *priv;

  self->priv = photos_delete_item_job_get_instance_private (self);
  priv = self->priv;

  priv->queue = photos_tracker_queue_dup_singleton (NULL, NULL);
}


static void
photos_delete_item_job_class_init (PhotosDeleteItemJobClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = photos_delete_item_job_dispose;
  object_class->finalize = photos_delete_item_job_finalize;
  object_class->set_property = photos_delete_item_job_set_property;

  g_object_class_install_property (object_class,
                                   PROP_URN,
                                   g_param_spec_string ("urn",
                                                        "Uniform Resource Name",
                                                        "An unique ID associated with this item",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));
}


PhotosDeleteItemJob *
photos_delete_item_job_new (const gchar *urn)
{
  return g_object_new (PHOTOS_TYPE_DELETE_ITEM_JOB, "urn", urn, NULL);
}


void
photos_delete_item_job_run (PhotosDeleteItemJob *self,
                            PhotosDeleteItemJobCallback callback,
                            gpointer user_data)
{
  PhotosDeleteItemJobPrivate *priv = self->priv;
  GApplication *app;
  PhotosQuery *query;
  PhotosSearchContextState *state;

  if (G_UNLIKELY (priv->queue == NULL))
    {
      if (callback != NULL)
        (*callback) (user_data);
      return;
    }

  priv->callback = callback;
  priv->user_data = user_data;

  app = g_application_get_default ();
  state = photos_search_context_get_state (PHOTOS_SEARCH_CONTEXT (app));

  query = photos_query_builder_delete_resource_query (state, priv->urn);
  photos_tracker_queue_update (priv->queue,
                               query->sparql,
                               NULL,
                               photos_delete_item_job_query_executed,
                               g_object_ref (self),
                               g_object_unref);
  photos_query_free (query);
}
