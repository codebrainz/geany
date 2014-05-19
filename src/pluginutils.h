/*
 *      pluginutils.h - this file is part of Geany, a fast and lightweight IDE
 *
 *      Copyright 2009-2012 Nick Treleaven <nick(dot)treleaven(at)btinternet(dot)com>
 *      Copyright 2009-2012 Enrico Tröger <enrico(dot)troeger(at)uvena(dot)de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License along
 *      with this program; if not, write to the Free Software Foundation, Inc.,
 *      51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef GEANY_PLUGIN_UTILS_H
#define GEANY_PLUGIN_UTILS_H 1

#ifdef HAVE_PLUGINS

#include "gtkcompat.h"
#include "keybindings.h"	/* GeanyKeyGroupCallback */

G_BEGIN_DECLS

typedef struct GeanyPlugin GeanyPlugin;

void plugin_add_toolbar_item(GeanyPlugin *plugin, GtkToolItem *item);

void plugin_module_make_resident(GeanyPlugin *plugin);

void plugin_signal_connect(GeanyPlugin *plugin,
		GObject *object, const gchar *signal_name, gboolean after,
		GCallback callback, gpointer user_data);

guint plugin_timeout_add(GeanyPlugin *plugin, guint interval, GSourceFunc function,
		gpointer data);

guint plugin_timeout_add_seconds(GeanyPlugin *plugin, guint interval, GSourceFunc function,
		gpointer data);

guint plugin_idle_add(GeanyPlugin *plugin, GSourceFunc function, gpointer data);

GeanyKeyGroup *plugin_set_key_group(GeanyPlugin *plugin,
		const gchar *section_name, gsize count, GeanyKeyGroupCallback callback);

void plugin_show_configure(GeanyPlugin *plugin);

void plugin_builder_connect_signals(GeanyPlugin *plugin,
	GtkBuilder *builder, gpointer user_data);

G_END_DECLS

#endif /* HAVE_PLUGINS */
#endif /* GEANY_PLUGIN_UTILS_H */
