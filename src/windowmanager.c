/* windowmanager.h
 *
 * Original Copyright Tomas Styblo <tripie@cpan.org>
 * Copyright 2021 Andrison Vargas
 *
 * The following work is a derivative of the code from the wmctrl project,
 * which is licensed GPLv2.
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

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "datatypes.h"
#include "parser.h"
#include "wacom_settings-window.h"


static char
*get_property(Display       *display,
              Window         window,
              Atom           req_type,
              char          *prop_name,
              unsigned long *size)
{
  Atom           property;
  Atom           actual_type_return;
  int            actual_format_return;
  unsigned long  nitems_return;
  unsigned long  bytes_after_return;
  unsigned long  tmp_size;
  unsigned char *prop_return;
  char          *ret;

  /* Get property and check if successful */
  property = XInternAtom(display, prop_name, False);
  if (XGetWindowProperty( display,
                          window,
                          property,
                          0,
                          1024,
                          False,
                          req_type,
                         &actual_type_return,
                         &actual_format_return,
                         &nitems_return,
                         &bytes_after_return,
                         &prop_return) != Success) {
    return NULL;
  }

  /* Check if valid type for property */
  if (actual_type_return != req_type) {
    free(prop_return);
    return NULL;
  }

  /* Null terminate the result */
  tmp_size = (actual_format_return / 8) * nitems_return;

  /* Increase to 64-bit multiples */
  if (actual_format_return == 32) {
    tmp_size *= sizeof(long) / 4;
  }

  ret = malloc(tmp_size + 1);
  memcpy(ret, prop_return, tmp_size);
  ret[tmp_size] = '\0';

  if (size) {
    *size = tmp_size;
  }

  free (prop_return);
  return ret;
}


static Window
*get_client_list(Display       *disp,
                 unsigned long *size)
{
  Window *client_list;

  client_list = (Window *) get_property(disp,
                                        DefaultRootWindow(disp),
                                        XA_WINDOW,
                                        "_NET_CLIENT_LIST",
                                        size);

  if (client_list == NULL) {
    client_list = (Window *) get_property(disp,
                                          DefaultRootWindow(disp),
                                          XA_CARDINAL,
                                          "_WIN_CLIENT_LIST",
                                          size);
  }

  if (client_list == NULL) {
    printf("Cannot get client list properties.\n(_NET_CLIENT_LIST or _WIN_CLIENT_LIST)\n");
    return NULL;
  }

  return client_list;
}


static char
*get_window_title(Display *disp,
                  Window   win)
{
  char *title_utf8;
  char *wm_name;
  char *net_wm_name;

  wm_name = get_property(disp, win, XA_STRING, "WM_NAME", NULL);
  net_wm_name = get_property(disp,
                             win,
                             XInternAtom(disp, "UTF8_STRING", False),
                             "_NET_WM_NAME",
                             NULL);

  if (net_wm_name) {
    title_utf8 = g_strdup(net_wm_name);
  }
  else {
    if (wm_name) {
      title_utf8 = g_locale_to_utf8(wm_name, -1, NULL, NULL, NULL);
    }
    else {
      title_utf8 = NULL;
    }
  }

  free(wm_name);
  free(net_wm_name);

  return title_utf8;
}


static char
*get_output_str (char     *str,
                 bool      is_utf8)
{
  char *out;

  if (str == NULL) {
    return NULL;
  }

  if (is_utf8) {
    out = g_strdup(str);
  }
  else {
    if (! (out = g_locale_to_utf8(str, -1, NULL, NULL, NULL))) {
      printf("Cannot convert string from locale charset to UTF-8.\n");
      out = g_strdup(str);
    }
  }

  return out;
}


void
list_windows(ApplicationWindow *window)
{
  Display           *display;
  Window            *client_list;
  unsigned long      client_list_size;
  long unsigned int  i;
  char              *name;

  if (! (display = XOpenDisplay(NULL))) {
    exit(0);
  }

  if ((client_list = get_client_list(display, &client_list_size)) == NULL) {
    exit(0);
  }

  /* Print the list */
  for (i = 0; i < client_list_size / sizeof(Window); i++) {
    char *title_utf8;
    char *title_out;
    unsigned long *desktop;
    int x, y, junkx, junky;
    unsigned int wwidth, wheight, bw, depth;
    Window junkroot;

    /* Get window title */
    title_utf8 = get_window_title(display, client_list[i]); /* UTF8 */
    title_out = get_output_str(title_utf8, true);

    /* Desktop ID */
    desktop = (unsigned long *) get_property(display,
                                             client_list[i],
                                             XA_CARDINAL,
                                             "_NET_WM_DESKTOP",
                                             NULL);

    if (desktop == NULL) {
      desktop = (unsigned long *) get_property(display,
                                               client_list[i],
                                               XA_CARDINAL,
                                               "_WIN_WORKSPACE",
                                               NULL);
    }

	  /* Geometry */
    XGetGeometry( display,
                  client_list[i],
                 &junkroot,
                 &junkx,
                 &junky,
                 &wwidth,
                 &wheight,
                 &bw,
                 &depth);

    /* Keep the titlebar in the window height. */
    wheight = wheight + junky;
    junky = 0;

    XTranslateCoordinates( display,
                           client_list[i],
                           junkroot,
                           junkx,
                           junky,
                          &x,
                          &y,
                          &junkroot);

    /* Cut title max length to 30 characters */
    if (strlen(title_out) > 30) {
      title_out[30] = '\0';
    }

    if (asprintf(&name, "%s", title_out) == -1) {
      exit(0);
    }

    window->name = name;
    window->x = x;
    window->y = y;
    window->w = wwidth;
    window->h = wheight;

    /* Avoids creating a new window that won't be used at end of loop */
    if (i < client_list_size / sizeof(Window) - 1) {
      window->next = new_window();
    }

    window = window->next;

    free(desktop);
    free(title_out);
    free(title_utf8);
    desktop = NULL;
    title_out = NULL;
    title_utf8 = NULL;
    name = NULL;
  }

  free(client_list);
  XCloseDisplay(display);
  client_list = NULL;
  display = NULL;
}

