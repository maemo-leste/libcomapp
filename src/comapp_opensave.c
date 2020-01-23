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


#include "comapp_opensave.h"
#include "comapp_common.h"
#include "comapp_i18n.h"
#include <string.h>
#include <gio/gio.h>
#include <hildon/hildon-helper.h>
#include <hildon/hildon-file-chooser-dialog.h>
#include <hildon/hildon-file-selection.h>
#include <hildon/hildon-banner.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>


/* Forward declarations for static functions */
static gboolean comapp_opensave_check_write_access(ComappOpenSave * data);
static gboolean comapp_opensave_check_location_mounted(ComappOpenSave * data);


static int
hex_to_int(gchar c)
{
    return  c >= '0' && c <= '9' ? c - '0'
        : c >= 'A' && c <= 'F' ? c - 'A' + 10
        : c >= 'a' && c <= 'f' ? c - 'a' + 10
        : -1;
}

/*
 * This is based on gnome_vfs_unescape_string_for_display(), which
 * is deprecated due to the replacement of gnomevfs -> gvfs.
 */
static gchar *
unescape_string_for_display(const gchar * escaped)
{
    const gchar *in, *start_escape;
    gchar *out, *result;
    gint i,j;
    gchar c;
    gint invalid_escape;

    if (escaped == NULL) {
        return NULL;
    }

    result = g_malloc(strlen(escaped) + 1);

    out = result;
    for (in = escaped; *in != '\0'; ) {
        start_escape = in;
        c = *in++;
        invalid_escape = 0;

        if (c == '%') {
            /* Get the first hex digit. */
            i = hex_to_int(*in++);
            if (i < 0) {
                invalid_escape = 1;
                in--;
            }
            c = i << 4;

            if (invalid_escape == 0) {
                /* Get the second hex digit. */
                i = hex_to_int(*in++);
                if (i < 0) {
                    invalid_escape = 2;
                    in--;
                }
                c |= i;
            }
            if (invalid_escape == 0) {
                /* Check for an illegal character. */
                if (c == '\0') {
                    invalid_escape = 3;
                }
            }
        }
        if (invalid_escape != 0) {
            for (j = 0; j < invalid_escape; j++) {
                *out++ = *start_escape++;
            }
        } else {
            *out++ = c;
        }
    }

    *out = '\0';
    g_assert(out - result <= strlen(escaped));
    return result;
}

ComappOpenSave *
comapp_opensave_new(ComappOpenSave * data)
{
    GtkWidget *fc_dialog = NULL;
    gchar *name = NULL, *dir_uri = NULL, *escaped_name = NULL;

    g_assert(data);

    if (!data->model)
    {
        data->model = comapp_common_new_fs_model(data->parent);
    }
    else
    {
        if (data->parent)
        {
            comapp_common_fs_model_set_ref_widget(data->model, data->parent);
        }
    }
    fc_dialog = hildon_file_chooser_dialog_new_with_properties
        (GTK_WINDOW(data->parent),
         "file_system_model", data->model, "action", data->action, NULL);

    gtk_window_set_destroy_with_parent(GTK_WINDOW(fc_dialog), TRUE);

    gtk_window_set_title(GTK_WINDOW(fc_dialog),
                         data->action == GTK_FILE_CHOOSER_ACTION_OPEN ?
                         data->open_window_title : data->save_window_title);

    if (data->empty_text)
    {
        g_object_set(G_OBJECT(fc_dialog),
                     "empty-text", data->empty_text, NULL);
    }

    gtk_window_set_modal(GTK_WINDOW(fc_dialog), TRUE);
    if (data->mime_types)
    {
        GtkFileFilter *filter = g_object_ref_sink(gtk_file_filter_new());
	//GtkFileFilter *filter = gtk_file_filter_new();
        gint i = 0;
        while (data->mime_types[i])
        {
            gtk_file_filter_add_mime_type(filter,
                                          (gchar *) data->mime_types[i]);
            i++;
        }
        if (HILDON_IS_FILE_CHOOSER_DIALOG(fc_dialog))
        {
            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc_dialog), filter);
            gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fc_dialog), filter);
        }
	/* reducing the reference count*/
	g_object_unref(filter);
    }
    if(!g_file_test(data->current_uri,G_FILE_TEST_IS_DIR))
    {
	    name = g_strrstr(data->current_uri, "/");
    }
    
    if (name)
    {
        name[0] = 0;
        name++;
    }
    else
    {
        name = data->current_uri;
    }
    dir_uri = data->current_uri;
    if (HILDON_IS_FILE_SELECTION(fc_dialog) ||
            HILDON_IS_FILE_CHOOSER_DIALOG(fc_dialog))
    {
        gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(fc_dialog),
                                                dir_uri);
    }
    escaped_name = unescape_string_for_display(name);
    if (HILDON_IS_FILE_CHOOSER_DIALOG(fc_dialog))
    {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fc_dialog),
                                          escaped_name);
    }
    g_free(escaped_name);
    data->fc_dialog = fc_dialog;
    return data;

}

comapp_fc_result
comapp_opensave_run(ComappOpenSave * data)
{
    gint status = -1;

    g_assert(data);
    g_assert(data->fc_dialog);

    
    while( TRUE ) {
        if (!HILDON_IS_FILE_CHOOSER_DIALOG(data->fc_dialog))
        {
            return COMAPP_FILE_CHOOSER_ERROR;
        }

		status = gtk_dialog_run(GTK_DIALOG(data->fc_dialog));
    
	    if( status == GTK_RESPONSE_OK ) {
        	data->result_uri =
            	gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(data->fc_dialog));
            
        	/* Don't allow to save to target without write access */
        	if( data->action == GTK_FILE_CHOOSER_ACTION_SAVE ) {
        		if (!comapp_opensave_check_location_mounted(data)) {
    		        hildon_banner_show_information( GTK_WIDGET( data->parent ),
    		        	NULL, dgettext ("hildon-fm", "sfil_nc_not_available_save_local" ));
    		        g_free( data->result_uri );
	    	        data->result_uri = NULL;
        		    continue;
                }
        		if (!comapp_opensave_check_write_access(data)) {
    		        hildon_banner_show_information( GTK_WIDGET( data->parent ),
    		        	NULL, dgettext ("hildon-fm","sfil_ib_readonly_location"));
    		        g_free( data->result_uri );
	    	        data->result_uri = NULL;
        		    continue;
                }
        	}
            
        	gtk_widget_destroy(data->fc_dialog);
        	data->fc_dialog = NULL;
    		if( data->result_uri == NULL)
			return COMAPP_FILE_CHOOSER_ERROR;
		else
	        	return COMAPP_FILE_CHOOSER_SELECTED;
    	} else {

	        gtk_widget_destroy(data->fc_dialog);
	        data->fc_dialog = NULL;
	        return COMAPP_FILE_CHOOSER_CANCEL;
	    }
	    
	}
}


/**
  Check if file location get from dialog is mounted
  @param data the filled data structure
  @return TRUE if folder is mounted and FALSE if not
*/

static gboolean
comapp_opensave_check_location_mounted(ComappOpenSave * data)
{
    gboolean retval = TRUE;
    gchar * folder = g_path_get_dirname( data->result_uri );
    gchar * mmcprefix  = NULL;
    gchar * oldmmcprefix = NULL;
    GConfClient *gc_client = NULL;
#if !GLIB_CHECK_VERSION (2,35,0)
    g_type_init();
#endif
    gc_client = gconf_client_get_default();

    /* If the target is not on the mmc card, return TRUE */
    if (!g_str_has_prefix(folder,INTERNAL_MMC_LOCATION) &&
        !g_str_has_prefix(folder,EXTERNAL_MMC_LOCATION)) {
	g_free(folder);
        return TRUE;
    }

    if (gc_client != NULL) {

        if ( gconf_client_get_bool(gc_client,EXTERNAL_MMC_COVEROPEN,NULL) ) {
            retval = FALSE;
        } else {
            if (g_str_has_prefix(folder,INTERNAL_MMC_LOCATION)) {
                retval = gconf_client_get_bool(gc_client,INTERNAL_MMC_PRESENT,NULL);
            } else {
                mmcprefix = gconf_client_get_string(gc_client,EXTERNAL_MMC,NULL);
                if (mmcprefix != NULL) {
                    if (g_str_has_prefix(folder,FILE_URI_PREFIX)) {
                        oldmmcprefix = mmcprefix;
                        mmcprefix = g_strconcat(FILE_URI_PREFIX,oldmmcprefix,NULL);
                        g_free(oldmmcprefix);
                        oldmmcprefix = NULL;
                    }
    
                    if (g_str_has_prefix(folder,mmcprefix) == TRUE) {
                        retval = gconf_client_get_bool(gc_client,EXTERNAL_MMC_PRESENT,NULL);
                    } 
                    g_free(mmcprefix);
                    mmcprefix = NULL;
                }
            }
            g_object_unref(gc_client);
        }
    }
    g_free(folder);
    return retval;
}


/**
  Check if file get from dialog has write access
  @param data the filled data structure
  @return TRUE if folder has write access and FALSE if not
*/

static gboolean
comapp_opensave_check_write_access(ComappOpenSave * data)
{
    gchar *folder = g_path_get_dirname(data->result_uri);
    GFile *g_folder = g_file_new_for_path(folder);
    gboolean write_acc = FALSE;

    GFileInfo *folder_info = g_file_query_info(g_folder,
                                              G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE,
                                              G_FILE_QUERY_INFO_NONE,
                                              NULL, NULL);

    if (folder_info) {
        write_acc = g_file_info_get_attribute_boolean(folder_info,
                                              G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
        g_object_unref(folder_info);
    }

    g_object_unref(g_folder);
    g_free(folder);

    return write_acc;
}

void
comapp_opensave_response(ComappOpenSave * data)
{
    g_assert(data);
    g_assert(data->fc_dialog);
    /* Destroy dialog, don't send a response...
    gtk_dialog_response(GTK_DIALOG(data->fc_dialog), GTK_RESPONSE_CANCEL); */
    gtk_widget_destroy( GTK_WIDGET( data->fc_dialog ) );
}
