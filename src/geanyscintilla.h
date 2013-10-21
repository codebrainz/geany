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

typedef enum
{
	GEANY_SCINTILLA_EOL_MODE_CRLF,
	GEANY_SCINTILLA_EOL_MODE_CR,
	GEANY_SCINTILLA_EOL_MODE_LF
}
GeanyScintillaEOLMode;

typedef enum
{
	GEANY_SCINTILLA_MARGIN_LINE_NUMBERS=0,
	GEANY_SCINTILLA_MARGIN_FOLD=2
}
GeanyScintillaMargin;


GType geany_scintilla_get_type(void);
GtkWidget *geany_scintilla_new(void);

gboolean geany_scintilla_get_line_numbers_visible(GeanyScintilla *sci);
void geany_scintilla_set_line_numbers_visible(GeanyScintilla *sci, gboolean visible);

gboolean geany_scintilla_get_code_folding_visible(GeanyScintilla *sci);
void geany_scintilla_set_code_folding_visible(GeanyScintilla *sci, gboolean visible);

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

gboolean geany_scintilla_get_can_undo(GeanyScintilla *sci);
gboolean geany_scintilla_get_can_redo(GeanyScintilla *sci);
void geany_scintilla_undo(GeanyScintilla *sci);
void geany_scintilla_redo(GeanyScintilla *sci);
void geany_scintilla_begin_undo_action(GeanyScintilla *sci);
void geany_scintilla_end_undo_action(GeanyScintilla *sci);
gboolean geany_scintilla_get_enable_undo_collection(GeanyScintilla *sci);
void geany_scintilla_set_enable_undo_collection(GeanyScintilla *sci, gboolean enable);

gboolean geany_scintilla_get_modified(GeanyScintilla *sci);

gint geany_scintilla_get_zoom_level(GeanyScintilla *sci);
void geany_scintilla_set_zoom_level(GeanyScintilla *sci, gint zoom_level);
void geany_scintilla_zoom_in(GeanyScintilla *sci);
void geany_scintilla_zoom_out(GeanyScintilla *sci);
#define geany_scintilla_zoom_reset(sci) \
	geany_scintilla_set_zoom_level(sci, 0)

GeanyScintillaEOLMode geany_scintilla_get_eol_mode(GeanyScintilla *sci);
void geany_scintilla_set_eol_mode(GeanyScintilla *sci, GeanyScintillaEOLMode eol_mode);
void geany_scintilla_convert_eols(GeanyScintilla *sci, GeanyScintillaEOLMode eol_mode);
gboolean geany_scintilla_get_eol_visible(GeanyScintilla *sci);
void geany_scintilla_set_eol_visible(GeanyScintilla *sci, gboolean eol_visible);

gboolean geany_scintilla_get_whitespace_visible(GeanyScintilla *sci);
void geany_scintilla_set_whitespace_visible(GeanyScintilla *sci, gboolean visible);

void geany_scintilla_cut(GeanyScintilla *sci);
void geany_scintilla_copy(GeanyScintilla *sci);
void geany_scintilla_paste(GeanyScintilla *sci);
void geany_scintilla_clear(GeanyScintilla *sci);

gboolean geany_scintilla_get_use_tabs(GeanyScintilla *sci);
void geany_scintilla_set_use_tabs(GeanyScintilla *sci, gboolean use_tabs);
gboolean geany_scintilla_get_tab_indents(GeanyScintilla *sci);
void geany_scintilla_set_tab_indents(GeanyScintilla *sci, gboolean tab_indents);
gboolean geany_scintilla_get_backspace_unindents(GeanyScintilla *sci);
void geany_scintilla_set_backspace_unindents(GeanyScintilla *sci, gboolean unindents);

gboolean geany_scintilla_get_enable_overtype(GeanyScintilla *sci);
void geany_scintilla_set_enable_overtype(GeanyScintilla *sci, gboolean overtype);

gboolean geany_scintilla_get_has_selection(GeanyScintilla *sci);

gboolean geany_scintilla_get_read_only(GeanyScintilla *sci);
void geany_scintilla_set_read_only(GeanyScintilla *sci, gboolean read_only);

gboolean geany_scintilla_get_line_wrapping_enabled(GeanyScintilla *sci);
void geany_scintilla_set_line_wrapping_enabled(GeanyScintilla *sci, gboolean enabled);

void geany_scintilla_goto_position(GeanyScintilla *sci, guint pos, gboolean unfold);
void geany_scintilla_goto_line(GeanyScintilla *sci, guint line);

G_END_DECLS

#endif /* GEANYSCINTILLA_H_ */
