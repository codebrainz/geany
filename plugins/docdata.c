/*
 * docdata.c - this file is part of Geany, a fast and lightweight IDE
 *
 * Copyright 2016 Matthew Brush <matt@geany.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "geanyplugin.h"


#define KEY_NAME "docdata-test"


struct DocumentData
{
	gchar *message;
};


static struct DocumentData *docdata_new(const gchar *message)
{
	struct DocumentData *data = g_slice_new(struct DocumentData);
	if (data != NULL)
	{
		data->message = g_strdup(message);
		g_debug("docdata allocated with message '%s'", data->message);
	}
	return data;
}


static void docdata_free(gpointer pdata)
{
	struct DocumentData *data = pdata;
	if (data != NULL)
	{
		g_debug("docdata with message '%s' freed", data->message);
		g_free(data->message);
		g_slice_free(struct DocumentData, data);
	}
}


static void on_document_open(GObject *unused, GeanyDocument *doc, GeanyPlugin *plugin)
{
	plugin_set_document_data_full(plugin, doc, KEY_NAME, docdata_new("docdata"), docdata_free);
}


static void on_document_save(GObject *unused, GeanyDocument *doc, GeanyPlugin *plugin)
{
	struct DocumentData *data = plugin_get_document_data(plugin, doc, KEY_NAME);
	if (data != NULL)
	{
		g_debug("docdata message for key '%s' attached to document '%s': %s",
			KEY_NAME, doc->file_name, data->message);
	}
}


static gboolean docdata_init(GeanyPlugin *plugin, gpointer unused)
{
	plugin_signal_connect(plugin, NULL, "document-open", TRUE, G_CALLBACK(on_document_open), plugin);
	plugin_signal_connect(plugin, NULL, "document-new", TRUE, G_CALLBACK(on_document_open), plugin);
	plugin_signal_connect(plugin, NULL, "document-save", TRUE, G_CALLBACK(on_document_save), plugin);
	return TRUE;
}


static void docdata_cleanup(GeanyPlugin *plugin, gpointer unused)
{
}


G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin)
{
	plugin->info->name = "Document Data";
	plugin->info->description = "Tests docdata";
	plugin->info->version = "1.0";
	plugin->info->author = "Matthew Brush <matt@geany.org>";

	plugin->funcs->init = docdata_init;
	plugin->funcs->cleanup = docdata_cleanup;

	GEANY_PLUGIN_REGISTER(plugin, 228);
}
