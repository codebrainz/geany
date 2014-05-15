#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "wincmd.h"

#include <glib-object.h>
#include <gmodule.h>
#include <gtk/gtk.h>
#include <libpeas/peas.h>
#include <libpeas-gtk/peas-gtk.h>

typedef struct GeanyDocument GeanyDocument; // for ui_utils.h include
#include "ui_utils.h" // for ui_set_statusbar()

#define DEMO_TYPE_EXT         (demo_ext_get_type())
#define DEMO_EXT(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), DEMO_TYPE_EXT, DemoExt))
#define DEMO_EXT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DEMO_TYPE_EXT, DemoExt))
#define DEMO_IS_EXT(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), DEMO_TYPE_EXT))
#define DEMO_IS_EXT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), DEMO_TYPE_EXT))
#define DEMO_EXT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), DEMO_TYPE_EXT, DemoExtClass))

typedef struct
{
	PeasExtensionBase parent;
	GtkWindow *window;
} DemoExt;

typedef struct
{
	PeasExtensionBaseClass parent;
} DemoExtClass;

GType demo_ext_get_type(void);

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);
static void demo_ext_iface_init(GeanyWinCmdInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(DemoExt, demo_ext, PEAS_TYPE_EXTENSION_BASE, 0,
	G_IMPLEMENT_INTERFACE_DYNAMIC(GEANY_TYPE_WIN_CMD, demo_ext_iface_init))

enum
{
	PROP_0,
	PROP_WINDOW
};


static void demo_ext_set_property(GObject *object, guint prop_id,
	const GValue *value, GParamSpec *pspec)
{
	DemoExt *demo = DEMO_EXT(object);
	switch (prop_id)
	{
		case PROP_WINDOW:
			demo->window = GTK_WINDOW(g_value_dup_object(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}


static void demo_ext_get_property(GObject *object, guint prop_id,
	GValue *value, GParamSpec *pspec)
{
	DemoExt *demo = DEMO_EXT(object);
	switch (prop_id)
	{
		case PROP_WINDOW:
			g_value_set_object(value, demo->window);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}


static void demo_ext_init(DemoExt *demo)
{
	g_debug("%s", G_STRFUNC);
	ui_set_statusbar(FALSE, "%s", G_STRFUNC);
}


static void demo_ext_finalize(GObject *object)
{
	DemoExt *demo = DEMO_EXT(object);
	g_debug("%s", G_STRFUNC);
	ui_set_statusbar(FALSE, "%s", G_STRFUNC);
	g_object_unref(demo->window);
	G_OBJECT_CLASS(demo_ext_parent_class)->finalize(object);
}


static void demo_ext_activate(GeanyWinCmd *cmd)
{
	g_debug("%s", G_STRFUNC);
	ui_set_statusbar(FALSE, "%s", G_STRFUNC);
}


static void demo_ext_deactivate(GeanyWinCmd *cmd)
{
	g_debug("%s", G_STRFUNC);
	ui_set_statusbar(FALSE, "%s", G_STRFUNC);
}


static void demo_ext_update_state(GeanyWinCmd *cmd)
{
	g_debug("%s", G_STRFUNC);
	ui_set_statusbar(FALSE, "%s", G_STRFUNC);
}


static void demo_ext_class_init(DemoExtClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	
	object_class->set_property = demo_ext_set_property;
	object_class->get_property = demo_ext_get_property;
	object_class->finalize = demo_ext_finalize;
	
	g_object_class_override_property(object_class, PROP_WINDOW, "window");
}


static void demo_ext_iface_init(GeanyWinCmdInterface *iface)
{
	iface->activate = demo_ext_activate;
	iface->deactivate = demo_ext_deactivate;
	iface->update_state = demo_ext_update_state;
}


static void demo_ext_class_finalize(DemoExtClass *klass)
{
}


G_MODULE_EXPORT void
peas_register_types(PeasObjectModule *module)
{
	demo_ext_register_type(G_TYPE_MODULE(module));
	peas_object_module_register_extension_type(module, GEANY_TYPE_WIN_CMD,
		DEMO_TYPE_EXT);
}
