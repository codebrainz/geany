#include "geanyscintilla.h"

struct GeanyScintillaPrivate_
{
	gpointer reserved;
};

static void geany_scintilla_finalize(GObject *object);

G_DEFINE_TYPE(GeanyScintilla, geany_scintilla, scintilla_get_type())


static void
geany_scintilla_class_init(GeanyScintillaClass *klass)
{
	GObjectClass *g_object_class;

	g_object_class = G_OBJECT_CLASS(klass);

	g_object_class->finalize = geany_scintilla_finalize;

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
geany_scintilla_init(GeanyScintilla *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, GEANY_TYPE_SCINTILLA, GeanyScintillaPrivate);
}


GtkWidget *
geany_scintilla_new(void)
{
	return g_object_new(GEANY_TYPE_SCINTILLA, NULL);
}
