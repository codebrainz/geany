#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_PEAS

#include "wincmd.h"
#include "extensions.h"
#include <libpeas/peas.h>
#include <libpeas-gtk/peas-gtk.h>

#if !defined(GEANY_EXT_MODULE_DIR) || !defined(GEANY_EXT_DATA_DIR)
# error "GEANY_EXT_MODULE_DIR and GEANY_EXT_DATA_DIR must be defined"
#endif


typedef struct GeanyDocument GeanyDocument; // for ui_utils.h include
#include "ui_utils.h" // for ui_builder_get_object()
#include "geany.h" // for geany_debug and geany_object
#include "geanyobject.h" // for GeanyObject

static PeasEngine *ext_engine = NULL;
static PeasExtensionSet *ext_set = NULL;

static void on_ext_added(PeasExtensionSet *ext_set, PeasPluginInfo *info,
	PeasExtension *ext, GtkWindow *win)
{
	geany_win_cmd_activate(GEANY_WIN_CMD(ext));
}


static void on_ext_removed(PeasExtensionSet *ext_set, PeasPluginInfo *info,
	PeasExtension *ext, GtkWindow *win)
{
	geany_win_cmd_deactivate(GEANY_WIN_CMD(ext));
}


static void on_doc_update_state(PeasExtensionSet *ext_set, PeasPluginInfo *info,
	PeasExtension *ext, GeanyDocument *doc)
{
	geany_win_cmd_update_state(GEANY_WIN_CMD(ext));
}


static void on_doc_action(PeasExtensionSet *ext_set, GeanyDocument *doc,
	GeanyObject *unused)
{
	peas_extension_set_foreach(ext_set, (PeasExtensionSetForeachFunc) on_doc_update_state, doc);
}


static void on_doc_action_ft(PeasExtensionSet *ext_set, GeanyDocument *doc,
	GeanyFiletype *old_ft, GeanyObject *unused)
{
	peas_extension_set_foreach(ext_set, (PeasExtensionSetForeachFunc) on_doc_update_state, doc);
}


void ext_init(void)
{
	if (!ext_engine)
	{
		ext_engine = peas_engine_get_default();

		// TODO: enable any loaders needed

		// Add user search path
		gchar *mod_dir = g_build_filename(g_get_user_config_dir(), "geany", "extensions", NULL);
		gchar *data_dir = g_build_filename(g_get_user_data_dir(), "geany", "extensions", NULL);
		geany_debug("User extension module dir: %s", mod_dir);
		geany_debug("User extension data dir: %s", data_dir);
		peas_engine_add_search_path(ext_engine, mod_dir, data_dir);
		g_free(mod_dir);
		g_free(data_dir);

		// Add system-wide search path
		peas_engine_add_search_path(ext_engine, GEANY_EXT_MODULE_DIR, GEANY_EXT_DATA_DIR);
		geany_debug("System extension module dir: %s", GEANY_EXT_MODULE_DIR);
		geany_debug("System extension data dir: %s", GEANY_EXT_DATA_DIR);

		// Add the peas plugin manager UI to the prefs dialog
		GtkWidget *ext_manager = peas_gtk_plugin_manager_new(ext_engine);
		GObject *align_obj = ui_builder_get_object("ext_manage_alignment");
		g_assert(G_IS_OBJECT(align_obj));
		GtkContainer *ext_align = GTK_CONTAINER(align_obj);
		g_assert(GTK_IS_CONTAINER(ext_align));
		gtk_container_add(ext_align, ext_manager);
		gtk_widget_show_all(GTK_WIDGET(align_obj));

		// Add the extensions set to the main window as data since we don't
		// currently subclass GtkWindow to add a property.
		GObject *main_win = ui_builder_get_object("window1");
		g_assert(GTK_IS_WINDOW(main_win));
		ext_set = peas_extension_set_new(ext_engine,
			GEANY_TYPE_WIN_CMD, "window", GTK_WINDOW(main_win), NULL);
		g_object_set_data(main_win, "extensions", ext_set);
		g_signal_connect(ext_set, "extension-added", G_CALLBACK(on_ext_added), main_win);
		g_signal_connect(ext_set, "extension-removed", G_CALLBACK(on_ext_removed), main_win);

		// Activate initially loaded extensions
		peas_extension_set_foreach(ext_set,
			(PeasExtensionSetForeachFunc) on_ext_added, main_win);

		// We want to watch for document-related events so we can call the
		// extensions' update_state method.
		g_signal_connect_swapped(geany_object, "document-activate",
			G_CALLBACK(on_doc_action), ext_set);
		g_signal_connect_swapped(geany_object, "document-new",
			G_CALLBACK(on_doc_action), ext_set);
		g_signal_connect_swapped(geany_object, "document-open",
			G_CALLBACK(on_doc_action), ext_set);
		g_signal_connect_swapped(geany_object, "document-reload",
			G_CALLBACK(on_doc_action), ext_set);
		g_signal_connect_swapped(geany_object, "document-activate",
			G_CALLBACK(on_doc_action), ext_set);
		g_signal_connect_swapped(geany_object, "document-close",
			G_CALLBACK(on_doc_action), ext_set);
		g_signal_connect_swapped(geany_object, "document-filetype-set",
			G_CALLBACK(on_doc_action_ft), ext_set);
	}
}


void ext_finalize(void)
{
	if (ext_engine)
	{
		peas_extension_set_foreach(ext_set,
			(PeasExtensionSetForeachFunc) on_ext_removed, NULL);
		GObject *main_win = ui_builder_get_object("window1");
		g_object_unref(g_object_get_data(main_win, "extensions"));
		g_object_unref(ext_engine);
	}
}

#else // !HAVE_PEAS

void ext_init(void) {}
void ext_finalize(void) {}

#endif // HAVE_PEAS
