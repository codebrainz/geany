#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_PEAS

#include "extensions.h"
#include <libpeas/peas.h>
#include <libpeas-gtk/peas-gtk.h>

#if !defined(GEANY_EXT_MODULE_DIR) || !defined(GEANY_EXT_DATA_DIR)
# error "GEANY_EXT_MODULE_DIR and GEANY_EXT_DATA_DIR must be defined"
#endif


typedef struct GeanyDocument GeanyDocument; // for ui_utils.h include
#include "ui_utils.h" // for ui_builder_get_object()
#include "geany.h" // for geany_debug

static PeasEngine *ext_engine = NULL;


void ext_init(void)
{
	if (!ext_engine)
	{
		ext_engine = peas_engine_get_default();

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
	}
}


void ext_finalize(void)
{
	if (ext_engine)
		g_object_unref(ext_engine);
}

#else // !HAVE_PEAS

void ext_init(void) {}
void ext_finalize(void) {}

#endif // HAVE_PEAS
