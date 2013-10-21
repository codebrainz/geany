#include "geanyscintilla.h"
#include "geanyenums.h"

#define SSM(s, m, w, l) \
	scintilla_send_message(SCINTILLA(s), (guint)(m), (uptr_t)(w), (sptr_t)(l))


struct GeanyScintillaPrivate_
{
	gpointer reserved;
};

enum
{
	PROP_0,
	PROP_LINE_NUMBERS_VISIBLE,
	PROP_EDGE_MODE,
	PROP_EDGE_COLUMN,
	PROP_EDGE_COLOR,
	PROP_TEXT,
	N_PROPERTIES
};


typedef enum
{
	GEANY_SCINTILLA_MARGIN_LINE_NUMBERS,
}
GeanyScintillaMargin;


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
			}
			break;
		case SCN_ZOOM:
			geany_scintilla_update_line_numbers(sci);
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

guint geany_scintilla_get_text_length(GeanyScintilla *sci)
{
	g_return_val_if_fail(GEANY_IS_SCINTILLA(sci), 0);
	return SSM(sci, SCI_GETLENGTH, 0, 0);
}
