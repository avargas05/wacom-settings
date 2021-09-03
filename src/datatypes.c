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

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "datatypes.h"


ApplicationWindow *
new_window() {
  ApplicationWindow *window = (ApplicationWindow *) malloc(sizeof(ApplicationWindow));
  window->name = NULL;
  window->w = 0;
  window->h = 0;
  window->x = 0;
  window->y = 0;
  window->next = NULL;

  return window;
}


void
insert_window(ApplicationWindow *head, ApplicationWindow *window) {
  if(head->next == NULL) {
    head->next = window;
  }
  else {
    ApplicationWindow *current = head;
    while (true) {
      if(current->next == NULL) {
        current->next = window;
        break;
      }
      current = current->next;
    };
  }
}


Monitor *
new_monitor() {
  Monitor *monitor = (Monitor *) malloc(sizeof(Monitor));
  monitor->connector = NULL;
  monitor->w = 0;
  monitor->h = 0;
  monitor->x = 0;
  monitor->y = 0;
  monitor->next = NULL;

  return monitor;
}

void
inser_monitor(Monitor *head, Monitor *monitor) {
  if(head->next == NULL) {
    head->next = monitor;
  }
  else {
    Monitor *current = head;
    while (true) {
      if(current->next == NULL) {
        current->next = monitor;
        break;
      }
      current = current->next;
    };
  }
}

TabletDevice *
new_tablet_device() {
  TabletDevice *tablet_device = (TabletDevice *) malloc(sizeof(TabletDevice));
  tablet_device->name = NULL;
  tablet_device->id = NULL;
  tablet_device->type = NULL;
  tablet_device->w = 0;
  tablet_device->h = 0;
  tablet_device->x = 0;
  tablet_device->y = 0;
  tablet_device->next = NULL;

  return tablet_device;
}

void
insert_tablet_device(TabletDevice *head, TabletDevice *tablet_device) {
  if(head->next == NULL) {
    head->next = tablet_device;
  }
  else {
    TabletDevice *current = head;
    while (true) {
      if(current->next == NULL) {
        current->next = tablet_device;
        break;
      }
      current = current->next;
    };
  }
}
