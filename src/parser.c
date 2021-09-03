/* parser.c
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *
parse_name (char *string)
{
  /* Return substring preceding "id" and clip trailing white space. */
  char id[] = "\t";
  char *token;
  int   length;
  int   i;
  char *str;
  char *name;

  if (asprintf(&str, "%s", string) == -1) {
    exit(0);
  }

  token = strtok(str, id);

  // Clip trailing whitespace
  length = strlen(token) - 1;
  for (i = length; i > 1; i--) {
    if (isspace(token[i])) {
      token[i] = '\0';
    } else {
      break;
    }
  }

  if (asprintf(&name, "%s", token) == -1) {
    exit(0);
  }
  free(str);
  token = NULL;
  return name;
}


char *
parse_id (char *string)
{
  char token[] = "\t";
  char *sub;
  char *str;
  char  id_str[3];
  int   j = 0;
  char *id;

  if (asprintf(&str, "%s", string) == -1) {
    exit(0);
  }

  sub = strtok(str, token);
  sub = strtok(NULL, token);

  for (int i = 4; i < 6; i++) {
    id_str[j] = sub[i];
    j++;
  }

  id_str[2] = '\0';

  if (asprintf(&id, "%s", id_str) == -1) {
        exit(0);
  }

  free(str);
  sub = NULL;
  return id;
}


char *
parse_type (char *string)
{
  char token[] = "\t";
  char *str;
  char *sub;
  int   j = 0;
  int   length;
  char *type;

  if (asprintf(&str, "%s", string) == -1) {
    exit(0);
  }

  sub = strtok(str, token);
  sub = strtok(NULL, token);
  sub = strtok(NULL, token);
  length = (int) strlen(sub);

  char type_str[length];
  length--;

  // Clip trailing whitespace
  for (int i = length; i > 1; i--) {
    if (isspace(sub[i])) {
      sub[i] = '\0';
    } else {
      break;
    }
  }

  for (int i = 6; i < length; i++) {
    type_str[j] = sub[i];
    j++;
  }

  type_str[length] = '\0';

  if (asprintf(&type, "%s", type_str) == -1) {
    exit(0);
  }

  free(str);
  sub = NULL;
  return type;
}
