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

#ifndef PHOTOS_FLOW_BOX_H
#define PHOTOS_FLOW_BOX_H

#include <gtk/gtk.h>

#include "photos-mode-controller.h"

G_BEGIN_DECLS

#define PHOTOS_TYPE_FLOW_BOX (photos_flow_box_get_type ())

#define PHOTOS_FLOW_BOX(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   PHOTOS_TYPE_FLOW_BOX, PhotosFlowBox))

#define PHOTOS_FLOW_BOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   PHOTOS_TYPE_FLOW_BOX, PhotosFlowBoxClass))

#define PHOTOS_IS_FLOW_BOX(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   PHOTOS_TYPE_FLOW_BOX))

#define PHOTOS_IS_FLOW_BOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   PHOTOS_TYPE_FLOW_BOX))

#define PHOTOS_FLOW_BOX_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   PHOTOS_TYPE_FLOW_BOX, PhotosFlowBoxClass))

typedef struct _PhotosFlowBox        PhotosFlowBox;
typedef struct _PhotosFlowBoxClass   PhotosFlowBoxClass;
typedef struct _PhotosFlowBoxPrivate PhotosFlowBoxPrivate;

struct _PhotosFlowBox
{
  GtkFlowBox parent_instance;
  PhotosFlowBoxPrivate *priv;
};

struct _PhotosFlowBoxClass
{
  GtkFlowBoxClass parent_class;
};

GType                  photos_flow_box_get_type               (void) G_GNUC_CONST;

GtkWidget             *photos_flow_box_new                    (PhotosWindowMode mode);

G_END_DECLS

#endif /* PHOTOS_FLOW_BOX_H */
