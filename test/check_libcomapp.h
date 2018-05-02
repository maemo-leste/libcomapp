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

#ifndef CHECK_CHECK_H
#define CHECK_CHECK_H

#ifndef TIMEOUT_TESTS_ENABLED
#define TIMEOUT_TESTS_ENABLED 1
#endif

Suite *make_master_suite(void);
Suite *make_recent_suite(void);
Suite *make_sendvia_suite(void);

#endif /* CHECK_CHECK_H */
