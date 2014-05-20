/*
 *      build.h - this file is part of Geany, a fast and lightweight IDE
 *
 *      Copyright 2005-2012 Enrico Tröger <enrico(dot)troeger(at)uvena(dot)de>
 *      Copyright 2006-2012 Nick Treleaven <nick(dot)treleaven(at)btinternet(dot)com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License along
 *      with this program; if not, write to the Free Software Foundation, Inc.,
 *      51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/** @file build.h Interface to the Build menu functionality. */

#ifndef GEANY_BUILD_H
#define GEANY_BUILD_H 1

#include "document.h"
#include "filetypes.h"

#include <glib.h>

G_BEGIN_DECLS

/** Groups of Build menu items. */
typedef enum
{
	GEANY_GBG_FT,		/* *< filetype items */
	GEANY_GBG_NON_FT,	/* *< non filetype items.*/
	GEANY_GBG_EXEC,		/* *< execute items */
	GEANY_GBG_COUNT		/* *< count of groups. */
} GeanyBuildGroup;

/** Build menu item sources in increasing priority */
typedef enum
{
	GEANY_BCS_DEF,		/* *< Default values. */
	GEANY_BCS_FT,		/* *< System filetype values. */
	GEANY_BCS_HOME_FT,	/* *< Filetypes in ~/.config/geany/filedefs */
	GEANY_BCS_PREF,		/* *< Preferences file ~/.config/geany/geany.conf */
	GEANY_BCS_PROJ_FT,	/* *< Project file filetype command */
	GEANY_BCS_PROJ,		/* *< Project file if open. */
	GEANY_BCS_COUNT		/* *< Count of sources. */
} GeanyBuildSource;

/** The entries of a command for a menu item */
typedef enum GeanyBuildCmdEntries
{
	GEANY_BC_LABEL,				/* *< The menu item label, _ marks mnemonic */
	GEANY_BC_COMMAND,			/* *< The command to run. */
	GEANY_BC_WORKING_DIR,		/* *< The directory to run in */
	GEANY_BC_CMDENTRIES_COUNT	/* *< Count of entries */
} GeanyBuildCmdEntries;

G_MODULE_EXPORT
void build_activate_menu_item(const GeanyBuildGroup grp, const guint cmd);

G_MODULE_EXPORT
const gchar *build_get_current_menu_item(const GeanyBuildGroup grp, const guint cmd, 
                                         const GeanyBuildCmdEntries field);

G_MODULE_EXPORT
void build_remove_menu_item(const GeanyBuildSource src, const GeanyBuildGroup grp, const gint cmd);

G_MODULE_EXPORT
void build_set_menu_item(const GeanyBuildSource src, const GeanyBuildGroup grp,
                         const guint cmd, const GeanyBuildCmdEntries field, const gchar *value);

G_MODULE_EXPORT
guint build_get_group_count(const GeanyBuildGroup grp);

G_END_DECLS

#endif /* GEANY_BUILD_H */
