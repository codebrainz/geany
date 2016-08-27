#ifndef GEANY_HIGHLIGHTER_H
#define GEANY_HIGHLIGHTER_H

#include "featureprovider.h"

G_BEGIN_DECLS


#define GEANY_TYPE_HIGHLIGHTER (geany_highlighter_get_type())
G_DECLARE_INTERFACE(GeanyHighlighter, geany_highlighter, GEANY, HIGHLIGHTER, GeanyFeatureProvider)


struct _GeanyHighlighterInterface
{
	GeanyFeatureProviderInterface parent_iface;

	void (*init_styles) (GeanyHighlighter *self, guint start_id);
	void (*highlight) (GeanyHighlighter *self, guint start, guint end);
};


void geany_highlighter_init_styles(GeanyHighlighter *self, guint start_id);
void geany_highlighter_highlight(GeanyHighlighter *self, guint start, guint end);


G_END_DECLS

#endif // GEANY_HIGHLIGHTER_H
