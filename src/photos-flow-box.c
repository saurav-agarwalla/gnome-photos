/*
 * Photos - access, organize and share your photos on GNOME
 * Copyright © 2013 Red Hat, Inc.
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


#include "config.h"

#include <glib.h>

#include "photos-collection-manager.h"
#include "photos-enums.h"
#include "photos-flow-box.h"
#include "photos-item-manager.h"
#include "photos-view-model.h"


struct _PhotosFlowBoxPrivate
{
  GtkTreeModel *model;
  PhotosBaseManager *col_mngr;
  PhotosBaseManager *item_mngr;
  PhotosWindowMode mode;
  gchar *is_present_key;
};

enum
{
  PROP_0,
  PROP_MODE
};


G_DEFINE_TYPE_WITH_PRIVATE (PhotosFlowBox, photos_flow_box, GTK_TYPE_FLOW_BOX);


/* static gboolean */
/* photos_flow_box_configure_event (GtkWidget *widget, GdkEventConfigure *event) */
/* { */
/*   PhotosFlowBox *self = PHOTOS_FLOW_BOX (widget); */
/*   PhotosFlowBoxPrivate *priv = self->priv; */
/*   gboolean ret_val; */

/*   ret_val = GTK_WIDGET_CLASS (photos_flow_box_parent_class)->configure_event (widget, event); */

/*   if (photos_mode_controller_get_fullscreen (priv->controller)) */
/*     return ret_val; */

/*   if (priv->configure_id != 0) */
/*     { */
/*       g_source_remove (priv->configure_id); */
/*       priv->configure_id = 0; */
/*     } */

/*   priv->configure_id = g_timeout_add (CONFIGURE_ID_TIMEOUT, photos_flow_box_configure_id_timeout, self); */
/*   return ret_val; */
/* } */


/* static void */
/* photos_flow_box_fullscreen_changed (PhotosModeController *controller, gboolean fullscreen, gpointer user_data) */
/* { */
/*   PhotosFlowBox *self = PHOTOS_FLOW_BOX (user_data); */

/*   if (fullscreen) */
/*     gtk_window_fullscreen (GTK_WINDOW (self)); */
/*   else */
/*     gtk_window_unfullscreen (GTK_WINDOW (self)); */
/* } */


/* static gboolean */
/* photos_flow_box_handle_key_overview (PhotosFlowBox *self, GdkEventKey *event) */
/* { */
/*   return GDK_EVENT_PROPAGATE; */
/* } */


/* static gboolean */
/* photos_flow_box_handle_key_preview (PhotosFlowBox *self, GdkEventKey *event) */
/* { */
/*   PhotosFlowBoxPrivate *priv = self->priv; */
/*   GtkTextDirection direction; */
/*   gboolean fullscreen; */

/*   direction = gtk_widget_get_direction (GTK_WIDGET (self)); */
/*   fullscreen = photos_mode_controller_get_fullscreen (priv->controller); */

/*   if ((fullscreen && event->keyval == GDK_KEY_Escape) */
/*       || ((event->state & GDK_MOD1_MASK) != 0 */
/*           && ((direction == GTK_TEXT_DIR_LTR && event->keyval == GDK_KEY_Left) */
/*               || (direction == GTK_TEXT_DIR_RTL && event->keyval == GDK_KEY_Right))) */
/*       || event->keyval == GDK_KEY_BackSpace */
/*       || event->keyval == GDK_KEY_Back) */
/*     { */
/*       photos_base_manager_set_active_object (priv->item_mngr, NULL); */
/*       return GDK_EVENT_STOP; */
/*     } */

/*   return GDK_EVENT_PROPAGATE; */
/* } */


/* static gboolean */
/* photos_flow_box_key_press_event (GtkWidget *widget, GdkEventKey *event) */
/* { */
/*   PhotosFlowBox *self = PHOTOS_FLOW_BOX (widget); */
/*   PhotosFlowBoxPrivate *priv = self->priv; */
/*   PhotosWindowMode mode; */
/*   gboolean handled; */

/*   mode = photos_mode_controller_get_window_mode (priv->controller); */
/*   if (mode == PHOTOS_WINDOW_MODE_PREVIEW) */
/*     handled = photos_flow_box_handle_key_preview (self, event); */
/*   else */
/*     handled = photos_flow_box_handle_key_overview (self, event); */

/*   if (!handled) */
/*     handled = GTK_WIDGET_CLASS (photos_flow_box_parent_class)->key_press_event (widget, event); */

/*   return handled; */
/* } */


static void
photos_flow_box_add_item (PhotosFlowBox *self, PhotosBaseItem *item)
{
  GdkPixbuf *icon;
  GtkWidget *child;
  GtkWidget *image;

  g_object_set_data (G_OBJECT (item), self->priv->is_present_key, GINT_TO_POINTER (TRUE));

  child = gtk_flow_box_child_new ();
  g_object_set_data_full (G_OBJECT (child), "item", g_object_ref (item), g_object_unref);
  gtk_container_add (GTK_CONTAINER (self), child);

  icon = photos_base_item_get_icon (item);
  image = gtk_image_new_from_pixbuf (icon);
  g_object_bind_property (item, "icon", image, "pixbuf", G_BINDING_DEFAULT);
  gtk_container_add (GTK_CONTAINER (child), image);

  gtk_widget_show_all (child);
}


static void
photos_flow_box_remove_item (PhotosFlowBox *self, PhotosBaseItem *item)
{
  GList *children = NULL;
  GList *l;

  g_object_set_data (G_OBJECT (item), self->priv->is_present_key, NULL);

  children = gtk_container_get_children (GTK_CONTAINER (self));
  for (l = children; l != NULL; l = l->next)
    {
      GtkWidget *child = GTK_WIDGET (l->data);
      GObject *object;

      object = G_OBJECT (g_object_get_data (G_OBJECT (child), "item"));
      if ((gpointer) object == (gpointer) item)
        {
          gtk_container_remove (GTK_CONTAINER (self), child);
          break;
        }
    }

  g_list_free (children);
}


static void
photos_flow_box_clear (PhotosFlowBox *self)
{
  GList *children = NULL;
  GList *l;

  children = gtk_container_get_children (GTK_CONTAINER (self));
  for (l = children; l != NULL; l = l->next)
    {
      GtkWidget *child = GTK_WIDGET (l->data);
      GObject *object;

      object = G_OBJECT (g_object_get_data (G_OBJECT (child), "item"));
      g_object_set_data (object, self->priv->is_present_key, NULL);
      gtk_container_remove (GTK_CONTAINER (self), child);
    }

  g_list_free (children);
}


static void
photos_flow_box_info_updated (PhotosBaseItem *item, gpointer user_data)
{
  PhotosFlowBox *self = PHOTOS_FLOW_BOX (user_data);
  PhotosFlowBoxPrivate *priv = self->priv;
  GObject *collection;
  gboolean is_collection;
  gboolean is_favorite;
  gboolean is_present;

  collection = photos_base_manager_get_active_object (priv->col_mngr);
  is_collection = photos_base_item_is_collection (item);
  is_favorite = photos_base_item_is_favorite (item);
  is_present = (g_object_get_data (G_OBJECT (item), priv->is_present_key) != NULL);

  if (priv->mode == PHOTOS_WINDOW_MODE_COLLECTIONS)
    {
      if (!is_collection && is_present && collection == NULL)
        photos_flow_box_remove_item (self, item);
      else if (is_collection && !is_present)
        photos_flow_box_add_item (self, item);
    }
  else if (priv->mode == PHOTOS_WINDOW_MODE_FAVORITES)
    {
      if (!is_favorite && is_present && collection == NULL)
        photos_flow_box_remove_item (self, item);
      else if (is_favorite && !is_present && collection == NULL)
        photos_flow_box_add_item (self, item);
    }
  else if (priv->mode == PHOTOS_WINDOW_MODE_OVERVIEW)
    {
      if (is_collection && is_present)
        photos_flow_box_remove_item (self, item);
      else if (!is_collection && !is_present)
        photos_flow_box_add_item (self, item);
    }
}


static void
photos_flow_box_object_added (PhotosFlowBox *self, GObject *object)
{
  PhotosBaseItem *item = PHOTOS_BASE_ITEM (object);
  PhotosFlowBoxPrivate *priv = self->priv;
  GObject *collection;
  gboolean is_collection;
  gboolean is_favorite;

  collection = photos_base_manager_get_active_object (priv->col_mngr);
  is_collection = photos_base_item_is_collection (item);
  is_favorite = photos_base_item_is_favorite (item);

  if (collection == NULL)
    {
      if ((priv->mode == PHOTOS_WINDOW_MODE_COLLECTIONS && !is_collection)
          || (priv->mode == PHOTOS_WINDOW_MODE_FAVORITES && !is_favorite)
          || (priv->mode == PHOTOS_WINDOW_MODE_OVERVIEW && is_collection))
        goto out;
    }

  photos_flow_box_add_item (self, item);

 out:
  g_signal_connect (item, "info-updated", G_CALLBACK (photos_flow_box_info_updated), self);
}


static void
photos_flow_box_object_removed (PhotosFlowBox *self, GObject *object)
{
  photos_flow_box_remove_item (self, PHOTOS_BASE_ITEM (object));
}


static gint
photos_flow_box_sort_func (GtkFlowBoxChild *child1, GtkFlowBoxChild *child2, gpointer user_data)
{
  PhotosBaseItem *item;
  gint64 mtime1;
  gint64 mtime2;

  item = PHOTOS_BASE_ITEM (g_object_get_data (G_OBJECT (child1), "item"));
  mtime1 = photos_base_item_get_mtime (item);

  item = PHOTOS_BASE_ITEM (g_object_get_data (G_OBJECT (child2), "item"));
  mtime2 = photos_base_item_get_mtime (item);

  return mtime2 - mtime1; /* descending order */
}


static void
photos_flow_box_constructed (GObject *object)
{
  PhotosFlowBox *self = PHOTOS_FLOW_BOX (object);
  GHashTable *items_hash;
  GList *items;
  GList *l;

  G_OBJECT_CLASS (photos_flow_box_parent_class)->constructed (object);

  items_hash = photos_base_manager_get_objects (self->priv->item_mngr);
  items = g_hash_table_get_values (items_hash);
  for (l = items; l != NULL; l = l->next)
    {
      GObject *object = G_OBJECT (l->data);
      photos_flow_box_object_added (self, object);
    }

  g_list_free (items);
}


static void
photos_flow_box_dispose (GObject *object)
{
  PhotosFlowBox *self = PHOTOS_FLOW_BOX (object);
  PhotosFlowBoxPrivate *priv = self->priv;

  g_clear_object (&priv->model);
  g_clear_object (&priv->col_mngr);
  g_clear_object (&priv->item_mngr);

  G_OBJECT_CLASS (photos_flow_box_parent_class)->dispose (object);
}


static void
photos_flow_box_finalize (GObject *object)
{
  PhotosFlowBox *self = PHOTOS_FLOW_BOX (object);

  g_free (self->priv->is_present_key);

  G_OBJECT_CLASS (photos_flow_box_parent_class)->finalize (object);
}


static void
photos_flow_box_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  PhotosFlowBox *self = PHOTOS_FLOW_BOX (object);
  PhotosFlowBoxPrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_MODE:
      priv->mode = (PhotosWindowMode) g_value_get_enum (value);
      priv->is_present_key = g_strdup_printf ("is-present-%d", priv->mode);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
photos_flow_box_init (PhotosFlowBox *self)
{
  PhotosFlowBoxPrivate *priv;

  self->priv = photos_flow_box_get_instance_private (self);
  priv = self->priv;

  gtk_flow_box_set_sort_func (GTK_FLOW_BOX (self), photos_flow_box_sort_func, NULL, NULL);

  priv->col_mngr = photos_collection_manager_dup_singleton ();

  priv->item_mngr = photos_item_manager_dup_singleton ();
  g_signal_connect_swapped (priv->item_mngr, "object-added", G_CALLBACK (photos_flow_box_object_added), self);
  g_signal_connect_swapped (priv->item_mngr, "object-removed", G_CALLBACK (photos_flow_box_object_removed), self);
  g_signal_connect_swapped (priv->item_mngr, "clear", G_CALLBACK (photos_flow_box_clear), self);
}


static void
photos_flow_box_class_init (PhotosFlowBoxClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->constructed = photos_flow_box_constructed;
  object_class->dispose = photos_flow_box_dispose;
  object_class->finalize = photos_flow_box_finalize;
  object_class->set_property = photos_flow_box_set_property;
  /* widget_class->configure_event = photos_flow_box_configure_event; */
  /* widget_class->delete_event = photos_flow_box_delete_event; */
  /* widget_class->key_press_event = photos_flow_box_key_press_event; */

  g_object_class_install_property (object_class,
                                   PROP_MODE,
                                   g_param_spec_enum ("mode",
                                                      "PhotosWindowMode enum",
                                                      "The mode for the flow box",
                                                      PHOTOS_TYPE_WINDOW_MODE,
                                                      PHOTOS_WINDOW_MODE_NONE,
                                                      G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));
}


GtkWidget *
photos_flow_box_new (PhotosWindowMode mode)
{
  return g_object_new (PHOTOS_TYPE_FLOW_BOX,
                       "homogeneous", TRUE,
                       "min-children-per-line", 2,
                       "mode", mode,
                       NULL);
}
