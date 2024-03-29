/* wacom_settings-window.c
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

#define _GNU_SOURCE

#include <ctype.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "wacom_settings-config.h"
#include "wacom_settings-window.h"
#include "datatypes.h"
#include "parser.h"
#include "windowmanager.h"


struct _WacomSettingsWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkDropDown       *devices_combobox;
  GtkDropDown       *monitors_combobox;
  GtkDropDown       *windows_combobox;
  GtkDropDown       *rotation_combobox;
  GtkCheckButton    *aspect_ratio_checkbox;
  GtkButton         *reset_all_button;
  GtkButton         *apply_button;
  GtkButton         *close_button;

  /* Options in the drop down */
  TabletDevice      *device;
  Monitor           *monitor;
  ApplicationWindow *window;
};

G_DEFINE_TYPE (WacomSettingsWindow, wacom_settings_window, GTK_TYPE_APPLICATION_WINDOW)


/* xsetwacom --set id ResetArea */
static void
reset_device (TabletDevice *device)
{
  FILE *shell = NULL;
  char *command = NULL;
  FILE *area_shell = NULL;
  char area[30];
  char s[] = " ";
  char *token = NULL;

  if (asprintf(&command, "xsetwacom --set %s ResetArea", device->id) == -1) {
    exit(0);
  }

  shell = popen(command, "r");
  pclose(shell);
  free(command);
  command = NULL;

  if (asprintf(&command, "xsetwacom --get %s Area", device->id) == -1) {
    exit(0);
  }

  area_shell = popen(command, "r");

  while (fgets (area, sizeof (area), area_shell) != NULL) {
    token = strtok(area, s);
    device->x = atoi(token);
    token = strtok(NULL, s);
    device->y = atoi(token);
    token = strtok(NULL, s);
    device->w = atoi(token);
    token = strtok(NULL, s);
    device->h = atoi(token);
  }

  pclose(area_shell);
  free(command);
  command = NULL;

  shell = NULL;
  area_shell = NULL;
  token = NULL;
}


/* Call cmd xsetwacom --list devices.
 * Parse each line for name, id, type and add to GtkDropDown widget.
 * Use id to call 'xsetwacom --get id Area'
 * Parse those lines for dimensions of each type.
 */
static void
get_devices(GtkDropDown  *devices_combobox,
            TabletDevice *device)
{
  g_assert (GTK_IS_DROP_DOWN(devices_combobox));

  GtkStringList *device_names = gtk_string_list_new(NULL);
  FILE *shell = NULL;
  FILE *area_shell = NULL;
  char  buff[800];
  char  area[30];
  char  s[] = " ";
  char *token = NULL;
  char *command = NULL;

  shell = popen("xsetwacom --list devices", "r");
  while (fgets (buff, sizeof (buff), shell) != NULL) {
    /* Move to new device if name has a value. */
    if (device->name != NULL) {
      device->next = new_tablet_device();
      device = device->next;
    }

    device->name = parse_name(buff);
    device->id = parse_id(buff);
    device->type = parse_type(buff);

    gtk_string_list_append (device_names, device->type);

    if (asprintf(&command, "xsetwacom --get %s Area", device->id) == -1) {
      exit(0);
    }

    area_shell = popen(command, "r");
    while (fgets (area, sizeof (area), area_shell) != NULL) {
      token = strtok(area, s);
      device->x = atoi(token);
      token = strtok(NULL, s);
      device->y = atoi(token);
      token = strtok(NULL, s);
      device->w = atoi(token);
      token = strtok(NULL, s);
      device->h = atoi(token);
    }

    gtk_drop_down_set_model(devices_combobox, (GListModel*) device_names);
    pclose(area_shell);
    free(command);
    command = NULL;
  }

  pclose(shell);
  shell = NULL;
  area_shell = NULL;
  token = NULL;
  gtk_widget_set_size_request((GtkWidget*) devices_combobox, 250, -1);
}


/* Get gdk display manager and names and dimensions of all displays.
 * Add each monitor name to the GtkDropDown widget.
 */
static void
get_monitors(GtkDropDown *monitor_combobox,
             Monitor     *monitors)
{
  g_assert (GTK_IS_DROP_DOWN(monitor_combobox));

  GtkStringList     *monitor_options = gtk_string_list_new(NULL);
  GdkDisplayManager *manager = NULL;
  GdkMonitor        *monitor = NULL;
  GListModel        *names = NULL;
  GSList            *displays = NULL;
  GSList            *l = NULL;
  guint              position;
  gpointer           display;
  GdkRectangle       rectangle;

  position = 0;

  manager = gdk_display_manager_get();
  displays = gdk_display_manager_list_displays(manager);

  /* Add empty option. */
  char *str = "";
  gtk_string_list_append (monitor_options, str);

  /* Iterate through displays. */
  for (l = displays; l != NULL; l = l->next) {
    display = l->data;

    /* Iterate through monitors in displays,
     * and add connector names of monitors to GtkDropDown widget.
     */
    names = gdk_display_get_monitors (display);
    monitor = g_list_model_get_item (names, position);
    while (monitor != NULL) {
      /* Move to new monitor if connector has a value. */
      if (monitors->connector != NULL) {
        monitors->next = new_monitor();
        monitors = monitors->next;
      }

      monitors->connector = gdk_monitor_get_connector (monitor);

      gtk_string_list_append (monitor_options, monitors->connector);
      gdk_monitor_get_geometry (monitor, &rectangle);
      monitors->w = rectangle.width;
      monitors->h = rectangle.height;
      monitors->x = rectangle.x;
      monitors->y = rectangle.y;

      position += 1;
      monitor = g_list_model_get_item (names, position);
    }
  }

  gtk_drop_down_set_model(monitor_combobox, (GListModel*) monitor_options);
  manager = NULL;
  monitor = NULL;
  names = NULL;
  displays = NULL;
  l = NULL;
}


/* Call list_windows and add each window name to the
 * GtkDropDown widget.
 */
static void
get_windows(GtkDropDown       *box,
            ApplicationWindow *windows)
{
  g_assert (GTK_IS_DROP_DOWN(box));

  ApplicationWindow *window = NULL;
  GtkStringList     *window_names = gtk_string_list_new(NULL);

  gtk_string_list_append (window_names, "");
  list_windows(windows);

  for (window = windows; window != NULL; window = window->next) {
    gtk_string_list_append (window_names, window->name);
  }

  gtk_drop_down_set_model(box, (GListModel*) window_names);
  window = NULL;
}


/* Set options for rotation GtkDropDown */
static void
set_rotation_options(GtkDropDown *box)
{
  g_assert (GTK_IS_DROP_DOWN(box));

  GtkStringList *options = gtk_string_list_new(NULL);

  gtk_string_list_append(options, "");
  gtk_string_list_append(options, "None");
  gtk_string_list_append(options, "Clockwise");
  gtk_string_list_append(options, "Counter-Clockwise");
  gtk_string_list_append(options, "180\302\260");

  gtk_drop_down_set_model(box, (GListModel*) options);
}

static void
set_rotation(const char *option,
             char       *id)
{
  char *rotation = NULL;
  char *command = NULL;
  FILE *shell = NULL;

  if (!strcmp(option, "None")) {
    rotation = "none";
  } else if (!strcmp(option, "Clockwise")) {
    rotation = "cw";
  } else if (!strcmp(option, "Counter-Clockwise")) {
    rotation = "ccw";
  } else if (!strcmp(option, "180\302\260")) {
    rotation = "half";
  }

  if (asprintf(&command, "xsetwacom --set %s Rotate %s", id, rotation) == -1) {
    exit(0);
  }

  shell = popen(command, "r");
  pclose(shell);
  free(command);
  shell = NULL;
  command = NULL;
}


/* Call command-line xsetwacom --set id MapToOutput width*height+x+y */
static void
set_screen_area (char *id,
                 int   width,
                 int   height,
                 int   x_axis,
                 int   y_axis)
{
  FILE *shell = NULL;
  char *command = NULL;
  int   success;

  success = asprintf(&command,
                     "xsetwacom --set %s MapToOutput %dx%d+%d+%d",
                     id,
                     width,
                     height,
                     x_axis,
                     y_axis);

  if (success == -1) {
    exit(0);
  }

  shell = popen(command, "r");
  pclose(shell);
  free(command);
  shell = NULL;
  command = NULL;
}


/* Set tablet area by calling command-line
 * xsetwacom --set id Area x1 y1 x2 y2
 */
static void
set_tablet_area (char *id,
                 int   width,
                 int   height,
                 int   x_axis,
                 int   y_axis)
{
  FILE *shell = NULL;
  char *command = NULL;
  int   x2 = x_axis + width;
  int   y2 = y_axis + height;
  int   success;

  success = asprintf(&command,
                     "xsetwacom --set %s Area %d %d %d %d",
                     id,
                     x_axis,
                     y_axis,
                     x2,
                     y2);

  if (success == -1) {
    exit(0);
  }

  shell = popen(command, "r");
  pclose(shell);
  free(command);
  shell = NULL;
  command = NULL;
}


/* Gets values from GtkDropDown widgets and applies changes
 * to screen and tablet areas as selected.
 */
static void
apply_settings (WacomSettingsWindow *app)
{
  char              *id = NULL;
  int                tablet_w = 0;
  int                tablet_h = 0;
  int                tablet_x = 0;
  int                tablet_y = 0;
  int                screen_area_w = 0;
  int                screen_area_h = 0;
  int                screen_area_x = 0;
  int                screen_area_y = 0;
  TabletDevice      *device = app->device;
  Monitor           *monitor = app->monitor;
  ApplicationWindow *window = app->window;
  int                cmp;

  const char *equip = gtk_string_object_get_string((GtkStringObject*) gtk_drop_down_get_selected_item (app->devices_combobox));
  const char *app_window = gtk_string_object_get_string((GtkStringObject*) gtk_drop_down_get_selected_item (app->windows_combobox));
  const char *screen = gtk_string_object_get_string((GtkStringObject*) gtk_drop_down_get_selected_item (app->monitors_combobox));
  const char *rotation = gtk_string_object_get_string((GtkStringObject*) gtk_drop_down_get_selected_item (app->rotation_combobox));
  gboolean keep_ratio = gtk_check_button_get_active (app->aspect_ratio_checkbox);

  /* Get device id and dimensions. */
  if (equip != NULL && strcmp(equip, "")) {
    while(device) {
      cmp = strcmp(device->type, equip);
      if (!cmp) {
        /* Set the rotation first. */
        if (rotation != NULL && strcmp(rotation, "")) {
          set_rotation(rotation, device->id);
        }

        reset_device(device);
        id = device->id;
        tablet_w = device->w;
        tablet_h = device->h;
        tablet_x = device->x;
        tablet_y = device->y;
        break;
      } else {
        device = device->next;
      }
    }
  } else {
    return;
  }

  /* Get the screen area from the application window or monitor selected. */
  if (screen != NULL && strcmp(screen, "")) {
    while(monitor) {
      cmp = strcmp(monitor->connector, screen);
      if (!cmp) {
        screen_area_w = monitor->w;
        screen_area_h = monitor->h;
        screen_area_x = monitor->x;
        screen_area_y = monitor->y;
        break;
      } else {
        monitor = monitor->next;
      }
    }
  } else if (app_window != NULL && strcmp(app_window, "")) {
    while(app_window) {
      cmp = strcmp(window->name, app_window);
      if (!cmp) {
        get_window_dimensions(window);
        screen_area_w = window->w;
        screen_area_h = window->h;
        screen_area_x = window->x;
        screen_area_y = window->y;
        break;
      } else {
        window = window->next;
      }
    }
  }

  /* Keep aspect ratio by adjusting the tablet's drawing area. */
  if (keep_ratio) {
    double tablet_ratio = (double) tablet_w / tablet_h;
    double area_ratio = (double) screen_area_w / screen_area_h;

    if (tablet_ratio > area_ratio) {
      tablet_x += (tablet_w - tablet_h * area_ratio) / 2;
      tablet_w = tablet_h * area_ratio;
    } else {
      tablet_y += (tablet_h - tablet_w / area_ratio) / 2;
      tablet_h = tablet_w / area_ratio;
    }

    set_tablet_area(id, tablet_w, tablet_h, tablet_x, tablet_y);
  }

  set_screen_area(id, screen_area_w, screen_area_h, screen_area_x, screen_area_y);

  equip = NULL;
  app_window = NULL;
  screen = NULL;
  id = NULL;
  device = NULL;
  monitor = NULL;
  window = NULL;
}


/* For each device id call
 * xsetwacom --set id ResetArea
 */
static void
reset_devices (WacomSettingsWindow *app)
{
  FILE         *shell = NULL;
  char         *command = NULL;
  FILE         *area_shell = NULL;
  char          area[30];
  char          s[] = " ";
  char         *token = NULL;
  TabletDevice *device = NULL;

  for (device = app->device; device != NULL; device = device->next) {
    if (asprintf(&command, "xsetwacom --set %s ResetArea", device->id) == -1) {
      exit(0);
    }

    shell = popen(command, "r");
    pclose(shell);
    free(command);
    command = NULL;

    if (asprintf(&command, "xsetwacom --set %s Rotate none", device->id) == -1) {
      exit(0);
    }

    shell = popen(command, "r");
    pclose(shell);
    free(command);
    command = NULL;

    if (asprintf(&command, "xsetwacom --get %s Area", device->id) == -1) {
      exit(0);
    }

    area_shell = popen(command, "r");

    while (fgets (area, sizeof (area), area_shell) != NULL) {
      token = strtok(area, s);
      device->x = atoi(token);
      token = strtok(NULL, s);
      device->y = atoi(token);
      token = strtok(NULL, s);
      device->w = atoi(token);
      token = strtok(NULL, s);
      device->h = atoi(token);
    }

    pclose(area_shell);
    free(command);
    command = NULL;
  }

  shell = NULL;
  area_shell = NULL;
  token = NULL;
  device = NULL;
}


static void
wacom_settings_window_class_init (WacomSettingsWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/com/github/avargas05/wacom-settings/wacom_settings-window.ui");
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, devices_combobox);
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, monitors_combobox);
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, windows_combobox);
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, rotation_combobox);
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, aspect_ratio_checkbox);
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, reset_all_button);
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, apply_button);
  gtk_widget_class_bind_template_child (widget_class, WacomSettingsWindow, close_button);
}


static void
wacom_settings_window_init (WacomSettingsWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

/* Initialize data types */
  self->device = new_tablet_device();
  self->monitor = new_monitor();
  self->window = new_window();

  /* Set items for the tablet's devices in GtkDropDown widget. */
  get_devices (self->devices_combobox, self->device);

  /* Set items for monitors in GtkDropDown widget. */
  get_monitors (self->monitors_combobox, self->monitor);

  /* Set items for active windows in GtkDropDown widget. */
  get_windows (self->windows_combobox, self->window);

  /* Set rotation options. */
  set_rotation_options (self->rotation_combobox);

  /* Reset devices. */
  g_signal_connect_swapped (self->reset_all_button, "clicked", G_CALLBACK (reset_devices), self);

  /* Apply settings to tablet. */
  g_signal_connect_swapped (self->apply_button, "clicked", G_CALLBACK (apply_settings), self);

  /* Connect signal to close application. */
  g_signal_connect_swapped (self->close_button, "clicked", G_CALLBACK (gtk_window_close), self);
}
