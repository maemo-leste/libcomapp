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


#ifndef __COMAPP_RECENT_H__
#define __COMAPP_RECENT_H__


#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkmenuitem.h>



G_BEGIN_DECLS
#define COMAPP_RECENT_TYPE            (comapp_recent_get_type ())
#define COMAPP_RECENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), COMAPP_RECENT_TYPE, ComappRecent))
#define COMAPP_RECENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), COMAPP_RECENT_TYPE, ComappRecentClass))
#define IS_COMAPP_RECENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), COMAPP_RECENT_TYPE))
#define IS_COMAPP_RECENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), COMAPP_RECENT_TYPE))
typedef struct _ComappRecent_Private ComappRecent_Private;
typedef struct _ComappRecent ComappRecent;
typedef struct _ComappRecentClass ComappRecentClass;

struct _ComappRecent {
    /* < private > */
    GtkMenuItem recentmenu;


    ComappRecent_Private *recentprivate;



};

struct _ComappRecentClass {
    /* < private > */
    GtkMenuItemClass parent_class;

    void (*recent_selected) (ComappRecent * comapprecent, gchar * fileuri);

    /* padding for future expansion */
    void (*_comapp_recent1) (void);
    void (*_comapp_recent2) (void);
    void (*_comapp_recent3) (void);
    void (*_comapp_recent4) (void);
};

GType comapp_recent_get_type(void);

/**
 * Deletes an item from the recent database
 *
 * @recent_widget created ComappRecent widget
 * @uri the uri of the file, what will be deleted
 * @return TRUE on success, FALSE otherwise
 */
gboolean comapp_recent_delete_item(ComappRecent * recent_widget,
                                   const gchar * uri);

/**
 * Adds a new item to the recent database
 *
 * @recent_widget created ComappRecent widget
 * @uri the uri of the new file
 * @return TRUE on success, FALSE otherwise
 * 
 */
gboolean comapp_recent_add_new_item(ComappRecent * recent_widget,
                                    const gchar * uri);

/**
 * Sets the application name for the widget
 *
 * @recent_widget created ComappRecent widget
 * @app_name the application name
 */
void comapp_recent_set_app_name(ComappRecent * recent_widget,
                                const gchar * app_name);

/**
 * Returns the maximal item number of the widget
 *
 * @recent_widget created ComappRecent widget
 * @return the maximal showed recent items
 *
 */
gint comapp_recent_get_max_items(ComappRecent * recent_widget);

/**
 * Sets the maximal item number for the widget
 *
 * @recent_widget created ComappRecent widget
 * @max_items the number of the maximum visible recent items. The default is 6
 *
 */
void comapp_recent_set_max_items(ComappRecent * recent_widget,
                                 gint max_items);


/**
 * Returns the cut-extension property
 *
 * @recent_widget created ComappRecent widget
 * @return the state of the property
 *
 */
gboolean comapp_recent_get_cut_extension(ComappRecent * recent_widget);

/**
 * Sets the cut-extension property
 *
 * @recent_widget created ComappRecent widget
 * @cut_extension If this is TRUE, then the file extension will be cut, if it is not in the list
 *
 */
void comapp_recent_set_cut_extension(ComappRecent * recent_widget,
                                     gboolean cut_extension);


/**
 * Make files listed insensitive (this can be used to prevent user to open files)
 *
 * @param recent_widget created ComappRecent widget
 * @param sensitive If the files should be made insensitive
 *
 */
void comapp_recent_set_files_insensitive( ComappRecent * recent_widget,
										   gboolean insensitive );

/**
 * Creates a new ComappRecent widget
 *
 * @app_name the application name. If this is NULL, g_get_application_name will be used to  determine this
 * @mime_type the mime type of the newly added file. If this is NULL, text/plain will be used
 * @fextension the file extensions of the new files. This can be NULL. The list with its strings should be freed
 * @cut_extension If this is TRUE, and the file doesn't have an extension from the list, the string after the last '.' will be cut
 * @return a new ComappRecent widget
 */
GtkWidget *comapp_recent_new(const gchar * app_name, const gchar * mime_type,
                             const gchar ** fextension,
                             gboolean cut_extension);

G_END_DECLS
#endif /* __COMAPP_RECENT_H__ */
