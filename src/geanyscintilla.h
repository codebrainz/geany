#ifndef GEANYSCINTILLA_H_
#define GEANYSCINTILLA_H_ 1

#include <gtk/gtk.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <ScintillaWidget.h>

G_BEGIN_DECLS


#define GEANY_TYPE_SCINTILLA            (geany_scintilla_get_type())
#define GEANY_SCINTILLA(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GEANY_TYPE_SCINTILLA, GeanyScintilla))
#define GEANY_SCINTILLA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GEANY_TYPE_SCINTILLA, GeanyScintillaClass))
#define GEANY_IS_SCINTILLA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GEANY_TYPE_SCINTILLA))
#define GEANY_IS_SCINTILLA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GEANY_TYPE_SCINTILLA))
#define GEANY_SCINTILLA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GEANY_TYPE_SCINTILLA, GeanyScintillaClass))

typedef struct GeanyScintilla_        GeanyScintilla;
typedef struct GeanyScintillaClass_   GeanyScintillaClass;
typedef struct GeanyScintillaPrivate_ GeanyScintillaPrivate;

struct GeanyScintilla_
{
	ScintillaObject parent;
	GeanyScintillaPrivate *priv;
};

struct GeanyScintillaClass_
{
	ScintillaClass parent_class;
};


GType geany_scintilla_get_type(void);

GtkWidget *geany_scintilla_new(void);

G_END_DECLS

#endif /* GEANYSCINTILLA_H_ */
