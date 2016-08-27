/* Abstract base class for all plugins which provide the highlighting feature */

#ifndef GEANY_HIGHLIGHTER_H
#define GEANY_HIGHLIGHTER_H

#include "featureprovider.h"

G_BEGIN_DECLS


#define GEANY_TYPE_HIGHLIGHTER (geany_highlighter_get_type())


G_DECLARE_DERIVABLE_TYPE(GeanyHighlighter, geany_highlighter, GEANY, HIGHLIGHTER, GeanyFeatureProvider)


struct _GeanyHighlighterClass
{
	GObjectClass parent_class;

	void (*init_styles) (GeanyHighlighter *self, guint start_id);
	void (*highlight) (GeanyHighlighter *self, guint start, guint end);
};


#ifdef GEANY_PRIVATE

void geany_highlighter_init_styles(GeanyHighlighter *self, guint start_id);
void geany_highlighter_highlight(GeanyHighlighter *self, guint start, guint end);

#endif


G_END_DECLS

#endif // GEANY_HIGHLIGHTER_H
