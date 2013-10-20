#include "geanyscintilla.h"

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
			if ((notif->modificationType & SC_MOD_INSERTTEXT ||
			    notif->modificationType & SC_MOD_DELETETEXT) &&
			    notif->linesAdded != 0)
			{
				geany_scintilla_update_line_numbers(sci);
			}
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
	return (SSM(sci, SCI_GETMARGINWIDTHN, 0, 0) != 0);
}


void
geany_scintilla_set_line_numbers_visible(GeanyScintilla *sci, gboolean visible)
{
	g_return_if_fail(GEANY_IS_SCINTILLA(sci));

	if (visible != geany_scintilla_get_line_numbers_visible(sci))
	{
		if (!visible)
			SSM(sci, SCI_SETMARGINWIDTHN, GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, 0);
		else
			geany_scintilla_update_line_numbers(sci);
		g_object_notify_by_pspec(G_OBJECT(sci),
			geany_scintilla_pspecs[PROP_LINE_NUMBERS_VISIBLE]);
	}
}
