/*
 *      editorprivate.h - this file is part of Geany, a fast and lightweight IDE
 *
 *      Copyright 2008-2014 Enrico Tröger <enrico(dot)troeger(at)uvena(dot)de>
 *      Copyright 2008-2014 Nick Treleaven <nick(dot)treleaven(at)btinternet(dot)com>
 *      Copyright 2014 Matthew Brush <matt@geany.org>
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

#ifndef GEANY_EDITOR_PRIVATE_H
#define GEANY_EDITOR_PRIVATE_H 1

#include "editor.h"
#include "tm_tag.h" /* for TMTag */


G_BEGIN_DECLS

#define GEANY_MAX_WORD_LENGTH 192

typedef enum
{
	GEANY_AUTOINDENT_NONE = 0,
	GEANY_AUTOINDENT_BASIC,
	GEANY_AUTOINDENT_CURRENTCHARS,
	GEANY_AUTOINDENT_MATCHBRACES
}
GeanyAutoIndent;

typedef enum
{
	GEANY_VIRTUAL_SPACE_DISABLED = 0,
	GEANY_VIRTUAL_SPACE_SELECTION = 1,
	GEANY_VIRTUAL_SPACE_ALWAYS = 3
}
GeanyVirtualSpace;


/* Auto-close brackets/quotes */
enum {
	GEANY_AC_PARENTHESIS	= 1,
	GEANY_AC_CBRACKET		= 2,
	GEANY_AC_SBRACKET		= 4,
	GEANY_AC_SQUOTE			= 8,
	GEANY_AC_DQUOTE			= 16
};


typedef struct
{
	gchar	*current_word;	/* holds word under the mouse or keyboard cursor */
	gint	click_pos;		/* text position where the mouse was clicked */
} EditorInfo;


extern GeanyEditorPrefs editor_prefs;
extern EditorInfo editor_info;


typedef struct SCNotification SCNotification;


void editor_init(void);

GeanyEditor *editor_create(struct GeanyDocument *doc);

void editor_destroy(GeanyEditor *editor);

void editor_sci_notify_cb(GtkWidget *widget, gint scn, gpointer scnt, gpointer data);

gboolean editor_start_auto_complete(GeanyEditor *editor, gint pos, gboolean force);

gboolean editor_complete_word_part(GeanyEditor *editor);

void editor_goto_next_snippet_cursor(GeanyEditor *editor);

gboolean editor_complete_snippet(GeanyEditor *editor, gint pos);

void editor_show_macro_list(GeanyEditor *editor);

gboolean editor_show_calltip(GeanyEditor *editor, gint pos);

void editor_do_comment_toggle(GeanyEditor *editor);

gint editor_do_comment(GeanyEditor *editor, gint line, gboolean allow_empty_lines, gboolean toggle,
		gboolean single_comment);

gint editor_do_uncomment(GeanyEditor *editor, gint line, gboolean toggle);

void editor_insert_multiline_comment(GeanyEditor *editor);

void editor_insert_alternative_whitespace(GeanyEditor *editor);

void editor_indent(GeanyEditor *editor, gboolean increase);

void editor_smart_line_indentation(GeanyEditor *editor, gint pos);

void editor_indentation_by_one_space(GeanyEditor *editor, gint pos, gboolean decrease);

gboolean editor_line_in_view(GeanyEditor *editor, gint line);

void editor_scroll_to_line(GeanyEditor *editor, gint line, gfloat percent_of_view);

void editor_display_current_line(GeanyEditor *editor, gfloat percent_of_view);

void editor_finalize(void);

void editor_snippets_init(void);

void editor_snippets_free(void);

const GeanyEditorPrefs *editor_get_prefs(GeanyEditor *editor);

/* General editing functions */

void editor_find_current_word(GeanyEditor *editor, gint pos, gchar *word, gsize wordlen,
	const gchar *wc);

void editor_find_current_word_sciwc(GeanyEditor *editor, gint pos, gchar *word, gsize wordlen);

gchar *editor_get_default_selection(GeanyEditor *editor, gboolean use_current_word, const gchar *wordchars);

void editor_select_word(GeanyEditor *editor);

void editor_select_lines(GeanyEditor *editor, gboolean extra_line);

void editor_select_paragraph(GeanyEditor *editor);

void editor_select_indent_block(GeanyEditor *editor);

void editor_set_font(GeanyEditor *editor, const gchar *font);

void editor_indicator_clear(GeanyEditor *editor, gint indic);

void editor_fold_all(GeanyEditor *editor);

void editor_unfold_all(GeanyEditor *editor);

void editor_replace_tabs(GeanyEditor *editor);

void editor_replace_spaces(GeanyEditor *editor);

void editor_strip_line_trailing_spaces(GeanyEditor *editor, gint line);

void editor_strip_trailing_spaces(GeanyEditor *editor);

void editor_ensure_final_newline(GeanyEditor *editor);

void editor_insert_color(GeanyEditor *editor, const gchar *colour);

void editor_set_indent_width(GeanyEditor *editor, gint width);

void editor_set_indent(GeanyEditor *editor, GeanyIndentType type, gint width);

void editor_set_line_wrapping(GeanyEditor *editor, gboolean wrap);

gboolean editor_goto_line(GeanyEditor *editor, gint line_no, gint offset);

void editor_set_indentation_guides(GeanyEditor *editor);

void editor_apply_update_prefs(GeanyEditor *editor);

gchar *editor_get_calltip_text(GeanyEditor *editor, const TMTag *tag);

void editor_toggle_fold(GeanyEditor *editor, gint line, gint modifiers);

G_END_DECLS

#endif /* GEANY_EDITOR_PRIVATE_H */
