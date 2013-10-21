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

typedef enum
{
	GEANY_SCINTILLA_EDGE_MODE_NONE,
	GEANY_SCINTILLA_EDGE_MODE_LINE,
	GEANY_SCINTILLA_EDGE_MODE_BACKGROUND
}
GeanyScintillaEdgeMode;


GType geany_scintilla_get_type(void);
GtkWidget *geany_scintilla_new(void);

gboolean geany_scintilla_get_line_numbers_visible(GeanyScintilla *sci);
void geany_scintilla_set_line_numbers_visible(GeanyScintilla *sci, gboolean visible);

GeanyScintillaEdgeMode geany_scintilla_get_edge_mode(GeanyScintilla *sci);
void geany_scintilla_set_edge_mode(GeanyScintilla *sci, GeanyScintillaEdgeMode edge_mode);

guint geany_scintilla_get_edge_column(GeanyScintilla *sci);
void geany_scintilla_set_edge_column(GeanyScintilla *sci, guint column);

void geany_scintilla_get_edge_color(GeanyScintilla *sci, GdkColor *color);
void geany_scintilla_set_edge_color(GeanyScintilla *sci, const GdkColor *color);

const gchar *geany_scintilla_get_text(GeanyScintilla *sci);
void geany_scintilla_get_text_string(GeanyScintilla *sci, GString *out_string);
const gchar *geany_scintilla_get_text_range(GeanyScintilla *sci, guint start, guint end);
void geany_scintilla_get_text_range_string(GeanyScintilla *sci, GString *out_string,
	guint start, guint end);
void geany_scintilla_set_text(GeanyScintilla *sci, const gchar *text);
guint geany_scintilla_get_text_length(GeanyScintilla *sci);
void geany_scintilla_insert_text_length(GeanyScintilla *sci, gssize pos, gssize len, const gchar *text);
#define geany_scintilla_insert_text(sci, pos, text) \
	geany_scintilla_insert_text_length(sci, pos, -1, text)
#define geany_scintilla_add_text(sci, text) geany_scintilla_insert_text(sci, -1, text)

G_END_DECLS

#endif /* GEANYSCINTILLA_H_ */
