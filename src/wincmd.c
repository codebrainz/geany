#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_PEAS

#include <gtk/gtk.h>
#include "wincmd.h"


G_DEFINE_INTERFACE(GeanyWinCmd, geany_win_cmd, G_TYPE_OBJECT)


void geany_win_cmd_default_init(GeanyWinCmdInterface *iface)
{
	static gboolean once = FALSE;
	if (!once)
	{
		g_object_interface_install_property(iface,
			g_param_spec_object("window", "Window", "The Geany window", GTK_TYPE_WINDOW,
				G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
		once = TRUE;
	}
}


void geany_win_cmd_activate(GeanyWinCmd *cmd)
{
	g_return_if_fail(GEANY_IS_WIN_CMD(cmd));
	GeanyWinCmdInterface *iface = GEANY_WIN_CMD_GET_IFACE(cmd);
	if (iface->activate)
		iface->activate(cmd);
}


void geany_win_cmd_deactivate(GeanyWinCmd *cmd)
{
	g_return_if_fail(GEANY_IS_WIN_CMD(cmd));
	GeanyWinCmdInterface *iface = GEANY_WIN_CMD_GET_IFACE(cmd);
	if (iface->deactivate)
		iface->deactivate(cmd);
}


void geany_win_cmd_update_state(GeanyWinCmd *cmd)
{
	g_return_if_fail(GEANY_IS_WIN_CMD(cmd));
	GeanyWinCmdInterface *iface = GEANY_WIN_CMD_GET_IFACE(cmd);
	if (iface->update_state)
		iface->update_state(cmd);
}


#endif // HAVE_PEAS
