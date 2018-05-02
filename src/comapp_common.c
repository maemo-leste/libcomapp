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


#include "comapp_common.h"
#include <libgnomevfs/gnome-vfs.h>
#include <string.h>

HildonFileSystemModel *
comapp_common_new_fs_model(GtkWidget * ref_widget)
{
    g_assert(ref_widget);
    return
        HILDON_FILE_SYSTEM_MODEL(g_object_new
                                 (HILDON_TYPE_FILE_SYSTEM_MODEL, "ref_widget",
                                  ref_widget, NULL));
}

void
comapp_common_fs_model_set_ref_widget(HildonFileSystemModel * fsmodel,
                                      GtkWidget * ref_widget)
{
    g_assert(ref_widget);
    g_assert(fsmodel);
    g_object_set(G_OBJECT(fsmodel), "ref_widget", ref_widget, NULL);
}

/**
 * Puts a \0 to the last '.' place, if it is there
 **/
static gchar *
comapp_common_cut_extension(gchar * fname)
{
    char *pos = strrchr(fname, '.');
    if (pos)
        *pos = '\0';
    return fname;
}

gchar *
comapp_common_get_display_name(const gchar * uri, gchar ** extension,
                               gboolean cut_extension)
{
    gchar *unescaped_str = NULL;
    gchar *bname = NULL;
    gint i = 0;

    g_assert(uri);

    unescaped_str = gnome_vfs_unescape_string(uri, NULL);
    bname = g_path_get_basename(unescaped_str);
    g_free(unescaped_str);
    if (extension)
    {
        while (extension[i])
        {
            if (g_str_has_suffix(bname, (gchar *) extension[i]))
            {
                bname[strlen(bname) - strlen((gchar *) extension[i])] = '\0';
                break;
            }
            i++;
        }
        if (extension[i])
        {
            return bname;
        }
    }
    if (cut_extension)
    {
        bname = comapp_common_cut_extension(bname);
    }
    return bname;
}

