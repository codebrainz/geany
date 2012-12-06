/*
 *      editor.h - this file is part of Geany, a fast and lightweight IDE
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


#ifndef GEANY_EDITOR_H
#define GEANY_EDITOR_H 1

G_BEGIN_DECLS

#include "Scintilla.h"
#include "ScintillaWidget.h"

#include "editorprefs.h"

/** Default character set to define which characters should be treated as part of a word. */
#define GEANY_WORDCHARS					"_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define GEANY_MAX_WORD_LENGTH			192


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

/** Geany indicator types, can be used with Editor indicator functions to highlight
 *  text in the document. */
typedef enum
{
	/** Indicator to highlight errors in the document text. This is a red squiggly underline. */
	GEANY_INDICATOR_ERROR = 0,
	/** Indicator used to highlight search results in the document. This is a
	 *  rounded box around the text. */
	/* start container indicator outside of lexer indicators (0..7), see Scintilla docs */
	GEANY_INDICATOR_SEARCH = 8
}
GeanyIndicator;


/** Editor-owned fields for each document. */
struct GeanyEditor
{
	GeanyDocument	*document;		/**< The document associated with the editor. */
	ScintillaObject	*sci;			/**< The Scintilla editor @c GtkWidget. */
	gboolean		 line_wrapping;	/**< @c TRUE if line wrapping is enabled. */
	gboolean		 auto_indent;	/**< @c TRUE if auto-indentation is enabled. */
	/** Percentage to scroll view by on paint, if positive. */
	gfloat			 scroll_percent;
	GeanyIndentType	 indent_type;	/* Use editor_get_indent_prefs() instead. */
	gboolean		 line_breaking;	/**< Whether to split long lines as you type. */
	gint			 indent_width;
	/* holds word under the mouse or keyboard cursor */
	gchar			 current_word[GEANY_MAX_WORD_LENGTH];
	gint			 click_pos;		/* text position where the mouse was clicked */
	/* flag to indicate that an insert callback was triggered from the editing
	 * widget's context menu, so we need to store the current cursor position
	 * in editor->click_pos amongst other things */
	gboolean		 insert_callback_from_menu;
};


typedef struct SCNotification SCNotification;


void editor_init(void);

GeanyEditor *editor_create(GeanyDocument *doc);

void editor_destroy(GeanyEditor *editor);

ScintillaObject *editor_create_widget(GeanyEditor *editor);

void editor_sci_notify_cb(GtkWidget *widget, gint scn, gpointer scnt, gpointer data);

gboolean editor_start_auto_complete(GeanyEditor *editor, gint pos, gboolean force);

gboolean editor_complete_word_part(GeanyEditor *editor);

void editor_goto_next_snippet_cursor(GeanyEditor *editor);

gboolean editor_complete_snippet(GeanyEditor *editor, gint pos);

void editor_show_macro_list(GeanyEditor *editor);

gboolean editor_show_calltip(GeanyEditor *editor, gint pos);

void editor_do_comment_toggle(GeanyEditor *editor);

void editor_do_comment(GeanyEditor *editor, gint line, gboolean allow_empty_lines, gboolean toggle,
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

gchar *editor_get_word_at_pos(GeanyEditor *editor, gint pos, const gchar *wordchars);

gchar *editor_get_default_selection(GeanyEditor *editor, gboolean use_current_word, const gchar *wordchars);


void editor_select_word(GeanyEditor *editor);

void editor_select_lines(GeanyEditor *editor, gboolean extra_line);

void editor_select_paragraph(GeanyEditor *editor);

void editor_select_indent_block(GeanyEditor *editor);


void editor_set_font(GeanyEditor *editor, const gchar *font);

void editor_indicator_set_on_line(GeanyEditor *editor, gint indic, gint line);

void editor_indicator_clear_errors(GeanyEditor *editor);

void editor_indicator_set_on_range(GeanyEditor *editor, gint indic, gint start, gint end);

void editor_indicator_clear(GeanyEditor *editor, gint indic);

gint editor_get_eol_char_mode(GeanyEditor *editor);

const gchar *editor_get_eol_char_name(GeanyEditor *editor);

gint editor_get_eol_char_len(GeanyEditor *editor);

const gchar *editor_get_eol_char(GeanyEditor *editor);

void editor_convert_line_endings(GeanyEditor *editor, gint sci_eol_mode);

void editor_fold_all(GeanyEditor *editor);

void editor_unfold_all(GeanyEditor *editor);

void editor_replace_tabs(GeanyEditor *editor);

void editor_replace_spaces(GeanyEditor *editor);

void editor_strip_line_trailing_spaces(GeanyEditor *editor, gint line);

void editor_strip_trailing_spaces(GeanyEditor *editor);

void editor_ensure_final_newline(GeanyEditor *editor);

void editor_insert_color(GeanyEditor *editor, const gchar *colour);

const GeanyIndentPrefs *editor_get_indent_prefs(GeanyEditor *editor);

void editor_set_indent_type(GeanyEditor *editor, GeanyIndentType type);

void editor_set_indent_width(GeanyEditor *editor, gint width);

void editor_set_indent(GeanyEditor *editor, GeanyIndentType type, gint width);

void editor_set_line_wrapping(GeanyEditor *editor, gboolean wrap);

gboolean editor_goto_pos(GeanyEditor *editor, gint pos, gboolean mark);

gboolean editor_goto_line(GeanyEditor *editor, gint line_no, gint offset);

void editor_set_indentation_guides(GeanyEditor *editor);

void editor_apply_update_prefs(GeanyEditor *editor);

gchar *editor_get_calltip_text(GeanyEditor *editor, const TMTag *tag);

void editor_insert_text_block(GeanyEditor *editor, const gchar *text,
	 						  gint insert_pos, gint cursor_index,
	 						  gint newline_indent_size, gboolean replace_newlines);

void editor_toggle_fold(GeanyEditor *editor, gint line, gint modifiers);

const gchar *editor_find_snippet(GeanyEditor *editor, const gchar *snippet_name);

void editor_insert_snippet(GeanyEditor *editor, gint pos, const gchar *snippet);

G_END_DECLS

#endif
