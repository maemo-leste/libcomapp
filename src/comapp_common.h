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


#ifndef __COMAPP_COMMON_H__
#define __COMAPP_COMMON_H__


#include <hildon/hildon-file-system-model.h>


G_BEGIN_DECLS
/**
 * Creates a new HildonFileSystemModel with the given reference widget
 *
 * @ref_widget the reference widget
 * @return a new HildonFileSystemModel object
 */
    HildonFileSystemModel * comapp_common_new_fs_model(GtkWidget *
                                                       ref_widget);

/**
 * Updates the existing HildonFileSystemModel object to the new reference widget
 *
 * @fsmodel existing HildonFileSystemModel object
 * @ref_widget new reference widget
 */
void comapp_common_fs_model_set_ref_widget(HildonFileSystemModel * fsmodel,
                                           GtkWidget * ref_widget);

/**
 * Returns the newly allocated unescaped name of the uri.
 *
 * @uri the uri of the file
 * @extension the extension of the file, what shuld be cut if exists, or NULL, if nothing should be cut
 * @return the newly allocated unescaped filename
 */
gchar *comapp_common_get_display_name(const gchar * uri, gchar ** extension,
                                      gboolean cut_extension);

G_END_DECLS
#endif /* __COMAPP_COMMON_H__ */
