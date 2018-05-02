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

#include <check.h>
#include "../src/comapp_recent.h"



START_TEST(test_recent_create_destroy)
{
  ComappRecent *recent_widget=NULL;
  recent_widget= comapp_recent_new(NULL,NULL,NULL);
  fail_unless(recent_widget, "Creating of recent widget without data failed");
  gtk_widget_destroy(GTK_WIDGET(recent_widget));
  recent_widget= comapp_recent_new("testapp","mime/type","extension");
  fail_unless(recent_widget, "Creating of recent widget with data failed");
  gtk_widget_destroy(GTK_WIDGET(recent_widget));
}
END_TEST

START_TEST(test_recent_property)
{
  ComappRecent *recent_widget=NULL;
  gchar *app_name=NULL,*mimetype=NULL,*extension=NULL;
  recent_widget= comapp_recent_new(NULL,NULL,NULL);
  fail_unless(recent_widget, "Creating of recent widget without data failed");
  comapp_recent_set_max_items(recent_widget,10);
  fail_unless(comapp_recent_get_max_items(recent_widget)==10,"Can not set maximal items");
  fail_unless(comapp_recent_add_new_item(recent_widget,"file:///scratchbox"),"Can not add test file\n");
  fail_unless(comapp_recent_delete_item(recent_widget,"file:///scratchbox"),"Can not remove test file\n");
  gtk_widget_destroy(GTK_WIDGET(recent_widget));
}
END_TEST


Suite *make_recent_suite (void)
{
  Suite *s = suite_create("Recent");
  TCase * tc = tcase_create("Core");

  suite_add_tcase (s, tc);
  tcase_add_test (tc, test_recent_create_destroy);
  tcase_add_test (tc, test_recent_property);
  return s;
}
