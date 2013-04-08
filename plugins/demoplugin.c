/*
 *      demoplugin.c - this file is part of Geany, a fast and lightweight IDE
 *
 *      Copyright 2007-2012 Enrico Tröger <enrico(dot)troeger(at)uvena(dot)de>
 *      Copyright 2007-2012 Nick Treleaven <nick(dot)treleaven(at)btinternet(dot)com>
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

/**
 * Demo plugin - example of a basic plugin for Geany. Adds a menu item to the
 * Tools menu.
 *
 * Note: This is not installed by default, but (on *nix) you can build it as follows:
 * cd plugins
 * make demoplugin.so
 *
 * Then copy or symlink the plugins/demoplugin.so file to ~/.config/geany/plugins
 * - it will be loaded at next startup.
 */


#include "geanyplugin.h"	/* plugin API, always comes first */
#include "Scintilla.h"	/* for the SCNotification struct */


/* Initializes plugin to be loaded by Geany, checking for a minimum
 * version and supplying plugin information such as name, description,
 * version and author. */
PLUGIN_REGISTER(
	1, 24, 0,						/* Minimum Geany version that has the needed API */
	"Demo",							/* Name of the plugin */
	"Example plugin.",				/* Description of the plugin */
	"0.1",							/* Version number string of the plugin */
	_("The Geany developer team")	/* Author of the plugin */
);


static gboolean on_editor_notify(GObject *object, GeanyEditor *editor,
	SCNotification *nt, GeanyPlugin *plugin)
{
	/* For detailed documentation about the SCNotification struct, please see
	 * http://www.scintilla.org/ScintillaDoc.html#Notifications. */
	switch (nt->nmhdr.code)
	{
		case SCN_UPDATEUI:
			/* This notification is sent very often, you should not do time-consuming tasks here */
			break;
		case SCN_CHARADDED:
			/* For demonstrating purposes simply print the typed character in the status bar */
			ui_set_statusbar(FALSE, _("Typed character: %c"), nt->ch);
			break;
		case SCN_URIDROPPED:
		{
			/* Show a message dialog with the dropped URI list when files (i.e. a list of
			 * filenames) is dropped to the editor widget) */
			if (nt->text != NULL)
			{
				GtkWidget *dialog;

				dialog = gtk_message_dialog_new(
					GTK_WINDOW(plugin->data->main_widgets->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_OK,
					_("The following files were dropped:"));
				gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
					"%s", nt->text);

				gtk_dialog_run(GTK_DIALOG(dialog));
				gtk_widget_destroy(dialog);
			}
			/* we return TRUE here which prevents Geany from processing the SCN_URIDROPPED
			 * notification, i.e. Geany won't open the passed files */
			return TRUE;
		}
	}

	return FALSE;
}


/* Callback when the menu item is clicked. */
static void
item_activate(GeanyPlugin *plugin, GtkMenuItem *menuitem)
{
	GtkWidget *dialog;
	gchar *welcome_text;

	welcome_text = plugin_get_field(plugin, "welcome-text");

	dialog = gtk_message_dialog_new(
		GTK_WINDOW(plugin->data->main_widgets->window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"%s", welcome_text);

	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
		_("(From the %s plugin)"), plugin->info->name);

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}


/* Called by Geany to initialize the plugin. */
gboolean plugin_load(GeanyPlugin *plugin)
{
	GtkWidget *demo_item;

	/* Add an item to the Tools menu */
	demo_item = gtk_menu_item_new_with_mnemonic(_("_Demo Plugin"));
	gtk_widget_show(demo_item);
	gtk_container_add(GTK_CONTAINER(plugin->data->main_widgets->tools_menu), demo_item);
	g_signal_connect_swapped(demo_item, "activate", G_CALLBACK(item_activate), plugin);

	/* make the menu item sensitive only when documents are open */
	ui_add_document_sensitive(demo_item);
	/* keep a pointer to the menu item, so we can remove it when the plugin is unloaded */

	/* Add the new menu item and the welcome text as fields of the plugin
	 * to be able to access them in other functions. Also provide destruction
	 * functions to be called when the plugin is unloaded (or the field is re-set). */
	plugin_set_field(plugin, "main-menu-item", demo_item, (GDestroyNotify) gtk_widget_destroy);
	plugin_set_field(plugin, "welcome-text", g_strdup(_("Hello World")), g_free);

	/* Connect to the Scintilla editor-notify signals
	 * Note: this signal is sent before the default handler no matter
	 * the value of the 'after' argument (4th arg), so this callback
	 * handler has the ability by default to prevent Geany from
	 * receiving the event itself. Use with care. */
	plugin_signal_connect(plugin, NULL, "editor-notify", FALSE,
		G_CALLBACK(on_editor_notify), plugin);

	return TRUE; /* Tell Geany everything went OK */
}


/* Callback connected in plugin_configure_begin(). */
static void
on_configure_response(GeanyPlugin *plugin, gint response, GtkDialog *dialog)
{
	/* catch OK or Apply clicked */
	if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY)
	{
		/* In plugin_configure() we set this GtkEntry as a field on the plugin,
		 * now we get back the pointer to it. */
		GtkWidget *entry = plugin_get_field(plugin, "welcome-text-entry");

		/* Over-writing the welcome text here free's the previously set
		 * welcome text automatically. */
		plugin_set_field(plugin, "welcome-text",
			g_strdup(gtk_entry_get_text(GTK_ENTRY(entry))), g_free);

		/* maybe the plugin should write here the settings into a file
		 * (e.g. using GLib's GKeyFile API)
		 * all plugin specific files should be created in:
		 * plugin->data->app->configdir G_DIR_SEPARATOR_S plugins G_DIR_SEPARATOR_S pluginname G_DIR_SEPARATOR_S
		 * e.g. this could be: ~/.config/geany/plugins/Demo/, please use plugin->data->app->configdir */
	}
}


/* Called by Geany to show the plugin's configure dialog. This function is always called after
 * plugin_init() or plugin_load() was called.
 * You can omit this function if the plugin doesn't need to be configured.
 * Note: dialog is the dialog that the returned page will be added to, you
 * can use it to connect to it's response signal or use it as a transient
 * parent for another window. */
GtkWidget *plugin_configure_begin(GeanyPlugin *plugin, GtkDialog *dialog)
{
	GtkWidget *label, *entry, *vbox;
	gchar *welcome_text;

	/* example configuration dialog */
	vbox = gtk_vbox_new(FALSE, 6);

	/* add a label and a text entry to the dialog */
	label = gtk_label_new(_("Welcome text to show:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	entry = gtk_entry_new();

	/* Look up the existing welcome text (if any) and set it as the
	 * default in the entry. */
	welcome_text = plugin_get_field(plugin, "welcome-text");
	if (welcome_text != NULL)
		gtk_entry_set_text(GTK_ENTRY(entry), welcome_text);

	gtk_container_add(GTK_CONTAINER(vbox), label);
	gtk_container_add(GTK_CONTAINER(vbox), entry);

	gtk_widget_show_all(vbox);

	/* We'll need this in the callback handler so store it a field.
	 * Note that passing NULL as the free_func will prevent any
	 * destruction of the field's value when it's over-written or
	 * the plugin is unloaded. */
	plugin_set_field(plugin, "welcome-text-entry", entry, NULL);

	/* Connect a callback for when the user clicks a dialog button passing
	 * the GeanyPlugin as user_data. */
	g_signal_connect_swapped(dialog, "response",
		G_CALLBACK(on_configure_response), plugin);

	return vbox;
}


/* Called by Geany before unloading the plugin.
 * Here any UI changes should be removed, memory freed and any other finalization done.
 * Be sure to leave Geany as it was before plugin_init() or plugin_load()
 * was called. */
gboolean plugin_unload(GeanyPlugin *plugin)
{
	/* We don't need to do anything here like destroy the main-menu-item
	 * added or the stored welcome-text because they are set as fields
	 * on the plugin with destruction functions and so will be freed
	 * automatically. */
	return TRUE; /* Tell Geany it's OK to unload the plugin. */
}
