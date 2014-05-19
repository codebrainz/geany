#ifndef GEANY_APP_H
#define GEANY_APP_H 1

#include <glib.h>
#include "tm_tag.h" /* FIXME: should be included in tm_workspace.h */
#include "tm_workspace.h"
#include "project.h"

G_BEGIN_DECLS

/** Important application fields. */
typedef struct GeanyApp
{
	gboolean			debug_mode;		/**< @c TRUE if debug messages should be printed. */
	/** User configuration directory, usually @c ~/.config/geany.
	 * This is a full path read by @ref tm_get_real_path().
	 * @note Plugin configuration files should be saved as:
	 * @code g_build_path(G_DIR_SEPARATOR_S, geany->app->configdir, "plugins", "pluginname",
	 * 	"file.conf", NULL); @endcode */
	gchar				*configdir;
	gchar				*datadir;
	gchar				*docdir;
	const TMWorkspace	*tm_workspace;	/**< TagManager workspace/session tags. */
	struct GeanyProject	*project;		/**< Currently active project or @c NULL if none is open. */
}
GeanyApp;

extern GeanyApp *app;

G_END_DECLS

#endif /* GEANY_APP_H */
