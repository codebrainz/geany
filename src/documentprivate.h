/*
 *      document-private.h - this file is part of Geany, a fast and lightweight IDE
 *
 *      Copyright 2008-2012 Enrico Tröger <enrico(dot)troeger(at)uvena(dot)de>
 *      Copyright 2008-2012 Nick Treleaven <nick(dot)treleaven(at)btinternet(dot)com>
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

#ifndef GEANY_DOCUMENT_PRIVATE_H
#define GEANY_DOCUMENT_PRIVATE_H 1

#include "document.h"
#include "search.h"

#include "gtkcompat.h" /* Needed by ScintillaWidget.h */
#include "Scintilla.h" /* Needed by ScintillaWidget.h */
#include "ScintillaWidget.h" /* For ScintillaObject */

#include "gtkcompat.h"

G_BEGIN_DECLS

/* available UNDO actions, UNDO_SCINTILLA is a pseudo action to trigger Scintilla's
 * undo management */
enum
{
	UNDO_SCINTILLA = 0,
	UNDO_ENCODING,
	UNDO_BOM,
	UNDO_ACTIONS_MAX
};

typedef enum
{
	FILE_OK,
	FILE_CHANGED, /* also valid for deleted files */
	FILE_IGNORE
}
FileDiskStatus;


typedef struct FileEncoding
{
	gchar 			*encoding;
	gboolean		 has_bom;
}
FileEncoding;


/* Private GeanyDocument fields */
typedef struct GeanyDocumentPrivate
{
	/* GtkLabel shown in the notebook header. */
	GtkWidget		*tab_label;
	/* GtkTreeView object for this document within the Symbols treeview of the sidebar. */
	GtkWidget		*tag_tree;
	/* GtkTreeStore object for this document within the Symbols treeview of the sidebar. */
	GtkTreeStore	*tag_store;
	/* Iter for this document within the Open Files treeview of the sidebar. */
	GtkTreeIter		 iter;
	/* Used by the Undo/Redo management code. */
	GTrashStack		*undo_actions;
	/* Used by the Undo/Redo management code. */
	GTrashStack		*redo_actions;
	/* Used so Undo/Redo works for encoding changes. */
	FileEncoding	 saved_encoding;
	gboolean		 colourise_needed;	/* use document.c:queue_colourise() instead */
	gint			 line_count;		/* Number of lines in the document. */
	gint			 symbol_list_sort_mode;
	/* indicates whether a file is on a remote filesystem, works only with GIO/GVfs */
	gboolean		 is_remote;
	/* File status on disk of the document */
	FileDiskStatus	 file_disk_status;
	/* Reference to a GFileMonitor object, only used when GIO file monitoring is used. */
	gpointer		 monitor;
	/* Time of the last disk check, only used when legacy file monitoring is used. */
	time_t			 last_check;
	/* Modification time of the document on disk, only used when legacy file monitoring is used. */
	time_t			 mtime;
	/* ID of the idle callback updating the tag list */
	guint			 tag_list_update_source;
}
GeanyDocumentPrivate;


extern GeanyFilePrefs file_prefs;


GeanyDocument* document_new_file_if_non_open(void);

void document_reload_config(GeanyDocument *doc);

GeanyDocument *document_find_by_sci(ScintillaObject *sci);

void document_show_tab(GeanyDocument *doc);

void document_init_doclist(void);

void document_finalize(void);

void document_try_focus(GeanyDocument *doc, GtkWidget *source_widget);

gboolean document_account_for_unsaved(void);

gboolean document_close_all(void);

GeanyDocument *document_open_file_full(GeanyDocument *doc, const gchar *filename, gint pos,
		gboolean readonly, GeanyFiletype *ft, const gchar *forced_enc);

void document_open_file_list(const gchar *data, gsize length);

gboolean document_search_bar_find(GeanyDocument *doc, const gchar *text, gint flags, gboolean inc,
		gboolean backwards);

gint document_find_text(GeanyDocument *doc, const gchar *text, const gchar *original_text,
		gint flags, gboolean search_backwards, GeanyMatchInfo **match_,
		gboolean scroll, GtkWidget *parent);

gint document_replace_text(GeanyDocument *doc, const gchar *find_text, const gchar *original_find_text,
		const gchar *replace_text, gint flags, gboolean search_backwards);

gint document_replace_all(GeanyDocument *doc, const gchar *find_text, const gchar *replace_text,
		const gchar *original_find_text, const gchar *original_replace_text, gint flags);

void document_replace_sel(GeanyDocument *doc, const gchar *find_text, const gchar *replace_text,
						  const gchar *original_find_text, const gchar *original_replace_text, gint flags);

void document_update_tags(GeanyDocument *doc);

void document_update_tag_list_in_idle(GeanyDocument *doc);

void document_highlight_tags(GeanyDocument *doc);

gboolean document_check_disk_status(GeanyDocument *doc, gboolean force);

/* own Undo / Redo implementation to be able to undo / redo changes
 * to the encoding or the Unicode BOM (which are Scintilla independent).
 * All Scintilla events are stored in the undo / redo buffer and are passed through. */

gboolean document_can_undo(GeanyDocument *doc);

gboolean document_can_redo(GeanyDocument *doc);

void document_undo(GeanyDocument *doc);

void document_redo(GeanyDocument *doc);

void document_undo_add(GeanyDocument *doc, guint type, gpointer data);

void document_update_tab_label(GeanyDocument *doc);

const gchar *document_get_status_widget_class(GeanyDocument *doc);

gboolean document_need_save_as(GeanyDocument *doc);

gboolean document_detect_indent_type(GeanyDocument *doc, GeanyIndentType *type_);

gboolean document_detect_indent_width(GeanyDocument *doc, gint *width_);

void document_apply_indent_settings(GeanyDocument *doc);

void document_grab_focus(GeanyDocument *doc);

GeanyDocument *document_clone(GeanyDocument *old_doc);

G_END_DECLS

#endif /* GEANY_DOCUMENT_PRIVATE_H */
