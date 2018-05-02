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


#include "comapp_i18n.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <hildon/hildon-defines.h>
#include <hildon/hildon-banner.h>
#include "comapp_details.h"
#include "comapp_common.h"

#define MSG_NOT_AVAILABE "note_ib_not_available"




GtkWidget *
comapp_details_new(ComappDetailsData * ddata)
{
    GSList *details = NULL;
    ComappDetailItem *detail_item = NULL;
    GtkTreeIter iter;
    gboolean iter_valid = TRUE;
    GtkWidget *details_dialog = NULL;

    g_assert(ddata);
    g_assert(ddata->details_data);
    details = ddata->details_data;

 	/* made changes to fix bug 94597*/
    if (!ddata->fs_model)
    {
        ddata->fs_model = comapp_common_new_fs_model(ddata->parent);
    }
    else
    {
        if (ddata->parent)
        {
            comapp_common_fs_model_set_ref_widget(ddata->fs_model,
                                                  ddata->parent);
        }
    }

    g_assert(ddata->uri);
    /* load the uri and set the GtkTreeIter */
    if (!hildon_file_system_model_load_uri
        (ddata->fs_model, ddata->uri, &iter))
    {
        hildon_banner_show_information(GTK_WIDGET(ddata->parent), NULL,
                                       _(MSG_NOT_AVAILABE));
        iter_valid = FALSE;
    }
    
    g_assert(ddata->parent);
    g_assert(ddata->fs_model);
    /* create file details dialog */
    details_dialog = hildon_file_details_dialog_new_with_model
        (GTK_WINDOW(ddata->parent), ddata->fs_model);

/* add additional details info */    

    if (g_slist_length(details))
    {
        while (details)
	{
	            detail_item = (ComappDetailItem *) details->data;
		    hildon_file_details_dialog_add_label_with_value(HILDON_FILE_DETAILS_DIALOG(details_dialog), 
						    detail_item->name,
						    detail_item->content);
		            details = g_slist_next(details);
        }
     }
    
    gtk_window_set_modal(GTK_WINDOW(details_dialog), TRUE);
    if (iter_valid)
    {
        hildon_file_details_dialog_set_file_iter
            (HILDON_FILE_DETAILS_DIALOG(details_dialog), &iter);
    }

    return details_dialog;
}
