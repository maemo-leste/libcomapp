/* LibComApp
 *
 * Copyright (C) 2007 Nokia Corporation. 
 *
 * Contact: Ahmad Qureshi <Ahmad.Qureshi@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */


#ifndef __COMAPP_DETAILS_H__
#define __COMAPP_DETAILS_H__


#include <glib.h>
#include <glib-object.h>
#include <libosso.h>
#include <hildon/hildon-file-details-dialog.h>
#include <hildon/hildon-file-system-model.h>


G_BEGIN_DECLS typedef struct _ComappDetailItem ComappDetailItem;
typedef struct _ComappDetailsData ComappDetailsData;

struct _ComappDetailItem {
  /** name of the field */
    gchar *name;
  /** field value */
    gchar *content;
};

struct _ComappDetailsData {
  /** list of ComappDetailItem */
    GSList *details_data;
  /** title of the window. Can be NULL */
    gchar *window_title;
  /** title of the label. Can be NULL */
    gchar *label_title;
  /** help title, or NULL */
    gchar *help_title;
  /** OSSO context. Needed for help_title. */
    osso_context_t *osso;
  /** cached FileSystemModel or NULL. If this is NULL, this will be filled*/
    HildonFileSystemModel *fs_model;
  /** parent widget. */
    GtkWidget *parent;
  /** uri of the file */
    gchar *uri;
};

/**
 * Creates a new details dialog according to the settings in the ddata
 *
 * @ddata Filled details Data structure
 * @return new details dialog
 */
GtkWidget *comapp_details_new(ComappDetailsData * ddata);


G_END_DECLS
#endif /* __COMAPP_DETAILS_H__ */
