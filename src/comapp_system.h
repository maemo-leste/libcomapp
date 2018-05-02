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


#ifndef __COMAPP_SYSTEM_H__
#define __COMAPP_SYSTEM_H__

#include <glib.h>
#include <libosso.h>
#include <gtk/gtk.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

G_BEGIN_DECLS

/** Close states of applications */
typedef enum {
    USER_CLOSE,				/**< Application is closed clean by user */
    BACKGROUND_CLOSE,		/**< Application was background killed */
    SHUTDOWN_CLOSE,			/**< Application was closed with device */
   	UNCLEAN_CLOSE			/**< Application was closed/killed by unknown manner */
} comapp_system_close_state;

typedef struct _ComappSystemData ComappSystemData;

typedef void (comapp_system_cb) (ComappSystemData * user_data);

/** The prototype for the save function. Should return TRUE if no close state handling */
typedef gboolean(comapp_system_save_cb) (ComappSystemData * user_data);


struct _ComappSystemData {
  /** the GConfClient. If this is NULL, the library will initialize this, and it will save this to here */
    GConfClient *gc_client;
  /** the callback function, what will be called, if the state should be save so, that on next time, a dialog should appear, which asks the user whether she/he wants to load the saved file */
    comapp_system_save_cb *save_for_asking;
  /** opens the "load saved document?" dialog, and opens the saved file if needed */
    comapp_system_cb *open_for_asking;
  /** this cb will save the file so, that on next time it will load this automatically */
    comapp_system_save_cb *save_for_loading;
  /** the cb for automatic loading function */
    comapp_system_cb *open_for_loading;
  /** called, when the low_battery or system_inacitive state changed. can be NULL if not needed */
    comapp_system_cb *hw_state_changed;
  /**called when the application got the mime_open dbus-call. The file-uri will be passed to this structure */
    comapp_system_cb *mime_open_cb;
  /** called, when not the mime-open dbus message was received */
    osso_rpc_cb_f *dbus_req_handler;
  /** the osso context. If this is NULL, the library will initialize this */
    osso_context_t *osso;
  /** the package name. Needed to the osso initialization */
    gchar *pkg_name;
  /** the package version. Needed to the osso initialization */
    gchar *pkg_version;
  /** the dbus service */
    gchar *dbus_service;
  /** the dbus path */
    gchar *dbus_path;
  /** the dbus interface */
    gchar *dbus_interface;
  /** the system inactive state */
    gboolean system_inactive;
  /** the memory low indicator */
    gboolean memory_low;
  /** the user data */
    gpointer user_data;
  /** the uri to the file what should be opened */
    const gchar *mime_open_uri;
  /** the close type */
    comapp_system_close_state close_state;
};

/**
 * This function will initialize the osso and gconf system if need. It will set up the hw-state monitor
 * function, it will set up a system signal monitor, and it will call the stored
 * functions in the structure when it is needed
 *
 * @data The filled data structure
 * @return the filled data structure
 */
ComappSystemData *comapp_system_init(ComappSystemData * data);

/**
 * This will deinitialize the gconf and osso system with its callbacks.
 * 
 * @data the filled data structure
 */
void comapp_system_deinit(ComappSystemData * data);

G_END_DECLS
#endif /* __COMAPP_SYSTEM_H__ */
