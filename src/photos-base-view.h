/*
 * Photos - access, organize and share your photos on GNOME
 * Copyright © 2014 Red Hat, Inc.
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

#ifndef PHOTOS_BASE_VIEW_H
#define PHOTOS_BASE_VIEW_H

#include <gtk/gtk.h>

#include "photos-base-manager.h"

G_BEGIN_DECLS

#define PHOTOS_TYPE_BASE_VIEW (photos_base_view_get_type ())

#define PHOTOS_BASE_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   PHOTOS_TYPE_BASE_VIEW, PhotosBaseView))

#define PHOTOS_BASE_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   PHOTOS_TYPE_BASE_VIEW, PhotosBaseViewClass))

#define PHOTOS_IS_BASE_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   PHOTOS_TYPE_BASE_VIEW))

#define PHOTOS_IS_BASE_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   PHOTOS_TYPE_BASE_VIEW))

#define PHOTOS_BASE_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   PHOTOS_TYPE_BASE_VIEW, PhotosBaseViewClass))

typedef struct _PhotosBaseView        PhotosBaseView;
typedef struct _PhotosBaseViewClass   PhotosBaseViewClass;
typedef struct _PhotosBaseViewPrivate PhotosBaseViewPrivate;

struct _PhotosBaseView
{
  GtkTreeView parent_instance;
  PhotosBaseViewPrivate *priv;
};

struct _PhotosBaseViewClass
{
  GtkTreeViewClass parent_class;

  /* signals */
  void (*item_activated) (PhotosBaseView *self);
};

GType             photos_base_view_get_type               (void) G_GNUC_CONST;

GtkWidget        *photos_base_view_new                    (PhotosBaseManager *mngr);

G_END_DECLS

#endif /* PHOTOS_BASE_VIEW_H */
