#include "geanyscintilla.h"
#include "geanyenums.h"
#include <string.h>

#define SSM(s, m, w, l) \
	scintilla_send_message(SCINTILLA(s), (guint)(m), (uptr_t)(w), (sptr_t)(l))


struct GeanyScintillaPrivate_
{
	gboolean can_undo;
	gboolean can_redo;
	gboolean modified;
	gboolean undo_collection;
	guint current_pos;
	guint current_line;
};

enum
{
	PROP_0,
	PROP_LINE_NUMBERS_VISIBLE,
	PROP_CODE_FOLDING_VISIBLE,
	PROP_EDGE_MODE,
	PROP_EDGE_COLUMN,
	PROP_EDGE_COLOR,
	PROP_TEXT,
	PROP_CAN_UNDO,
	PROP_CAN_REDO,
	PROP_MODIFIED,
	PROP_UNDO_COLLECTION,
	PROP_ZOOM_LEVEL,
	PROP_EOL_MODE,
	PROP_EOL_VISIBLE,
	PROP_WHITESPACE_VISIBLE,
	PROP_USE_TABS,
	PROP_TAB_INDENTS,
	PROP_BACKSPACE_UNINDENTS,
	PROP_ENABLE_OVERTYPE,
	PROP_READ_ONLY,
	PROP_LINE_WRAPPING_ENABLED,
	PROP_CURRENT_POSITION,
	PROP_CURRENT_LINE,
	N_PROPERTIES
};


static GParamSpec *geany_scintilla_pspecs[N_PROPERTIES] = { NULL };

static void geany_scintilla_finalize(GObject *object);
static void geany_scintilla_set_property(GObject *object, guint prop_id,
	const GValue *value, GParamSpec *pspec);
static void geany_scintilla_get_property(GObject *object, guint prop_id,
	GValue *value, GParamSpec *pspec);

static void on_scintilla_notify(GeanyScintilla *sci, guint id,
	struct SCNotification *notif, gpointer user_data);

// TODO: move to/replace with utils.c version
static void geany_color_from_int(guint32 color, GdkColor *gdk_color);
static guint32 geany_int_from_color(const GdkColor *gdk_color);


static void geany_scintilla_update_line_numbers(GeanyScintilla *sci);
static void geany_scintilla_update_undo_state(GeanyScintilla *sci);
static void geany_scintilla_update_modified_state(GeanyScintilla *sci);
static void geany_scintilla_update_current_pos(GeanyScintilla *sci);


G_DEFINE_TYPE(GeanyScintilla, geany_scintilla, scintilla_get_type())


static void
geany_scintilla_class_init(GeanyScintillaClass *klass)
{
	GObjectClass *g_object_class;

	g_object_class = G_OBJECT_CLASS(klass);

	g_object_class->finalize = geany_scintilla_finalize;
	g_object_class->set_property = geany_scintilla_set_property;
	g_object_class->get_property = geany_scintilla_get_property;

	geany_scintilla_pspecs[PROP_LINE_NUMBERS_VISIBLE] =
		g_param_spec_boolean("line-numbers-visible", "Line Numbers Visible",
			"Whether or not the line number margin is visible", TRUE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_CODE_FOLDING_VISIBLE] =
		g_param_spec_boolean("code-folding-visible", "Code Folding Visible",
			"Whether or not the code folding margin is visible", TRUE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_EDGE_MODE] =
		g_param_spec_enum("edge-mode", "Edge Mode",
			"The edge mode marking long lines", GEANY_TYPE_SCINTILLA_EDGE_MODE,
			GEANY_SCINTILLA_EDGE_MODE_NONE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_EDGE_COLUMN] =
		g_param_spec_uint("edge-column", "Edge Column",
			"The column where the edge marker is shown", 0, G_MAXUINT32, 0,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_EDGE_COLOR] =
		g_param_spec_boxed("edge-color", "Edge Color",
			"The colour of the edge marker", GDK_TYPE_COLOR,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_TEXT] =
		g_param_spec_string("text", "Text", "The text in the editor", "",
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_CAN_UNDO] =
		g_param_spec_boolean("can-undo", "Can Undo",
			"Whether an undo operation can be performed", FALSE,
			G_PARAM_READABLE);

	geany_scintilla_pspecs[PROP_CAN_REDO] =
		g_param_spec_boolean("can-redo", "Can Redo",
			"Whether a redo opertaion can be performed", FALSE,
			G_PARAM_READABLE);

	geany_scintilla_pspecs[PROP_MODIFIED] =
		g_param_spec_boolean("modified", "Is Modified",
			"Whether the text in the editor is modified", FALSE,
			G_PARAM_READABLE);

	geany_scintilla_pspecs[PROP_UNDO_COLLECTION] =
		g_param_spec_boolean("enable-undo-collection", "Enable Undo Collection",
			"Whether to collection undo information", TRUE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_ZOOM_LEVEL] =
		g_param_spec_int("zoom-level", "Zoom Level", "Zoom level of the editor",
			-10, 20, 0, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_EOL_MODE] =
		g_param_spec_enum("eol-mode", "EOL Mode", "The type of end-of-line character(s)",
			GEANY_TYPE_SCINTILLA_EOL_MODE, GEANY_SCINTILLA_EOL_MODE_LF,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_EOL_VISIBLE] =
		g_param_spec_boolean("eol-visible", "EOL Visible",
			"Whether to show end-of-lines", FALSE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_WHITESPACE_VISIBLE] =
		g_param_spec_boolean("whitespace-visible", "Whitespace Visible",
			"Whether to show whitespace", FALSE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_USE_TABS] =
		g_param_spec_boolean("use-tabs", "Use Tab Indentation",
			"Whether to use tabs for indentation", TRUE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_TAB_INDENTS] =
		g_param_spec_boolean("tab-indents", "Tab Indents",
			"Whether tab key inserts indentation inside indentation whitespace",
			TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_BACKSPACE_UNINDENTS] =
		g_param_spec_boolean("backspace-unindents", "Backspace Unindents",
			"Whether the backspace key removes indentation inside indentation whitespace",
			TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_ENABLE_OVERTYPE] =
		g_param_spec_boolean("enable-overtype", "Overtype Enabled",
			"Whether overtype/insert mode is enabled", FALSE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_READ_ONLY] =
		g_param_spec_boolean("read-only", "Read-Only",
			"Whether the document is read-only (locked) or read-write", FALSE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_LINE_WRAPPING_ENABLED] =
		g_param_spec_boolean("line-wrapping-enabled", "Line-Wrapping Enabled",
			"Whether line-wrapping is enabled", FALSE,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_CURRENT_POSITION] =
		g_param_spec_uint("current-position", "Current Caret Position",
			"The position of the caret", 0, G_MAXUINT, 0,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	geany_scintilla_pspecs[PROP_CURRENT_LINE] =
		g_param_spec_uint("current-line", "Current Caret Line",
			"The line where the caret is", 0, G_MAXUINT, 0,
			G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	g_object_class_install_properties(g_object_class, N_PROPERTIES,
		geany_scintilla_pspecs);

	g_type_class_add_private((gpointer)klass, sizeof(GeanyScintillaPrivate));
}


static void
geany_scintilla_finalize(GObject *object)
{
	GeanyScintilla *self;

	g_return_if_fail(GEANY_IS_SCINTILLA(object));

	self = GEANY_SCINTILLA(object);

	G_OBJECT_CLASS(geany_scintilla_parent_class)->finalize(object);
}


static void
geany_scintilla_set_property(GObject *obj, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	GeanyScintilla *sci = GEANY_SCINTILLA(obj);

	switch (prop_id)
	{
		case PROP_LINE_NUMBERS_VISIBLE:
			geany_scintilla_set_line_numbers_visible(sci,
				g_value_get_boolean(value));
			break;
		case PROP_CODE_FOLDING_VISIBLE:
			geany_scintilla_set_code_folding_visible(sci,
				g_value_get_boolean(value));
			break;
		case PROP_EDGE_MODE:
			geany_scintilla_set_edge_mode(sci, g_value_get_enum(value));
			break;
		case PROP_EDGE_COLUMN:
			geany_scintilla_set_edge_column(sci, g_value_get_uint(value));
			break;
		case PROP_EDGE_COLOR:
			geany_scintilla_set_edge_color(sci, g_value_get_boxed(value));
			break;
		case PROP_TEXT:
			geany_scintilla_set_text(sci, g_value_get_string(value));
			break;
		case PROP_UNDO_COLLECTION:
			geany_scintilla_set_enable_undo_collection(sci, g_value_get_boolean(value));
			break;
		case PROP_ZOOM_LEVEL:
			geany_scintilla_set_zoom_level(sci, g_value_get_int(value));
			break;
		case PROP_EOL_MODE:
			geany_scintilla_set_eol_mode(sci, g_value_get_enum(value));
			break;
		case PROP_EOL_VISIBLE:
			geany_scintilla_set_eol_visible(sci, g_value_get_boolean(value));
			break;
		case PROP_WHITESPACE_VISIBLE:
			geany_scintilla_set_whitespace_visible(sci, g_value_get_boolean(value));
			break;
		case PROP_USE_TABS:
			geany_scintilla_set_use_tabs(sci, g_value_get_boolean(value));
			break;
		case PROP_TAB_INDENTS:
			geany_scintilla_set_tab_indents(sci, g_value_get_boolean(value));
			break;
		case PROP_BACKSPACE_UNINDENTS:
			geany_scintilla_set_backspace_unindents(sci, g_value_get_boolean(value));
			break;
		case PROP_ENABLE_OVERTYPE:
			geany_scintilla_set_enable_overtype(sci, g_value_get_boolean(value));
			break;
		case PROP_READ_ONLY:
			geany_scintilla_set_read_only(sci, g_value_get_boolean(value));
			break;
		case PROP_LINE_WRAPPING_ENABLED:
			geany_scintilla_set_line_wrapping_enabled(sci, g_value_get_boolean(value));
			break;
		case PROP_CURRENT_POSITION:
			geany_scintilla_set_current_position(sci, g_value_get_uint(value));
			break;
		case PROP_CURRENT_LINE:
			geany_scintilla_set_current_line(sci, g_value_get_uint(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}


static void
geany_scintilla_get_property(GObject *obj, guint prop_id, GValue *value,
	GParamSpec *pspec)
{
	GeanyScintilla *sci = GEANY_SCINTILLA(obj);

	switch (prop_id)
	{
		case PROP_LINE_NUMBERS_VISIBLE:
			g_value_set_boolean(value,
				geany_scintilla_get_line_numbers_visible(sci));
			break;
		case PROP_CODE_FOLDING_VISIBLE:
			g_value_set_boolean(value,
				geany_scintilla_get_code_folding_visible(sci));
			break;
		case PROP_EDGE_MODE:
			g_value_set_enum(value, geany_scintilla_get_edge_mode(sci));
			break;
		case PROP_EDGE_COLUMN:
			g_value_set_uint(value, geany_scintilla_get_edge_column(sci));
			break;
		case PROP_EDGE_COLOR:
		{
			GdkColor color;
			geany_scintilla_get_edge_color(sci, &color);
			g_value_set_boxed(value, &color);
			break;
		}
		case PROP_TEXT:
			g_value_set_string(value, geany_scintilla_get_text(sci));
			break;
		case PROP_CAN_UNDO:
			g_value_set_boolean(value, geany_scintilla_get_can_undo(sci));
			break;
		case PROP_CAN_REDO:
			g_value_set_boolean(value, geany_scintilla_get_can_redo(sci));
			break;
		case PROP_MODIFIED:
			g_value_set_boolean(value, geany_scintilla_get_modified(sci));
			break;
		case PROP_UNDO_COLLECTION:
			g_value_set_boolean(value, geany_scintilla_get_enable_undo_collection(sci));
			break;
		case PROP_ZOOM_LEVEL:
			g_value_set_int(value, geany_scintilla_get_zoom_level(sci));
			break;
		case PROP_EOL_MODE:
			g_value_set_enum(value, geany_scintilla_get_eol_mode(sci));
			break;
		case PROP_EOL_VISIBLE:
			g_value_set_boolean(value, geany_scintilla_get_eol_visible(sci));
			break;
		case PROP_WHITESPACE_VISIBLE:
			g_value_set_boolean(value, geany_scintilla_get_whitespace_visible(sci));
			break;
		case PROP_USE_TABS:
			g_value_set_boolean(value, geany_scintilla_get_use_tabs(sci));
			break;
		case PROP_TAB_INDENTS:
			g_value_set_boolean(value, geany_scintilla_get_tab_indents(sci));
			break;
		case PROP_BACKSPACE_UNINDENTS:
			g_value_set_boolean(value, geany_scintilla_get_backspace_unindents(sci));
			break;
		case PROP_ENABLE_OVERTYPE:
			g_value_set_boolean(value, geany_scintilla_get_enable_overtype(sci));
			break;
		case PROP_READ_ONLY:
			g_value_set_boolean(value, geany_scintilla_get_read_only(sci));
			break;
		case PROP_LINE_WRAPPING_ENABLED:
			g_value_set_boolean(value, geany_scintilla_get_line_wrapping_enabled(sci));
			break;
		case PROP_CURRENT_POSITION:
			g_value_set_uint(value, geany_scintilla_get_current_position(sci));
			break;
		case PROP_CURRENT_LINE:
			g_value_set_uint(value, geany_scintilla_get_current_line(sci));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}


/* This is the first/main handler for Scintilla notifications, it goes before
 * any other parts of the code and plugins. Only stuff that controls the
 * state of this Scintilla view should be in here, other parts of the code
 * should connect separately. */
static void
on_scintilla_notify(GeanyScintilla *sci, guint id, struct SCNotification *notif,
	gpointer user_data)
{
	switch (notif->nmhdr.code)
	{
		case SCN_MODIFIED:
			if (notif->modificationType & SC_MOD_INSERTTEXT ||
			    notif->modificationType & SC_MOD_DELETETEXT)
			{
				if (notif->linesAdded != 0)
					geany_scintilla_update_line_numbers(sci);

				/* Emit "notify::text" when the buffer text changes */
				g_object_notify_by_pspec(G_OBJECT(sci),
					geany_scintilla_pspecs[PROP_TEXT]);

				geany_scintilla_update_undo_state(sci);
				geany_scintilla_update_current_pos(sci);
			}
			break;
		case SCN_ZOOM:
			geany_scintilla_update_line_numbers(sci);
			g_object_notify_by_pspec(G_OBJECT(sci),
				geany_scintilla_pspecs[PROP_ZOOM_LEVEL]);
			break;
		case SCN_SAVEPOINTLEFT:
		case SCN_SAVEPOINTREACHED:
			geany_scintilla_update_modified_state(sci);
			break;
		default:
			break;
	}
}


static void
geany_scintilla_init(GeanyScintilla *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, GEANY_TYPE_SCINTILLA, GeanyScintillaPrivate);

	/* Connect to the ScintillaObject signal for now because I don't know
	 * how to or if it's even possible to override the base class vfunc
	 * with the type of signature it has in ScintillaClass. */
	g_signal_connect(self, "sci-notify", G_CALLBACK(on_scintilla_notify), NULL);

	SSM(self, SCI_SETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, 16);
	geany_scintilla_update_line_numbers(self);

	GdkColor default_color = {0};
	geany_scintilla_set_edge_color(self, &default_color);

	self->priv->can_undo = SSM(self, SCI_CANUNDO, 0, 0);
	self->priv->can_redo = SSM(self, SCI_CANREDO, 0, 0);
	self->priv->current_pos = SSM(self, SCI_GETCURRENTPOS, 0, 0);
	self->priv->current_line = SSM(self, SCI_GETCURLINE, 0, 0);
}


GtkWidget *
geany_scintilla_new(void)
{
	return g_object_new(GEANY_TYPE_SCINTILLA, NULL);
}


/* Updates the line number margin width based on the number of lines */
static void
geany_scintilla_update_line_numbers(GeanyScintilla *sci)
{
	if (SSM(sci, SCI_GETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, 0))
	{
		gchar tmp_str[15];
		guint num_lines, width;
		num_lines = SSM(sci, SCI_GETLINECOUNT, 0, 0);
		g_snprintf(tmp_str, 15, "_%d", num_lines);
		width = SSM(sci, SCI_TEXTWIDTH, STYLE_LINENUMBER, tmp_str);
		/* use default behaviour */
		SSM(sci, SCI_SETMARGINSENSITIVEN, GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, FALSE);
		SSM(sci, SCI_SETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, width);
	}
}


gboolean
geany_scintilla_get_line_numbers_visible(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return (SSM(sci, SCI_GETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, 0) != 0);
}


void
geany_scintilla_set_line_numbers_visible(GeanyScintilla *sci, gboolean visible)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));

	if (visible != geany_scintilla_get_line_numbers_visible(sci))
	{
		if (!visible)
			SSM(sci, SCI_SETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, 0);
		geany_scintilla_update_line_numbers(sci);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_LINE_NUMBERS_VISIBLE]);
	}
}


gboolean
geany_scintilla_get_code_folding_visible(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return (SSM(sci, SCI_GETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_FOLD, 0) != 0);
}


void
geany_scintilla_set_code_folding_visible(GeanyScintilla *sci, gboolean visible)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (visible != geany_scintilla_get_code_folding_visible(sci))
	{
		SSM(sci, SCI_SETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_FOLD, visible ? 16 : 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_CODE_FOLDING_VISIBLE]);
	}
}


GeanyScintillaEdgeMode
geany_scintilla_get_edge_mode(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), GEANY_SCINTILLA_EDGE_MODE_NONE);
	return SSM(sci, SCI_GETEDGEMODE, 0, 0);
}


void
geany_scintilla_set_edge_mode(GeanyScintilla *sci, GeanyScintillaEdgeMode edge_mode)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));

	if (edge_mode != geany_scintilla_get_edge_mode(sci))
	{
		SSM(sci, SCI_SETEDGEMODE, edge_mode, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_EDGE_MODE]);
	}
}


guint
geany_scintilla_get_edge_column(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), 0);
	return SSM(sci, SCI_GETEDGECOLUMN, 0, 0);
}


void
geany_scintilla_set_edge_column(GeanyScintilla *sci, guint column)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));

	if (column != geany_scintilla_get_edge_column(sci))
	{
		SSM(sci, SCI_SETEDGECOLUMN, column, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_EDGE_COLUMN]);
	}
}


static void
geany_color_from_int(guint32 color, GdkColor *gdk_color)
{
	guint8 r = (color >> 16) & 0xFF;
	guint8 g = (color >> 8) & 0xFF;
	guint8 b = color & 0xFF;
	gdk_color->red = (r / 255.0) * G_MAXUINT16;
	gdk_color->green = (g / 255.0) * G_MAXUINT16;
	gdk_color->blue = (g / 255.0) * G_MAXINT16;
	gdk_color->pixel = color;
}


static guint32
geany_int_from_color(const GdkColor *gdk_color)
{
	guint8 r = (gdk_color->red / (gdouble)G_MAXUINT16) * 255;
	guint8 g = (gdk_color->green / (gdouble)G_MAXUINT16) * 255;
	guint8 b = (gdk_color->blue / (gdouble)G_MAXUINT16) * 255;
	guint32 color = b;
	color = (color << 8) + g;
	color = (color << 8) + r;
	return color;
}


void
geany_scintilla_get_edge_color(GeanyScintilla *sci, GdkColor *gdk_color)
{
	guint32 color;

	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	g_return_if_fail(gdk_color != NULL);

	color = SSM(sci, SCI_GETEDGECOLOUR, 0, 0);
	geany_color_from_int(color, gdk_color);
}


void
geany_scintilla_set_edge_color(GeanyScintilla *sci, const GdkColor *color)
{
	GdkColor old_color = {0};
	GdkColor new_color = {0};

	g_return_if_fail(GEANY_SCINTILLA(sci));

	geany_scintilla_get_edge_color(sci, &old_color);

	if (color != NULL)
		new_color = *color;

	if (!gdk_color_equal(&new_color, &old_color))
	{
		SSM(sci, SCI_SETEDGECOLOUR, geany_int_from_color(&new_color), 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_EDGE_COLOR]);
	}
}


/* NOTE: returned pointer is invalidated as soon as any changes are made
 * to the Scintilla buffer. */
const gchar *
geany_scintilla_get_text(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), NULL);
	return (const gchar*) SSM(sci, SCI_GETCHARACTERPOINTER, 0, 0);
}


/* Like _get_text() except puts a copy of the text into supplied string */
void
geany_scintilla_get_text_string(GeanyScintilla *sci, GString *out_string)
{
	const gchar *text;

	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	g_return_if_fail(out_string != NULL);

	text = geany_scintilla_get_text(sci);
	if (text != NULL)
	{
		g_string_set_size(out_string, SSM(sci, SCI_GETLENGTH, 0, 0));
		g_string_assign(out_string, text);
	}
}


/* NOTE: returned pointer is invalidated as soon as any changes are made
 * to the Scintilla buffer, also the returned string is not zero-terminated. */
const gchar *
geany_scintilla_get_text_range(GeanyScintilla *sci, guint start, guint end)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), NULL);
	return (const gchar*) SSM(sci, SCI_GETRANGEPOINTER, start, end - start);
}


void
geany_scintilla_get_text_range_string(GeanyScintilla *sci, GString *out_string,
	guint start, guint end)
{
	struct Sci_TextRange range;
	gsize len = end - start;

	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	g_return_if_fail(out_string != NULL);

	g_string_set_size(out_string, len + 1);
	range.chrg.cpMin = start;
	range.chrg.cpMax = end;
	range.lpstrText = out_string->str;
	g_string_set_size(out_string, SSM(sci, SCI_GETTEXTRANGE, 0, &range));
}


void
geany_scintilla_set_text(GeanyScintilla *sci, const gchar *text)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));

	if (text == NULL)
		text = "";

	/* TODO: check if this is too slow to be worth having correct property
	 * notifications. */
	if (g_strcmp0(text, geany_scintilla_get_text(sci)) != 0)
	{
		SSM(sci, SCI_SETTEXT, 0, text);
		g_object_notify_by_pspec(G_OBJECT(sci), geany_scintilla_pspecs[PROP_TEXT]);
	}
}


guint
geany_scintilla_get_text_length(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), 0);
	return SSM(sci, SCI_GETLENGTH, 0, 0);
}


/* If pos is < 0 then insert at current position, if len < 0, measure text
 * with strlen(). */
void geany_scintilla_insert_text_length(GeanyScintilla *sci, gssize pos, gssize len, const gchar *text)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	g_return_if_fail(text == NULL && len > 0);

	if (text == NULL)
		text = "";

	if (len < 0 && pos >= 0)
		SSM(sci, SCI_INSERTTEXT, pos, text);
	else
	{
		if (len < 0)
			len = strlen(text);

		if (pos < 0)
			SSM(sci, SCI_ADDTEXT, len, text);
		else
		{ /* Scintilla doesn't seem to have an "insert with length" message */
			guint cur_pos = SSM(sci, SCI_GETCURRENTPOS, 0, 0);
			SSM(sci, SCI_SETCURRENTPOS, pos, 0);
			SSM(sci, SCI_ADDTEXT, len, text);
			SSM(sci, SCI_SETCURRENTPOS, cur_pos, 0);
		}
	}
}


/* Emit property notifications when the can-undo/can-redo states change */
static void
geany_scintilla_update_undo_state(GeanyScintilla *sci)
{
	gboolean new_state = SSM(sci, SCI_CANUNDO, 0, 0);
	if (new_state != sci->priv->can_undo)
	{
		sci->priv->can_undo = new_state;
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_CAN_UNDO]);
	}
	new_state = SSM(sci, SCI_CANREDO, 0, 0);
	if (new_state != sci->priv->can_redo)
	{
		sci->priv->can_redo = new_state;
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_CAN_REDO]);
	}
}


gboolean
geany_scintilla_get_can_undo(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return sci->priv->can_undo;
}


gboolean
geany_scintilla_get_can_redo(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return sci->priv->can_redo;
}


void
geany_scintilla_undo(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_UNDO, 0, 0);
	geany_scintilla_update_undo_state(sci);
}


void
geany_scintilla_redo(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_REDO, 0, 0);
	geany_scintilla_update_undo_state(sci);
}


void
geany_scintilla_begin_undo_action(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_BEGINUNDOACTION, 0, 0);
}


void
geany_scintilla_end_undo_action(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_ENDUNDOACTION, 0, 0);
}


gboolean
geany_scintilla_get_enable_undo_collection(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return sci->priv->undo_collection;
}


void
geany_scintilla_set_enable_undo_collection(GeanyScintilla *sci, gboolean enable)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (enable != sci->priv->undo_collection)
	{
		sci->priv->undo_collection = enable;
		SSM(sci, SCI_SETUNDOCOLLECTION, enable, 0);
		if (!enable)
			SSM(sci, SCI_EMPTYUNDOBUFFER, 0, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_UNDO_COLLECTION]);
	}
}


static void
geany_scintilla_update_modified_state(GeanyScintilla *sci)
{
	gboolean modified = SSM(sci, SCI_GETMODIFY, 0, 0);
	if (modified != sci->priv->modified)
	{
		sci->priv->modified = modified;
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_MODIFIED]);
	}
}


gboolean
geany_scintilla_get_modified(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return sci->priv->modified;
}


gint geany_scintilla_get_zoom_level(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), 0);
	return SSM(sci, SCI_GETZOOM, 0, 0);
}


void geany_scintilla_set_zoom_level(GeanyScintilla *sci, gint zoom_level)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (zoom_level != geany_scintilla_get_zoom_level(sci))
		SSM(sci, SCI_SETZOOM, zoom_level, 0);
	/* property notification emitted from SCN_ZOOM handler */
}


void geany_scintilla_zoom_in(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_ZOOMIN, 0, 0);
	/* property notification emitted from SCN_ZOOM handler */
}


void geany_scintilla_zoom_out(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_ZOOMOUT, 0, 0);
	/* property notification emitted from SCN_ZOOM handler */
}


GeanyScintillaEOLMode
geany_scintilla_get_eol_mode(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), GEANY_SCINTILLA_EOL_MODE_LF);
	return SSM(sci, SCI_GETEOLMODE, 0, 0);
}


void
geany_scintilla_set_eol_mode(GeanyScintilla *sci, GeanyScintillaEOLMode eol_mode)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (eol_mode != geany_scintilla_get_eol_mode(sci))
	{
		SSM(sci, SCI_SETEOLMODE, eol_mode, 0);
		g_object_notify_by_pspec(G_OBJECT(sci), geany_scintilla_pspecs[PROP_EOL_MODE]);
	}
}


void geany_scintilla_convert_eols(GeanyScintilla *sci, GeanyScintillaEOLMode eol_mode)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_CONVERTEOLS, eol_mode, 0);
}


gboolean geany_scintilla_get_eol_visible(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return SSM(sci, SCI_GETVIEWEOL, 0, 0);
}


void geany_scintilla_set_eol_visible(GeanyScintilla *sci, gboolean eol_visible)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (eol_visible != geany_scintilla_get_eol_visible(sci))
	{
		SSM(sci, SCI_SETVIEWEOL, eol_visible, 0);
		g_object_notify_by_pspec(G_OBJECT(sci), geany_scintilla_pspecs[PROP_EOL_VISIBLE]);
	}
}


gboolean
geany_scintilla_get_whitespace_visible(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return (SSM(sci, SCI_GETVIEWWS, 0, 0) == SCWS_VISIBLEALWAYS);
}


void
geany_scintilla_set_whitespace_visible(GeanyScintilla *sci, gboolean visible)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (visible != geany_scintilla_get_whitespace_visible(sci))
	{
		SSM(sci, SCI_SETVIEWWS, visible ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_WHITESPACE_VISIBLE]);
	}
}


void
geany_scintilla_cut(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_CUT, 0, 0);
}


void
geany_scintilla_copy(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_COPY, 0, 0);
}


void
geany_scintilla_paste(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_PASTE, 0, 0);
}


void
geany_scintilla_clear(GeanyScintilla *sci)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_CLEAR, 0, 0);
}


gboolean
geany_scintilla_get_use_tabs(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return SSM(sci, SCI_GETUSETABS, 0, 0);
}


void
geany_scintilla_set_use_tabs(GeanyScintilla *sci, gboolean use_tabs)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (use_tabs != geany_scintilla_get_use_tabs(sci))
	{
		SSM(sci, SCI_SETUSETABS, use_tabs, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_USE_TABS]);
	}
}


gboolean
geany_scintilla_get_tab_indents(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return SSM(sci, SCI_GETTABINDENTS, 0, 0);
}


void
geany_scintilla_set_tab_indents(GeanyScintilla *sci, gboolean tab_indents)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (tab_indents != geany_scintilla_get_tab_indents(sci))
	{
		SSM(sci, SCI_SETTABINDENTS, tab_indents, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_TAB_INDENTS]);
	}
}


gboolean
geany_scintilla_get_backspace_unindents(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return SSM(sci, SCI_GETBACKSPACEUNINDENTS, 0, 0);
}


void
geany_scintilla_set_backspace_unindents(GeanyScintilla *sci, gboolean unindents)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (unindents != geany_scintilla_get_backspace_unindents(sci))
	{
		SSM(sci, SCI_SETBACKSPACEUNINDENTS, unindents, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_BACKSPACE_UNINDENTS]);
	}
}


gboolean
geany_scintilla_get_enable_overtype(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return SSM(sci, SCI_GETOVERTYPE, 0, 0);
}


void
geany_scintilla_set_enable_overtype(GeanyScintilla *sci, gboolean overtype)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (overtype != geany_scintilla_get_enable_overtype(sci))
	{
		SSM(sci, SCI_SETOVERTYPE, overtype, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_ENABLE_OVERTYPE]);
	}
}


gboolean
geany_scintilla_get_has_selection(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return (SSM(sci, SCI_GETSELECTIONEND, 0, 0) - SSM(sci, SCI_GETSELECTIONSTART, 0, 0) > 0);
}


gboolean
geany_scintilla_get_read_only(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return SSM(sci, SCI_GETREADONLY, 0, 0);
}


void
geany_scintilla_set_read_only(GeanyScintilla *sci, gboolean read_only)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (read_only != geany_scintilla_get_read_only(sci))
	{
		SSM(sci, SCI_SETREADONLY, read_only, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_READ_ONLY]);
	}
}


gboolean
geany_scintilla_get_line_wrapping_enabled(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), FALSE);
	return (SSM(sci, SCI_GETWRAPMODE, 0, 0) != SC_WRAP_NONE);
}


void
geany_scintilla_set_line_wrapping_enabled(GeanyScintilla *sci, gboolean enabled)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (enabled != geany_scintilla_get_line_wrapping_enabled(sci))
	{
		SSM(sci, SCI_SETWRAPMODE, enabled ? SC_WRAP_WORD : SC_WRAP_NONE, 0);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_LINE_WRAPPING_ENABLED]);
	}
}


static void
geany_scintilla_update_current_pos(GeanyScintilla *sci)
{
	guint new_pos = SSM(sci, SCI_GETCURRENTPOS, 0, 0);
	if (new_pos != sci->priv->current_pos)
	{
		sci->priv->current_pos = new_pos;
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_CURRENT_POSITION]);
	}
	new_pos = SSM(sci, SCI_LINEFROMPOSITION, new_pos, 0);
	if (new_pos != sci->priv->current_line)
	{
		sci->priv->current_line = new_pos;
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_CURRENT_LINE]);
	}
}


void
geany_scintilla_goto_position(GeanyScintilla *sci, guint pos, gboolean unfold)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	if (unfold)
		SSM(sci, SCI_ENSUREVISIBLE, SSM(sci, SCI_LINEFROMPOSITION, pos, 0), 0);
	SSM(sci, SCI_GOTOPOS, pos, 0);
	geany_scintilla_update_current_pos(sci);
}


void
geany_scintilla_goto_line(GeanyScintilla *sci, guint line)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_ENSUREVISIBLE, line, 0);
	SSM(sci, SCI_GOTOLINE, line, 0);
	geany_scintilla_update_current_pos(sci);
}


guint
geany_scintilla_get_current_position(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), 0);
	return sci->priv->current_pos;
}


void
geany_scintilla_set_current_position(GeanyScintilla *sci, guint pos)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	SSM(sci, SCI_SETCURRENTPOS, pos, 0);
	geany_scintilla_update_current_pos(sci);
}


guint
geany_scintilla_get_current_line(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), 0);
	return sci->priv->current_line;
}


void
geany_scintilla_set_current_line(GeanyScintilla *sci, guint line)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));
	geany_scintilla_set_current_position(sci, SSM(sci, SCI_POSITIONFROMLINE, line, 0));
}
