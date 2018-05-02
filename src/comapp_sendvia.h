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


#ifndef __COMAPP_SENDVIA_H__
#define __COMAPP_SENDVIA_H__

#include <glib.h>
#include <libosso.h>
#include <gtk/gtk.h>


G_BEGIN_DECLS

#define COMAPP_SENDVIA_EMAIL_ITEM "email"
#define COMAPP_SENDVIA_BLUETOOTH_ITEM "bluetooth"

typedef gchar *(comapp_save_file_cb) (gpointer user_data);

typedef struct _ComappSendviaMenu ComappSendviaMenu;

struct _ComappSendviaMenu {
	GtkMenuItem * email_item;
	GtkMenuItem * bluetooth_item;
};

typedef struct _ComappSendviaData ComappSendviaData;

struct _ComappSendviaData {
    gpointer user_data;
    GtkWidget *parent_widget;
    osso_context_t *osso;
    comapp_save_file_cb *save_file_cb;
    ComappSendviaMenu menu;
};

/**
 * Creates the "Send via bluetooth" menu
 *
 * @sendvia_data filled data structure
 * @return created GtkMenuItem
 */
GtkWidget *comapp_sendvia_create_send_bt(ComappSendviaData * sendvia_data);

/**
 * Creates the "Send via email" menu
 *
 * @sendvia_data filled data structure
 * @return created GtkMenuItem
 */
GtkWidget *comapp_sendvia_create_send_mail(ComappSendviaData * sendvia_data);

/**
 * Sending a saved file through BlueTooth
 *
 * @path the uri path to the file to be send
 * @parent_widget the widget of the infobanner, if there is any error
 * @return TRUE on succes, FALSE otherwise
 */
gboolean comapp_sendvia_bluetooth(gchar * path, GtkWidget * parent_widget);

/**
 * Sending a saved file in e-mail. On error, it will display the
 * "Unable to send via e-mail" message
 *
 * @parent_widget the widget of the infobanner, if there is any error
 * @osso the valid osso_context_t
 * @path the uri path to the file to be send
 * @return TRUE on succes, FALSE otherwise
 */
gboolean comapp_sendvia_email(GtkWidget * paren_widget, osso_context_t * osso,
                              const gchar * path);

/**
 * Creates a GtkMenuItem, which has a submenu, what contains the "Send via Bluetooth.."
 * and the "Send via e-mail..." menu items
 *
 * @deprecated Use new comapp_sendvia_create_menu_item instead
 * @see comapp_sendvia_create_menu_item
 * @save_cb Called when the item is selected. This will save the file if needed, and this will return the uri of the save file
 * @parent_widget needed for the "Send via e-mail...". The widget which will be the parent widget of the error infoprint if needed
 * @osso the valid osso_context_t
 * @user_data user data, what will be passed to the save_cb function
 * @return the GtkMenuItem, which has these sub-menus
 */
GtkWidget *comapp_sendvia_create_main_menu(comapp_save_file_cb * save_cb,
                                           GtkWidget * parent_widget,
                                           osso_context_t * osso,
                                           gpointer user_data);
                                           

/**
 * Creates a GtkMenuItem, which has a submenu, what contains the "Send via Bluetooth.."
 * and the "Send via e-mail..." menu items. This is just a front end to comapp_sendvia_connect_menu_to_item
 * function that creates the menu item. If you have send menu item defined in application
 * then use the other function.
 *
 * @see comapp_sendvia_connect_menu_to_item 
 * @param save_cb Called when the item is selected. This will save the file if needed, and this will return the uri of the save file
 * @param parent_widget needed for the "Send via e-mail...". The widget which will be the parent widget of the error infoprint if needed
 * @param osso the valid osso_context_t
 * @param menu_data pointer to menu data pointer. If not NULL sets pointer to point menu data which can be used with menu functions
 * @param user_data user data, what will be passed to the save_cb function
 * @return the GtkMenuItem, which has these sub-menus
 */
GtkWidget *comapp_sendvia_create_menu_item( comapp_save_file_cb * save_cb,
                                            GtkWidget * parent_widget,
                                            osso_context_t * osso,
                                            ComappSendviaMenu ** menu_data,
                                            gpointer user_data);

/**
 * Creates Sendvia sub menu and connects it to given menu item
 *
 * @param send_menu_item Menu item to which menu is connected
 * @param save_cb Called when the item is selected. This will save the file if needed, and this will return the uri of the save file
 * @param parent_widget needed for the "Send via e-mail...". The widget which will be the parent widget of the error infoprint if needed
 * @param osso the valid osso_context_t
 * @param user_data user data, what will be passed to the save_cb function
 * @return menu data pointer used with menu functions (valid until menu is destroyed)
 */                                          
ComappSendviaMenu *comapp_sendvia_connect_menu_to_item( GtkMenuItem * send_menu_item,
														comapp_save_file_cb * save_cb,
														GtkWidget * parent_widget,
														osso_context_t * osso,
														gpointer user_data);
                                                        

/**
 * Set sensitivity of all Sendvia submenu items
 *
 * @param menu_data pointer to menu data struct reveived from comapp_sendvia_create_menu_item (not NULL)
 * @param sentive boolean value of sentivity
 */                                                 
void compapp_sendvia_set_all_menu_items_sensitive( ComappSendviaMenu * menu_data,
                                                 gboolean sensitive );

/**
 * Set sensitivity of Sendvia submenu item
 *
 * @param menu_data pointer to menu data struct reveived from comapp_sendvia_create_menu_item (not NULL)
 * @param item identify string of item
 * @param sentive boolean value of sentivity
 */                                                 
void compapp_sendvia_set_menu_item_sensitive( ComappSendviaMenu * menu_data,
	const gchar * item, gboolean sensitive );

                                                 
/**
 * Set insensive message to item in Sendvia menu
 *
 * @param menu_data pointer to menu data struct reveived from comapp_sendvia_create_menu_item (not NULL)
 * @param item identify string of item
 * @param message message shown with this item
 */                                                 
void compapp_sendvia_set_insensitive_message( ComappSendviaMenu * menu_data,
	const gchar * item, const gchar * message );

                                       

G_END_DECLS
#endif /* __COMAPP_RECENT_H__ */
