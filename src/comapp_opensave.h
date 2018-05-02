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


#ifndef __COMAPP_OPENSAVE_H__
#define __COMAPP_OPENSAVE_H__

#include <libosso.h>
#include <hildon/hildon-file-system-model.h>
#include <gtk/gtk.h>



#define FILE_URI_PREFIX                 "file://"
#define EXTERNAL_MMC                "/system/osso/af/mmc-mount-point"
#define EXTERNAL_MMC_LOCATION       "/media/mmc1"
#define EXTERNAL_MMC_COVEROPEN      "/system/osso/af/mmc-cover-open"
#define EXTERNAL_MMC_PRESENT        "/system/osso/af/mmc-device-present"
#define INTERNAL_MMC_LOCATION       "/media/mmc2"
#define INTERNAL_MMC_PRESENT        "/system/osso/af/internal-mmc-device-present"



G_BEGIN_DECLS typedef enum {
  /** something was selected */
    COMAPP_FILE_CHOOSER_SELECTED,
  /** cancel pressed */
    COMAPP_FILE_CHOOSER_CANCEL,
  /** error */
    COMAPP_FILE_CHOOSER_ERROR
} comapp_fc_result;


typedef struct _ComappOpenSave ComappOpenSave;

struct _ComappOpenSave {
  /** parent widget*/
    GtkWidget *parent;
  /** GTK_FILE_CHOOSER_ACTION_OPEN or GTK_FILE_CHOOSER_ACTION_SAVE */
    GtkFileChooserAction action;
  /** the filesystem model if it is cached, or NULL. The comapp_opensave_new will fill this, if this was NULL (should be unref-ed, if not needed) */
    HildonFileSystemModel *model;
  /** the title for the dialog in case of open */
    gchar *open_window_title;
  /** the title of the dialog in case of save */
    gchar *save_window_title;
  /** in open case, what should be the label, if no files are in the selected directory */
    gchar *empty_text;
  /** osso content, can be NULL, needed for the help */
    osso_context_t *osso;
  /** the help chapter title for the open dialog or NULL */
    gchar *open_help_title;
  /** the help chapter title for the save dialog or NULL */
    gchar *save_help_title;
  /** NULL terminated list if gchar*. The list of the mime-types */
    gchar **mime_types;
  /** the uri to the current file, or to the default file */
    gchar *current_uri;
  /** the uri of the selected file will be here. should be freed if not needed. */
    gchar *result_uri;
  /** the created dialog */
    GtkWidget *fc_dialog;
};

/**
 * Creates a new open/save dialog.
 *
 * @data The filled data structure
 * @return the passed structure with additional fields filled
 */
ComappOpenSave *comapp_opensave_new(ComappOpenSave * data);

/**
 * Runs the created open/save dialog.
 *
 * @data The filled data structure
 * @return COMAPP_FILE_CHOOSER_SELECTED if something was selected, or COMAPP_FILE_CHOOSER_CANCEL if cancel was pressed
 */
comapp_fc_result comapp_opensave_run(ComappOpenSave * data);

/**
 * Stops the created running open/save dialog. Need to be called, when the dialog should be closed.
 *
 * @data The filled data structure
 */
void comapp_opensave_response(ComappOpenSave * data);

G_END_DECLS
#endif /* __COMAPP_OPENSAVE_H__ */
