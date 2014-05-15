#ifndef GEANY_WIN_CMD_H
#define GEANY_WIN_CMD_H

#include <glib-object.h>

G_BEGIN_DECLS


#define GEANY_TYPE_WIN_CMD           (geany_win_cmd_get_type())
#define GEANY_WIN_CMD(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), GEANY_TYPE_WIN_CMD, GeanyWinCmd))
#define GEANY_WIN_CMD_IFACE(obj)     (G_TYPE_CHECK_CLASS_CAST((obj), GEANY_TYPE_WIN_CMD, GeanyWinCmdInterface))
#define GEANY_IS_WIN_CMD(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), GEANY_TYPE_WIN_CMD))
#define GEANY_WIN_CMD_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE((obj), GEANY_TYPE_WIN_CMD, GeanyWinCmdInterface))


typedef struct GeanyWinCmd_ GeanyWinCmd;
typedef struct GeanyWinCmdInterface_ GeanyWinCmdInterface;


struct GeanyWinCmdInterface_
{
	GTypeInterface g_iface;

	void (*activate) (GeanyWinCmd *cmd);
	void (*deactivate) (GeanyWinCmd *cmd);
	void (*update_state) (GeanyWinCmd *cmd);
};


GType geany_win_cmd_get_type(void);

void geany_win_cmd_activate(GeanyWinCmd *cmd);

void geany_win_cmd_deactivate(GeanyWinCmd *cmd);

void geany_win_cmd_update_state(GeanyWinCmd *cmd);


G_END_DECLS

#endif // GEANY_WIN_CMD_H
