/* datatypes.h
 *
 * Copyright 2021 Andrison Vargas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

typedef struct _ApplicationWindow     ApplicationWindow;
typedef struct _Monitor               Monitor;
typedef struct _TabletDevice          TabletDevice;

struct _ApplicationWindow
{
  char              *name;
  unsigned int       w;
  unsigned int       h;
  int                x;
  int                y;
  ApplicationWindow *next;
};

struct _Monitor
{
  const char *connector;
  int         w;
  int         h;
  int         x;
  int         y;
  Monitor    *next;
};

struct _TabletDevice
{
  char         *name;
  char         *id;
  char         *type;
  int           w;
  int           h;
  int           x;
  int           y;
  TabletDevice *next;
};

ApplicationWindow * new_window();
void insert_window(ApplicationWindow *head, ApplicationWindow *window);

Monitor * new_monitor();
void insert_monitor(Monitor *head, Monitor *monitor);

TabletDevice * new_tablet_device();
void insert_tablet_device(TabletDevice *head, TabletDevice *tablet_device);
