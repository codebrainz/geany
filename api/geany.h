#ifndef GEANY_H__
#define GEANY_H__

#include <glib.h>
G_BEGIN_DECLS
struct GeanyDocument;
G_END_DECLS

#include <gtk/gtk.h>

#ifndef GTK
# define GTK
#endif
#include "Scintilla.h"
#include "SciLexer.h"
#include "ScintillaWidget.h"

typedef struct _ScintillaClass ScintillaObjectClass;

extern GObject *geany_object;
struct _GeanyObject;
#define geany_object_get_default() ((struct _GeanyObject*)(geany_object))

#include "geanyapi.h"

#endif // GEANY_H__
