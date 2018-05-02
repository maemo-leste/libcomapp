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


#include <gtk/gtkrecentmanager.h>
#include <gtk/gtkrecentchooser.h>

struct _ComappRecent_Private {
    GtkRecentManager *recent_manager;
    GtkRecentChooser *recent_chooser;

    gchar *app_name;
    gchar *mime_type;
    gchar **extension;
    gboolean cut_extension;
    gboolean files_insensitive;
};
