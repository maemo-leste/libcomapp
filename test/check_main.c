/* LibComApp
 *
 * Copyright (c) 2005-06 Nokia Corporation. All rights reserved.
 * Contact: Ahmad Qureshi <Ahmad.Qureshi@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <check.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "check_libcomapp.h"


int
main (int argc,char **argv)
{
  int number_failed;
  gtk_init(&argc,&argv);
  g_type_init();
  SRunner *sr = srunner_create (make_master_suite());
  srunner_add_suite(sr, make_recent_suite());
  srunner_add_suite(sr, make_sendvia_suite());
  
  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
