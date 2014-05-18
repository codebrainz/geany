#ifndef GEANY_KEYBINDINGS_PRIVATE_H
#define GEANY_KEYBINDINGS_PRIVATE_H 1

#include <glib.h>
#include "keybindings.h"

G_BEGIN_DECLS

struct GeanyKeyGroup
{
	const gchar *name;		/* Group name used in the configuration file, such as @c "html_chars" */
	const gchar *label;		/* Group label used in the preferences dialog keybindings tab */
	GeanyKeyGroupCallback callback;	/* use this or individual keybinding callbacks */
	gboolean plugin;		/* used by plugin */
	GPtrArray *key_items;	/* pointers to GeanyKeyBinding structs */
	gsize plugin_key_count;			/* number of keybindings the group holds */
	GeanyKeyBinding *plugin_keys;	/* array of GeanyKeyBinding structs */
};

G_END_DECLS

#endif /* GEANY_KEYBINDINGS_PRIVATE_H */
