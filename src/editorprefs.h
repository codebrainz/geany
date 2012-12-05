/*
 *      editorprefs.h - this file is part of Geany, a fast and lightweight IDE
 *
 *      Copyright 2005-2012 Enrico Tröger <enrico(dot)troeger(at)uvena(dot)de>
 *      Copyright 2006-2012 Nick Treleaven <nick(dot)treleaven(at)btinternet(dot)com>
 *      Copyright 2012      Matthew Brush <mbrush(at)codebrainz(dot)ca>
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


#ifndef GEANY_EDITOR_PREFS_H
#define GEANY_EDITOR_PREFS_H 1

#include <glib.h>


G_BEGIN_DECLS


/** Whether to use tabs, spaces or both to indent. */
typedef enum
{
	GEANY_INDENT_TYPE_SPACES,	/**< Spaces. */
	GEANY_INDENT_TYPE_TABS,		/**< Tabs. */
	GEANY_INDENT_TYPE_BOTH		/**< Both. */
}
GeanyIndentType;


typedef enum
{
	GEANY_AUTOINDENT_NONE = 0,
	GEANY_AUTOINDENT_BASIC,
	GEANY_AUTOINDENT_CURRENTCHARS,
	GEANY_AUTOINDENT_MATCHBRACES
}
GeanyAutoIndent;


/** Indentation prefs that might be different according to project or filetype.
 * Use @c editor_get_indent_prefs() to lookup the prefs for a particular document.
 *
 * @since 0.15
 **/
typedef struct GeanyIndentPrefs
{
	gint			width;				/**< Indent width. */
	GeanyIndentType	type;				/**< Whether to use tabs, spaces or both to indent. */
	/** Width of a tab, but only when using GEANY_INDENT_TYPE_BOTH.
	 * To get the display tab width, use sci_get_tab_width(). */
	gint			hard_tab_width;
	GeanyAutoIndent	auto_indent_mode;
	gboolean		detect_type;
	gboolean		detect_width;
}
GeanyIndentPrefs;


/** Default prefs when creating a new editor window.
 * Some of these can be overridden per document or per project. */
/* See editor_get_prefs(). */
typedef struct GeanyEditorPrefs
{
	GeanyIndentPrefs *indentation;	/* Default indentation prefs. Use editor_get_indent_prefs(). */
	gboolean	show_white_space;
	gboolean	show_indent_guide;
	gboolean	show_line_endings;
	/* 0 - line, 1 - background, 2 - disabled.
	 * This setting may be overridden when a project is opened. Use @c editor_get_prefs(). */
	gint		long_line_type;
	/* This setting may be overridden when a project is opened. Use @c editor_get_prefs(). */
	gint		long_line_column;
	gchar		*long_line_color;
	gboolean	show_markers_margin;		/* view menu */
	gboolean	show_linenumber_margin;		/* view menu */
	gboolean	show_scrollbars;			/* hidden pref */
	gboolean	scroll_stop_at_last_line;
	gboolean	line_wrapping;
	gboolean	use_indicators;
	gboolean	folding;
	gboolean	unfold_all_children;
	gboolean	disable_dnd;
	gboolean	use_tab_to_indent;	/* makes tab key indent instead of insert a tab char */
	gboolean	smart_home_key;
	gboolean	newline_strip;
	gboolean	auto_complete_symbols;
	gboolean	auto_close_xml_tags;
	gboolean	complete_snippets;
	gint		symbolcompletion_min_chars;
	gint		symbolcompletion_max_height;
	gboolean	brace_match_ltgt;	/* whether to highlight < and > chars (hidden pref) */
	gboolean	use_gtk_word_boundaries;	/* hidden pref */
	gboolean	complete_snippets_whilst_editing;	/* hidden pref */
	gint		line_break_column;
	gboolean	auto_continue_multiline;
	gchar		*comment_toggle_mark;
	guint		autocompletion_max_entries;
	guint		autoclose_chars;
	gboolean	autocomplete_doc_words;
	gboolean	completion_drops_rest_of_word;
	gchar		*color_scheme;
	gint 		show_virtual_space;
	/* This setting may be overridden when a project is opened. Use @c editor_get_prefs(). */
	gboolean	long_line_enabled;
	gint		autocompletion_update_freq;
}
GeanyEditorPrefs;


/* Defined in editor.c, initialized in keyfile.c */
extern GeanyEditorPrefs editor_prefs;


G_END_DECLS


#endif /* GEANY_EDITOR_PREFS_H */
