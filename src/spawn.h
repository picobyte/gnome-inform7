/*  Copyright 2007 P.F. Chimento
 *  This file is part of GNOME Inform 7.
 * 
 *  GNOME Inform 7 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  GNOME Inform 7 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNOME Inform 7; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _SPAWN_H
#define _SPAWN_H

#include <gnome.h>

typedef void IOHookFunc(gpointer, gchar *);

GPid run_command(const gchar *wd, gchar **argv, GtkTextBuffer *output);
void set_up_io_channel(gint fd, GtkTextBuffer *output);
gboolean write_channel_to_buffer(GIOChannel *ioc, GIOCondition cond,
  gpointer buffer);
GPid run_command_hook(const gchar *wd, gchar **argv, GtkTextBuffer *output,
  IOHookFunc *callback, gpointer data, gboolean get_out, gboolean get_err);
void set_up_io_channel_hook(gint fd, GtkTextBuffer *output,
  IOHookFunc *callback, gpointer data);
gboolean write_channel_hook(GIOChannel *ioc, GIOCondition cond,
  gpointer data);

#endif /* _SPAWN_H */