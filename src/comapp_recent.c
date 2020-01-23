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




#include <gtk/gtksignal.h>
#include <gtk/gtktable.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkrecentchoosermenu.h>
#include <gtk/gtkrecentchooser.h>
#include <gtk/gtkrecentmanager.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkaccellabel.h>
#include <gobject/gmarshal.h>
#include <glib.h>
#include <glib-object.h>
#include <hildon/hildon-helper.h>
#include <string.h>
#include "comapp_i18n.h"
#include "comapp_recent.h"
#include "comapp_recent_private.h"
#include "comapp_common.h"

#define MSG_NOT_AVAILABE "note_ib_not_available"

#define MSG_RECENT "note_me_main_menu_recent"

#define DEF_MAX_ITEMS 6

enum {
    PROP_0,

    PROP_APPNAME,
    PROP_MIMETYPE,
    PROP_EXTENSION,
    PROP_CUT_EXTENSION,
    PROP_FILES_INSENSITIVE
};

enum {
    RECENT_SELECTED_SIGNAL,
    LAST_SIGNAL
};

static void comapp_recent_class_init(ComappRecentClass * klass);
static void comapp_recent_init(ComappRecent * caprecent);
static void comapp_recent_item_activated(GtkRecentChooser * chooser,
                                         gpointer user_data);
static gboolean comapp_recent_menu_expose(GtkWidget * widget,
                                          GdkEventExpose * event,
                                          gpointer user_data);
static void comapp_recent_set_mimetype(ComappRecent * recent_widget,
                                       const gchar * mime_type);
static void comapp_recent_set_extension(ComappRecent * recent_widget,
                                        const gchar ** extension);
static void comapp_recent_set_property(GObject * object, guint prop_id,
                                       const GValue * value,
                                       GParamSpec * pspec);
static void comapp_recent_get_property(GObject * object, guint prop_id,
                                       GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE(ComappRecent, comapp_recent, GTK_TYPE_MENU_ITEM);


static guint comapp_recent_signals[LAST_SIGNAL] = { 0 };

static void
comapp_recent_finalize(GObject * object)
{
    ComappRecent *recent_widget = COMAPP_RECENT(object);
    if (recent_widget->recentprivate->app_name)
    {
        g_free(recent_widget->recentprivate->app_name);
    }
    if (recent_widget->recentprivate->mime_type)
    {
        g_free(recent_widget->recentprivate->mime_type);
    }
    if (recent_widget->recentprivate->extension)
    {
	g_strfreev(recent_widget->recentprivate->extension);
    }
    g_object_unref(G_OBJECT(recent_widget->recentprivate->recent_manager));
    g_object_unref(G_OBJECT(recent_widget->recentprivate->recent_chooser));
    g_free(recent_widget->recentprivate);
    (*G_OBJECT_CLASS(comapp_recent_parent_class)->finalize) (object);
}

static void
comapp_recent_class_init(ComappRecentClass * klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = comapp_recent_set_property;
    gobject_class->get_property = comapp_recent_get_property;
    gobject_class->finalize = comapp_recent_finalize;
  /**
   * ComappRecent::recent_selected
   * @comapp_recent: the recent widget
   * @file_uri: the uri of the file, what was selected
   *
   * Emitted when an item is selected from the list
   *
   */
    comapp_recent_signals[RECENT_SELECTED_SIGNAL] =
        g_signal_new("recent_selected", G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                     G_STRUCT_OFFSET(ComappRecentClass, recent_selected),
                     NULL, NULL, g_cclosure_marshal_VOID__STRING, G_TYPE_NONE,
                     1, G_TYPE_STRING);

  /**
   * ComappRecent:appname
   *
   * The application name, in which the files will be created
   *
   */
    g_object_class_install_property(gobject_class,
                                    PROP_APPNAME,
                                    g_param_spec_string("appname",
                                                        "ApplicationName",
                                                        "The name of the application",
                                                        NULL,
                                                        (G_PARAM_READABLE |
                                                         G_PARAM_WRITABLE)));

  /**
   * ComappRecent:mimetype
   *
   * The mime-type, what the application can handle
   *
   */
    g_object_class_install_property(gobject_class,
                                    PROP_MIMETYPE,
                                    g_param_spec_string("mimetype",
                                                        "Mime-type",
                                                        "The mime-type of the new files",
                                                        NULL,
                                                        (G_PARAM_READABLE |
                                                         G_PARAM_WRITABLE)));

  /**
   * ComappRecent:extension
   *
   * The file-extension of the saved files
   *
   */
    g_object_class_install_property(gobject_class,
                                    PROP_EXTENSION,
                                    g_param_spec_pointer("extension",
                                                         "File-extension",
                                                         "The file-extensions of the files",
                                                         (G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE)));

  /**
   * ComappRecent:cut_extension
   *
   * Whether the file extension should be cutted or not
   *
   */
    g_object_class_install_property(gobject_class,
                                    PROP_CUT_EXTENSION,
                                    g_param_spec_boolean("cut_extension",
                                                         "Cut file-extension",
                                                         "Whether the file extension should be cut, it is not in the list",
                                                         FALSE,
                                                         (G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE)));


  /**
   * ComappRecent:files_insensitive
   *
   * Whether files listed should be shown as insensitive
   *
   */
    g_object_class_install_property(gobject_class,
                                    PROP_FILES_INSENSITIVE,
                                    g_param_spec_boolean("files_insensitive",
                                                         "Files insensitive",
                                                         "Whether files listed should be shown as insensitive",
                                                         FALSE,
                                                         (G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE)));



}

static void
comapp_recent_set_property(GObject * object,
                           guint prop_id,
                           const GValue * value, GParamSpec * pspec)
{
    ComappRecent *recent_widget = COMAPP_RECENT(object);

    switch (prop_id)
    {
        case PROP_APPNAME:
            comapp_recent_set_app_name(recent_widget,
                                       g_value_get_string(value));
            break;
        case PROP_MIMETYPE:
            comapp_recent_set_mimetype(recent_widget,
                                       g_value_get_string(value));
            break;
        case PROP_EXTENSION:
            comapp_recent_set_extension(recent_widget,
                                        g_value_get_pointer(value));
            break;
        case PROP_CUT_EXTENSION:
            comapp_recent_set_cut_extension(recent_widget,
                                            g_value_get_boolean(value));
            break;
        case PROP_FILES_INSENSITIVE:
        	comapp_recent_set_files_insensitive(recent_widget,
        									     g_value_get_boolean(value));
        	break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
comapp_recent_get_property(GObject * object,
                           guint prop_id, GValue * value, GParamSpec * pspec)
{
    ComappRecent *recent_widget = COMAPP_RECENT(object);

    switch (prop_id)
    {
        case PROP_APPNAME:
            g_value_set_string(value, recent_widget->recentprivate->app_name);
            break;
        case PROP_MIMETYPE:
            g_value_set_string(value,
                               recent_widget->recentprivate->mime_type);
            break;
        case PROP_EXTENSION:
            g_value_set_pointer(value,
                                recent_widget->recentprivate->extension);
            break;
        case PROP_CUT_EXTENSION:
            g_value_set_boolean(value,
                                recent_widget->recentprivate->cut_extension);
            break;
        case PROP_FILES_INSENSITIVE:
	        g_value_set_boolean(value,
	        	recent_widget->recentprivate->files_insensitive );
        	break;            
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}



static void
comapp_recent_init(ComappRecent * caprecent)
{
    GtkMenuItem *recent_menu_item = GTK_MENU_ITEM(caprecent);
    GtkWidget *accel_label;
    GtkRecentManager *recent_manager = NULL;
    GtkWidget *recent_chooser = NULL;

    g_assert(caprecent);

    caprecent->recentprivate = g_new0(ComappRecent_Private, 1);

    recent_manager = gtk_recent_manager_get_default();
    caprecent->recentprivate->recent_manager = recent_manager;

    recent_chooser = gtk_recent_chooser_menu_new_for_manager(recent_manager);
    caprecent->recentprivate->recent_chooser =
        GTK_RECENT_CHOOSER(recent_chooser);

    gtk_recent_chooser_set_show_not_found(GTK_RECENT_CHOOSER(recent_chooser),
                                          TRUE);
    gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recent_chooser), FALSE);
    gtk_recent_chooser_set_show_icons(GTK_RECENT_CHOOSER(recent_chooser),
                                      FALSE);
    gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent_chooser),
                                 DEF_MAX_ITEMS);
    gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent_chooser),
                                     GTK_RECENT_SORT_MRU);

    accel_label = gtk_accel_label_new(_(MSG_RECENT));
    gtk_misc_set_alignment(GTK_MISC(accel_label), 0.0, 0.5);

    gtk_container_add(GTK_CONTAINER(recent_menu_item), accel_label);
    gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(accel_label),
                                     GTK_WIDGET(recent_menu_item));

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(recent_menu_item),
                              recent_chooser);


    hildon_helper_set_insensitive_message(GTK_WIDGET(recent_menu_item),
                                          _(MSG_NOT_AVAILABE));


    g_signal_connect(G_OBJECT(recent_menu_item), "expose-event",
                     G_CALLBACK(comapp_recent_menu_expose), NULL);

    g_signal_connect(G_OBJECT(recent_chooser), "item-activated",
                     G_CALLBACK(comapp_recent_item_activated), caprecent);
}

/**
 * Returns the application name, if this is set, otherwise it will return the same
 * as g_get_application_name
 * 
 * @recent_widget ComappRecent widget
 * @return newly allocated application name
 */
static const gchar *
comapp_recent_get_app_name(ComappRecent * recent_widget)
{
    if (recent_widget->recentprivate->app_name)
    {
        return recent_widget->recentprivate->app_name;
    }
    else
    {
        return g_get_application_name();
    }
}

/**
 * Called, when the widget is exposed. This will check the files, if exists or not,
 * and if not, it will remove those file from the database.
 * If the list is empty at the end, it will dimm the widget
 *
 */
static gboolean
comapp_recent_menu_expose(GtkWidget * widget, GdkEventExpose * event,
                          gpointer user_data)
{
    GList *item_list = NULL, *item_start_list = NULL;
    gint found_items = 0;
    const gchar *app_name = NULL;

    ComappRecent *recent_widget = (ComappRecent *) widget;
    app_name = comapp_recent_get_app_name(recent_widget);
    item_start_list = item_list =
        gtk_recent_chooser_get_items(GTK_RECENT_CHOOSER
                                     (recent_widget->recentprivate->
                                      recent_chooser));
    while (item_list)
    {
	GtkRecentInfo *info = (GtkRecentInfo *) item_list->data;
        if (gtk_recent_info_has_application
            (info, app_name))
        {
	    
	    /* g_debug("%s: recent_info display name: %s", __func__,
	    	gtk_recent_info_get_display_name(info)); */
	    	
	    /* only remove local files from recent menu, because
	     * gtk always returns false for remote ones. */
	    if (gtk_recent_info_is_local(info) && !gtk_recent_info_exists(info))
            {
                comapp_recent_delete_item(recent_widget,
                                          gtk_recent_info_get_uri(info));
            }
            else
            {
                found_items++;
            }
        }
        gtk_recent_info_unref(info);
        item_list = g_list_next(item_list);
    }
    g_list_free(item_start_list);
    if (found_items)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(widget), TRUE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
    }
    
    /* Set sensitivity of files listed */
    {
    	gboolean sensitive = !( recent_widget->recentprivate->files_insensitive );
    	GtkWidget * submenu = gtk_menu_item_get_submenu( GTK_MENU_ITEM( widget ) );
    	if( submenu != NULL ) {
	    	GList * node = GTK_MENU_SHELL( submenu )->children;
   			for(;node;node=node->next) {
    			gtk_widget_set_sensitive( GTK_WIDGET( node->data ), sensitive );
    		}
    	}
    }

    return FALSE;
}



static void
comapp_recent_set_mimetype(ComappRecent * recent_widget,
                           const gchar * mime_type)
{
    recent_widget->recentprivate->mime_type = g_strdup(mime_type);
}


static void
comapp_recent_set_extension(ComappRecent * recent_widget,
                            const gchar ** extension)
{
    gchar **new_extension_list = NULL;
    gint i = 0;
    if (recent_widget->recentprivate->extension)
    {
        g_strfreev(recent_widget->recentprivate->extension);
    }
    if (extension)
    {
        while (extension[i])
        {
            i++;
        }
        if (i > 0)
        {
            new_extension_list = g_new0(gchar *, i + 1);
            i = 0;
            while (extension[i])
            {
                new_extension_list[i] = g_strdup((gchar *) extension[i]);
                i++;
            }
        }
    }
    recent_widget->recentprivate->extension = new_extension_list;
}

gboolean
comapp_recent_get_cut_extension(ComappRecent * recent_widget)
{
    return recent_widget->recentprivate->cut_extension;
}

void
comapp_recent_set_cut_extension(ComappRecent * recent_widget,
                                gboolean cut_extension)
{
    recent_widget->recentprivate->cut_extension = cut_extension;
}

GtkWidget *
comapp_recent_new(const gchar * app_name, const gchar * mime_type,
                  const gchar ** fextension, gboolean cut_extension)
{
    ComappRecent *recent_widget = NULL;
    recent_widget = g_object_new(comapp_recent_get_type(), NULL);
    if (app_name)
    {
        comapp_recent_set_app_name(recent_widget, app_name);
    }
    if (mime_type)
    {
        comapp_recent_set_mimetype(recent_widget, mime_type);
    }
    if (fextension)
    {
        comapp_recent_set_extension(recent_widget, fextension);
    }
    comapp_recent_set_cut_extension(recent_widget, cut_extension);
    comapp_recent_set_max_items(recent_widget, DEF_MAX_ITEMS);
    return GTK_WIDGET(recent_widget);
}

void
comapp_recent_set_max_items(ComappRecent * recent_widget, gint max_items)
{
    gtk_recent_chooser_set_limit(recent_widget->recentprivate->recent_chooser,
                                 max_items);

}

gint
comapp_recent_get_max_items(ComappRecent * recent_widget)
{
    return gtk_recent_chooser_get_limit(recent_widget->recentprivate->
                                        recent_chooser);
}

void
comapp_recent_set_app_name(ComappRecent * recent_widget,
                           const gchar * app_name)
{
    GtkRecentFilter *recent_filter = NULL;

    g_assert(recent_widget);
    g_assert(app_name);

    recent_filter = gtk_recent_filter_new();
    gtk_recent_filter_add_application(recent_filter, app_name);
    gtk_recent_chooser_set_filter(recent_widget->recentprivate->
                                  recent_chooser, recent_filter);
    recent_widget->recentprivate->app_name = g_strdup(app_name);
    return;
}


gboolean
comapp_recent_add_new_item(ComappRecent * recent_widget, const gchar * uri)
{
    g_assert(recent_widget);
    if (uri)
    {
        gboolean add_ret_val;
	GtkRecentData recent_data = { 0 };

	GFile *file = g_file_new_for_commandline_arg(uri);
	gchar *really_uri = g_file_get_uri(file);
	g_object_unref(file);

        recent_data.display_name =
            comapp_common_get_display_name(really_uri,
                                           recent_widget->recentprivate->
                                           extension,
                                           recent_widget->recentprivate->
                                           cut_extension);
        if (recent_widget->recentprivate->mime_type)
        {
            recent_data.mime_type =
                g_strdup(recent_widget->recentprivate->mime_type);
        }
        else
        {
            recent_data.mime_type = g_strdup("text/plain");
        }
        recent_data.app_name =
            g_strdup(comapp_recent_get_app_name(recent_widget));
        recent_data.app_exec = g_get_prgname();
        add_ret_val =
            gtk_recent_manager_add_full(recent_widget->recentprivate->
                                        recent_manager, really_uri, &recent_data);

	g_free(really_uri);
        return add_ret_val;
    }
    return FALSE;
}

gboolean
comapp_recent_delete_item(ComappRecent * recent_widget, const gchar * uri)
{
    GError *recent_g_error = NULL;

    g_assert(recent_widget);

    if (uri)
    {
        return gtk_recent_manager_remove_item(recent_widget->recentprivate->
                                              recent_manager, uri,
                                              &recent_g_error);
    }
    return FALSE;
}

/**
 * Called, when and item is selected form the menu. It will emmit the "recent_selected" signal
 *
 */
static void
comapp_recent_item_activated(GtkRecentChooser * chooser, gpointer user_data)
{
    ComappRecent *recent_widget = (ComappRecent *) user_data;
    gchar *file_uri = NULL;

    g_assert(recent_widget);

    file_uri = gtk_recent_chooser_get_current_uri(chooser);
    g_signal_emit(G_OBJECT(recent_widget),
                  comapp_recent_signals[RECENT_SELECTED_SIGNAL], 0, file_uri);
    g_free(file_uri);
}

void comapp_recent_set_files_insensitive( ComappRecent * recent_widget,
										   gboolean insensitive ) {

    g_assert(recent_widget);
	recent_widget->recentprivate->files_insensitive = insensitive;
	gtk_widget_queue_draw( GTK_WIDGET( recent_widget ) );
}

