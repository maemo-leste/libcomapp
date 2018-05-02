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


#include "comapp_sendvia.h"
#include "comapp_i18n.h"
#include <dbus/dbus-glib.h>
#include <conbtdialogs-dbus.h>
#include <libmodest-dbus-client/libmodest-dbus-client.h>
#include <hildon/hildon-banner.h>
#include <hildon/hildon-helper.h> 
#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>

#define MSG_CAN_NOT_SEND_EMAIL "note_ms_cannot_email"
#define MSG_SEND "note_me_menu_note_send"
#define MSG_SEND_BT "note_me_menu_note_send_as_bluetooth"
#define MSG_SEND_EMAIL "note_me_menu_note_send_as_email"

static gboolean comapp_sendvia_menu_item_destroyed( GtkWidget * widget,
	GdkEvent * event, gpointer user_data );
static GtkMenuItem * compapp_sendvia_get_menu_item( ComappSendviaMenu * menu_data,
	const gchar * item );


gboolean
comapp_sendvia_bluetooth(gchar * path, GtkWidget * parent_widget)
{
    DBusGProxy *proxy = NULL;
    DBusGConnection *sys_conn = NULL;
    GError *error = NULL;
    gboolean result = FALSE;
    gchar **files = NULL;

    sys_conn = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
    if (sys_conn != NULL) {
        files = g_new0(gchar *, 2);
        if (files != NULL) {
            *files = g_strdup(path);
            files[1] = NULL;
            /* Open connection for btdialogs service */
            proxy = dbus_g_proxy_new_for_name(sys_conn,
                                              CONBTDIALOGS_DBUS_SERVICE,
                                              CONBTDIALOGS_DBUS_PATH,
                                              CONBTDIALOGS_DBUS_INTERFACE);
            if (proxy != NULL) {
                /* Send send file request to btdialogs service */
                if (!dbus_g_proxy_call(proxy, CONBTDIALOGS_SEND_FILE_REQ,
                                       &error, G_TYPE_STRV, files,
                                       G_TYPE_INVALID, G_TYPE_INVALID))
                {
                    g_warning("Error: %s\n", error->message);
                    g_clear_error(&error);
                } else {
                    result = TRUE;
                }
                g_object_unref(G_OBJECT(proxy));
            }
            g_strfreev(files);
        }
    }

    if (result == FALSE) {
        hildon_banner_show_information(GTK_WIDGET(parent_widget), NULL,
                                       dgettext("hildon-common-strings","sfil_ni_operation_failed"));
    }
    return result;
}

gboolean
comapp_sendvia_email(GtkWidget * parent_widget, osso_context_t * osso,
                     const gchar * path)
{
    gboolean ret = FALSE;
    GSList *list = NULL;
    gchar *uri = NULL;


    uri = g_strdup(path);
    list = g_slist_append(list, uri);

    ret = libmodest_dbus_client_compose_mail(osso, NULL /* to */,
                NULL /* cc */, NULL /* bcc */, NULL /* subject */,
                NULL /* body */, list /* attachments */);

    g_slist_free(list);
    g_free(uri);

    if (!ret)
    {
        hildon_banner_show_information(GTK_WIDGET(parent_widget), NULL,
                                       _(MSG_CAN_NOT_SEND_EMAIL));
    }

    return ret;
}

static void
comapp_sendvia_email_selected(GtkMenuItem * menuitem, gpointer user_data)
{
    ComappSendviaData *sendvia_data = (ComappSendviaData *) user_data;
    gchar *file_uri = NULL;
    gboolean retval = FALSE;
    file_uri = sendvia_data->save_file_cb(sendvia_data->user_data);
    if (file_uri)
    {
        retval =
            comapp_sendvia_email(sendvia_data->parent_widget,
                                 sendvia_data->osso,
                                 (const gchar *) file_uri);
        if (!retval)
        {
            g_warning("Unable to send via e-mail: %s", file_uri);
        }
        g_free(file_uri);
    }
}

static void
comapp_sendvia_bt_selected(GtkMenuItem * menuitem, gpointer user_data)
{
    ComappSendviaData *sendvia_data = (ComappSendviaData *) user_data;
    gchar *file_uri = NULL;
    gboolean retval = FALSE;
    file_uri = sendvia_data->save_file_cb(sendvia_data->user_data);
    if (file_uri)
    {
        retval = comapp_sendvia_bluetooth(file_uri,sendvia_data->parent_widget);
        if (!retval)
        {
            g_warning("Unable to send via Bluetooth: %s", file_uri);
        }
        g_free(file_uri);
    }
}

GtkWidget *
comapp_sendvia_create_send_bt(ComappSendviaData * sendvia_data)
{
    GtkMenuItem *send_bt = NULL;
    send_bt = GTK_MENU_ITEM(gtk_menu_item_new_with_label(_(MSG_SEND_BT)));
    g_signal_connect(G_OBJECT(send_bt), "activate",
                     G_CALLBACK(comapp_sendvia_bt_selected), sendvia_data);
    return GTK_WIDGET(send_bt);
}

GtkWidget *
comapp_sendvia_create_send_mail(ComappSendviaData * sendvia_data)
{
    GtkMenuItem *send_email = NULL;
    send_email =
        GTK_MENU_ITEM(gtk_menu_item_new_with_label(_(MSG_SEND_EMAIL)));
    g_signal_connect(G_OBJECT(send_email), "activate",
                     G_CALLBACK(comapp_sendvia_email_selected), sendvia_data);
    return GTK_WIDGET(send_email);
}

GtkWidget *
comapp_sendvia_create_main_menu(comapp_save_file_cb * save_cb,
                                GtkWidget * parent_widget,
                                osso_context_t * osso, gpointer user_data)
{

	return comapp_sendvia_create_menu_item( save_cb, parent_widget, osso, NULL,
		user_data );
}

GtkWidget *
comapp_sendvia_create_menu_item(comapp_save_file_cb * save_cb,
                                GtkWidget * parent_widget,
                                osso_context_t * osso,
                                ComappSendviaMenu ** menu_data,
                                gpointer user_data)
{
    GtkMenuItem * send_menu_item;
    ComappSendviaMenu * ret;
    
    g_assert( save_cb != NULL && osso != NULL );

    send_menu_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label(_(MSG_SEND)));

	ret = comapp_sendvia_connect_menu_to_item( send_menu_item, save_cb,
		parent_widget, osso, user_data );

	/* Set pointer to pointer */		
	if( menu_data != NULL ) {
		*menu_data = ret;
	}

    return GTK_WIDGET(send_menu_item);
}

ComappSendviaMenu *comapp_sendvia_connect_menu_to_item( GtkMenuItem * send_menu_item,
														comapp_save_file_cb * save_cb,
														GtkWidget * parent_widget,
														osso_context_t * osso,
														gpointer user_data) {
    GtkMenu * send_menu;
    ComappSendviaData * sendvia_data;

	g_assert( send_menu_item != NULL && save_cb != NULL && osso != NULL );

    sendvia_data = g_new0(ComappSendviaData, 1);

    sendvia_data->user_data = user_data;
    sendvia_data->save_file_cb = save_cb;
    sendvia_data->osso = osso;
    sendvia_data->parent_widget = parent_widget;

    sendvia_data->menu.email_item = GTK_MENU_ITEM(comapp_sendvia_create_send_mail(sendvia_data));
    sendvia_data->menu.bluetooth_item = GTK_MENU_ITEM(comapp_sendvia_create_send_bt(sendvia_data));

    send_menu = GTK_MENU(gtk_menu_new());

    gtk_menu_item_set_submenu(send_menu_item, GTK_WIDGET(send_menu));
    gtk_menu_append(send_menu, GTK_WIDGET( sendvia_data->menu.email_item ));
    gtk_menu_append(send_menu, GTK_WIDGET( sendvia_data->menu.bluetooth_item ));
            
    /* Connect delete event signal (to free memory alloced here) */
    g_signal_connect( G_OBJECT( send_menu ), "delete_event",
    	G_CALLBACK( comapp_sendvia_menu_item_destroyed ), sendvia_data );
    	
    /* Show menu and it's all widgets */
    gtk_widget_show_all( GTK_WIDGET( send_menu ) );

    return &( sendvia_data->menu );
													
}


static gboolean comapp_sendvia_menu_item_destroyed( GtkWidget * widget,
	GdkEvent * event, gpointer user_data ) {

	ComappSendviaData * sendvia_data = ( ComappSendviaData * ) user_data;
	
	/* Free Sendvia data as it's not used anymore */
	g_free( sendvia_data );
	
	/* Let menu to be destroyed */
	return FALSE;
}

void compapp_sendvia_set_all_menu_items_sensitive( ComappSendviaMenu * menu_data,
                                                 gboolean sensitive ) {
	g_assert( menu_data != NULL );
	if( menu_data->email_item == NULL || menu_data->bluetooth_item == NULL ) {
		g_warning( "%s: Null pointers in Sendvia menu data", __FUNCTION__ );
	}
	
	gtk_widget_set_sensitive( GTK_WIDGET( menu_data->email_item ), sensitive );
	gtk_widget_set_sensitive( GTK_WIDGET( menu_data->bluetooth_item ), sensitive );	
                                                 
}

void compapp_sendvia_set_menu_item_sensitive( ComappSendviaMenu * menu_data,
	const gchar * item, gboolean sensitive ) {
	
	GtkWidget * widget;
	g_assert( menu_data != NULL && item != NULL );
	
	widget = GTK_WIDGET( compapp_sendvia_get_menu_item( menu_data,
		item ) );	
	
	if( widget != NULL ) {
		gtk_widget_set_sensitive( widget, sensitive );	
	}
		
}      

void compapp_sendvia_set_insensitive_message( ComappSendviaMenu * menu_data,
	const gchar * item, const gchar * message ) {
	
	GtkWidget * widget;
	g_assert( menu_data != NULL && item != NULL && message != NULL );
	
	widget = GTK_WIDGET( compapp_sendvia_get_menu_item( menu_data,
		item ) );
		
	if( widget != NULL ) {
		hildon_helper_set_insensitive_message( widget, message );	
	}
}  

static GtkMenuItem * compapp_sendvia_get_menu_item( ComappSendviaMenu * menu_data,
	const gchar * item ) {
	
	if( strcmp( item, COMAPP_SENDVIA_EMAIL_ITEM ) == 0 ) {
		return menu_data->email_item;
	} else if( strcmp( item, COMAPP_SENDVIA_BLUETOOTH_ITEM ) == 0 ) {
		return menu_data->bluetooth_item;
	} else {
		g_warning( "%s: Invalid item string '%s'", __FUNCTION__, item );
		return NULL;
	}
}                                 

