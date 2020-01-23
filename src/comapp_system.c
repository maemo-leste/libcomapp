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

#include "comapp_system.h"

#include <osso-log.h>
#include <signal.h>

#define COMAPP_SYSTEM_CLOSESTATE_GCONF "/apps/osso/libcomapp/"


ComappSystemData *comapp_system_data = NULL;

/**
  Save close state of application
*/
static void
comapp_system_save_close_state( ComappSystemData * data ) {

	gchar * key;
 
    key = g_strconcat( COMAPP_SYSTEM_CLOSESTATE_GCONF, data->pkg_name, NULL );
    gconf_client_set_int( data->gc_client, key, data->close_state, NULL );
    g_free( key );

}

static void
comapp_system_signal_handler(int sig)
{           
    if (comapp_system_data->save_for_loading &&
    	!comapp_system_data->save_for_loading(comapp_system_data))
    {
	    comapp_system_data->close_state = BACKGROUND_CLOSE;
    }

    gtk_main_quit();
}

static void
comapp_system_hw_cb(osso_hw_state_t * state, gpointer data)
{
    gboolean call_cb = FALSE;
    ComappSystemData *cap_s_data = NULL;
    cap_s_data = (ComappSystemData *) data;
    g_assert(cap_s_data);

    /* shutdown_ind is replaced by save_unsaved_data_ind*/
    if (state->save_unsaved_data_ind) {
        if( cap_s_data->save_for_asking != NULL &&
        	!cap_s_data->save_for_asking(cap_s_data)) {
	        cap_s_data->close_state = SHUTDOWN_CLOSE;
	        /* Just to be sure save state already here */
		comapp_system_save_close_state( cap_s_data );
        } else {
        	cap_s_data->close_state = USER_CLOSE;
        }
        gtk_main_quit();
        return;
    }
    
    /* don't care, using osso auto save
    if (state->save_unsaved_data_ind)
    {
        comapp_system_data->close_state = BACKGROUND_CLOSE;
        if (cap_s_data->save_for_loading &&
        	cap_s_data->save_for_loading(cap_s_data))
        {
            comapp_system_data->close_state = CLEAN_CLOSE;
        }
    }
    */
    
    if (cap_s_data->system_inactive != state->system_inactivity_ind)
    {
        cap_s_data->system_inactive = state->system_inactivity_ind;
        call_cb = TRUE;
    }
    if (cap_s_data->memory_low != state->memory_low_ind)
    {
        cap_s_data->memory_low = state->memory_low_ind;
        call_cb = TRUE;
    }
    if (call_cb && cap_s_data->hw_state_changed)
    {
        cap_s_data->hw_state_changed(cap_s_data);
    }
}


static gint
comapp_system_req_handler(const gchar * interface, const gchar * method,
                          GArray * arguments, gpointer data,
                          osso_rpc_t * retval) 
{
                          
    ComappSystemData *cap_s_data = NULL;
    osso_rpc_t val;
    cap_s_data = (ComappSystemData *) data;
    
    g_assert(cap_s_data);

	/* Call mime open callback */
   if( g_ascii_strcasecmp(method, "mime_open") == 0 &&
    	cap_s_data->mime_open_cb != NULL ) 
   {
        osso_rpc_t *array = &g_array_index(arguments, osso_rpc_t, 0);
	if(array) 
	{
		/* NB#105223 */
		if(NULL == cap_s_data->mime_open_cb) {
			return OSSO_OK;
	        }
		
		val.type = array->type;
		val.value.s = g_strdup(array->value.s);
	
		if ((val.type == DBUS_TYPE_STRING) && (val.value.s != NULL))
		{
		    cap_s_data->mime_open_uri = val.value.s;
		    cap_s_data->mime_open_cb(cap_s_data);
		    retval->type = DBUS_TYPE_BOOLEAN;
		    retval->value.b = TRUE;
		    return OSSO_OK;
		}
	}
	else 
	{
		return OSSO_ERROR; 
	}
    }
  
    if( cap_s_data->dbus_req_handler != NULL ) 
    {
	   return cap_s_data->dbus_req_handler( interface, method, arguments,
	   	data, retval );
    }
    else
    {
	return OSSO_OK;
    }
}


ComappSystemData *
comapp_system_init(ComappSystemData * data)
{
    int close_state;
    gchar *g_key = NULL;
    struct sigaction siga;
    sigset_t empty_mask;
    sigemptyset(&empty_mask);
    siga.sa_handler = comapp_system_signal_handler;
    siga.sa_mask = empty_mask;
    siga.sa_flags = 0;

    sigaction(SIGTERM, &siga, NULL);
    sigaction(SIGINT, &siga, NULL);

    g_assert(data);

    if (!data->gc_client)
    {
#if !GLIB_CHECK_VERSION (2,35,0)
        g_type_init();
#endif
        data->gc_client = gconf_client_get_default();
    }
    if (!data->osso)
    {
        data->osso =
            osso_initialize(data->pkg_name, data->pkg_version, TRUE, NULL);
    }
    if( !data->osso ) {
        g_critical("OSSO context initialization failed");
        return NULL;
    }

    g_assert(data->osso);
    g_assert(data->dbus_service);
    g_assert(data->dbus_path);
    g_assert(data->dbus_interface);

    if (osso_rpc_set_cb_f
        (data->osso, data->dbus_service, data->dbus_path,
         data->dbus_interface, comapp_system_req_handler, data) != OSSO_OK)
    {
        g_critical("Error setting RPC callback");
        osso_deinitialize(data->osso);
        return NULL;
    }
    if (osso_hw_set_event_cb(data->osso, NULL, comapp_system_hw_cb, data) !=
        OSSO_OK)
    {
        osso_deinitialize(data->osso);
        return NULL;
    }

    g_assert(data->pkg_name);

    g_key = g_strconcat(COMAPP_SYSTEM_CLOSESTATE_GCONF, data->pkg_name, NULL);
    close_state = gconf_client_get_int(data->gc_client, g_key, NULL);
    if ((close_state == BACKGROUND_CLOSE) && (data->open_for_loading))
    {
        data->open_for_loading(data);
    }
    else if ( (close_state == SHUTDOWN_CLOSE || close_state == UNCLEAN_CLOSE ) 
    	&& (data->open_for_asking))
    {
        data->open_for_asking(data);
    }
    gconf_client_set_int(data->gc_client, g_key, UNCLEAN_CLOSE, NULL);
    data->close_state = USER_CLOSE;
    g_free(g_key);

    comapp_system_data = data;
    return data;
}

void
comapp_system_deinit(ComappSystemData * data) {
    osso_return_t result;

    g_assert(data);
    g_debug( __FUNCTION__ );

    comapp_system_save_close_state( data );
    g_object_unref(data->gc_client);
    osso_hw_unset_event_cb(data->osso, NULL);
    result =
        osso_rpc_unset_cb_f(data->osso, data->dbus_service, data->dbus_path,
                            data->dbus_interface, comapp_system_req_handler,
                            data);

    if (result != OSSO_OK)
    {
        g_critical("Not able to deinitialize the RPC_CB function");
    }

    osso_deinitialize(data->osso);
    data->osso = NULL;
    return;
}
