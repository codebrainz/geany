#ifndef GEANY_FTPLUGINS_H
#define GEANY_FTPLUGINS_H

#include "filetypes.h" // GeanyFiletypeID
#include <glib.h>

G_BEGIN_DECLS


struct GeanyDocument;
struct GeanyPlugin;


/** Features which can be provided by plugins. */
typedef enum
{
	GEANY_FILETYPE_FEATURE_AUTOCOMPLETE,  //!< Auto-completion
	GEANY_FILETYPE_FEATURE_CALLTIPS,      //!< Calltips
	GEANY_FILETYPE_FEATURE_HIGHLIGHTING,  //!< Syntax highlighting
	GEANY_FILETYPE_FEATURE_SYMBOLTREE,    //!< Sidebar symbols
	GEANY_NUM_FILETYPE_FEATURES
}
GeanyFiletypeFeature;


/** Generic function pointer used to store specific feature callbacks. */
typedef void (*GeanyFtPluginFunc)(struct GeanyPlugin *plugin);


/** Type of a function that provides syntax highlighting.
 *
 * @plugin The plugin that registered to provide highlighting.
 * @doc The GeanyDocument that needs highlighting.
 * @start The first position in the document that needs highlighting.
 * @end The last position in the document that needs highlighting.
 * @user_data The data pointer passed to geany_ftplugin_register_provider.
 *
 * @return `TRUE` if the provider performed syntax highlighting, or
 * `FALSE` to let another plugin (or the default code) handle it.
 */
typedef gboolean (*GeanyHighlightFunc)(struct GeanyPlugin *plugin,
	struct GeanyDocument *doc, guint start, guint end, gpointer user_data);


#ifdef GEANY_PRIVATE
void geany_ftplugin_init(void);
void geany_ftplugin_finalize(void);
gboolean geany_ftplugin_have_provider(GeanyFiletypeID ft,
	GeanyFiletypeFeature feature);
void geany_ftplugin_unload(struct GeanyPlugin *plugin);
gboolean geany_ftplugin_highlight(GeanyFiletypeID ft, GeanyFiletypeFeature feature,
	struct GeanyDocument *doc, guint start, guint end);
#endif


gboolean geany_ftplugin_register_provider(struct GeanyPlugin *plugin,
	GeanyFiletypeID ft, GeanyFiletypeFeature feature, GeanyFtPluginFunc func,
	gpointer user_data, GDestroyNotify user_data_free);


G_END_DECLS

#endif // GEANY_FTPLUGINS_H
