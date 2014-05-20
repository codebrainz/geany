/*
 *      document.h - this file is part of Geany, a fast and lightweight IDE
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

/**
 *  @file document.h
 *  Document related actions: new, save, open, etc.
 **/
/* Also Scintilla search actions - but these should probably be moved to search.c. */


#ifndef GEANY_DOCUMENT_H
#define GEANY_DOCUMENT_H 1

#include "editor.h"
#include "filetypes.h"
#include "geany.h" /* For GEANY_STRING_UNTITLED */

#include <glib.h>


G_BEGIN_DECLS

#if defined(G_OS_WIN32)
# define GEANY_DEFAULT_EOL_CHARACTER SC_EOL_CRLF
#elif defined(G_OS_UNIX)
# define GEANY_DEFAULT_EOL_CHARACTER SC_EOL_LF
#else
# define GEANY_DEFAULT_EOL_CHARACTER SC_EOL_CR
#endif


/** File Prefs. */
typedef struct GeanyFilePrefs
{
	gint			default_new_encoding;
	gint			default_open_encoding;
	gboolean		final_new_line;
	gboolean		strip_trailing_spaces;
	gboolean		replace_tabs;
	gboolean		tab_order_ltr;
	gboolean		tab_order_beside;
	gboolean		show_tab_cross;
	guint			mru_length;
	gint			default_eol_character;
	gint			disk_check_timeout;
	gboolean		cmdline_new_files;	/* New file if command-line filename doesn't exist */
	gboolean		use_safe_file_saving;
	gboolean		ensure_convert_new_lines;
	gboolean		gio_unsafe_save_backup;
	gboolean		use_gio_unsafe_file_saving; /* whether to use GIO as the unsafe backend */
	gchar			*extract_filetype_regex;	/* regex to extract filetype on opening */
	gboolean		tab_close_switch_to_mru;
}
GeanyFilePrefs;


/**
 *  Structure for representing an open tab with all its properties.
 **/
typedef struct GeanyDocument
{
	/** Flag used to check if this document is valid when iterating @ref documents_array. */
	gboolean		 is_valid;
	gint			 index;		/**< Index in the documents array. */
	/** Whether this document supports source code symbols(tags) to show in the sidebar. */
	gboolean		 has_tags;
	/** The UTF-8 encoded file name.
	 * Be careful; glibc and GLib file functions expect the locale representation of the
	 * file name which can be different from this.
	 * For conversion into locale encoding, you can use @ref utils_get_locale_from_utf8().
	 * @see real_path. */
	gchar 			*file_name;
	/** The encoding of the document, must be a valid string representation of an encoding, can
	 *  be retrieved with @ref encodings_get_charset_from_index. */
	gchar 			*encoding;
	/** Internally used flag to indicate whether the file of this document has a byte-order-mark. */
	gboolean		 has_bom;
	GeanyEditor *editor;	/**< The editor associated with the document. */
	/** The filetype for this document, it's only a reference to one of the elements of the global
	 *  filetypes array. */
	GeanyFiletype	*file_type;
	/** TMWorkObject object for this document, or @c NULL. */
	TMWorkObject	*tm_file;
	/** Whether this document is read-only. */
	gboolean		 readonly;
	/** Whether this document has been changed since it was last saved. */
	gboolean		 changed;
	/** The link-dereferenced, locale-encoded file name.
	 * If non-NULL, this indicates the file once existed on disk (not just as an
	 * unsaved document with a filename set).
	 *
	 * @note This is only assigned after a successful save or open - it should
	 * not be set elsewhere.
	 * @see file_name. */
	gchar 			*real_path;

	struct GeanyDocumentPrivate *priv;	/* should be last, append fields before this item */
}
GeanyDocument;

G_MODULE_EXPORT extern GPtrArray *documents_array;


/** Wraps @ref documents_array so it can be used with C array syntax.
 * @warning Always check the returned document is valid (@c doc->is_valid).
 *
 * Example: @code GeanyDocument *doc = documents[i]; @endcode
 * @see documents_array(). */
#define documents ((GeanyDocument **)(documents_array->pdata))

/** @deprecated Use @ref foreach_document() instead.
 * Iterates all valid documents.
 * Use like a @c for statement.
 * @param i @c guint index for document_index(). */
#ifndef GEANY_DISABLE_DEPRECATED
#define documents_foreach(i) foreach_document(i)
#endif

/** Iterates all valid document indexes.
 * Use like a @c for statement.
 * @param i @c guint index for @ref documents_array.
 *
 * Example:
 * @code
 * guint i;
 * foreach_document(i)
 * {
 * 	GeanyDocument *doc = documents[i];
 * 	g_assert(doc->is_valid);
 * }
 * @endcode */
#define foreach_document(i) \
	for (i = 0; i < documents_array->len; i++)\
		if (!((GeanyDocument*)documents_array->pdata[i])->is_valid)\
			{}\
		else /* prevent outside 'else' matching our macro 'if' */

/** Null-safe way to check @ref GeanyDocument::is_valid.
 * @note This should not be used to check the result of the main API functions,
 * these only need a NULL-pointer check - @c document_get_current() != @c NULL. */
#define DOC_VALID(doc_ptr) \
	((doc_ptr) != NULL && (doc_ptr)->is_valid)

/**
 *  Returns the filename of the document passed or @c GEANY_STRING_UNTITLED
 *  (e.g. _("untitled")) if the document's filename was not yet set.
 *  This macro never returns @c NULL.
 **/
#define DOC_FILENAME(doc) \
	(G_LIKELY((doc)->file_name != NULL) ? ((doc)->file_name) : GEANY_STRING_UNTITLED)

G_MODULE_EXPORT
GeanyDocument* document_new_file(const gchar *filename, GeanyFiletype *ft, const gchar *text);

G_MODULE_EXPORT
GeanyDocument *document_get_current(void);

G_MODULE_EXPORT
GeanyDocument* document_get_from_page(guint page_num);

G_MODULE_EXPORT
GeanyDocument* document_find_by_filename(const gchar *utf8_filename);

G_MODULE_EXPORT
GeanyDocument* document_find_by_real_path(const gchar *realname);

G_MODULE_EXPORT
gboolean document_save_file(GeanyDocument *doc, gboolean force);

G_MODULE_EXPORT
GeanyDocument* document_open_file(const gchar *locale_filename, gboolean readonly,
		GeanyFiletype *ft, const gchar *forced_enc);

G_MODULE_EXPORT
void document_open_files(const GSList *filenames, gboolean readonly, GeanyFiletype *ft,
		const gchar *forced_enc);

G_MODULE_EXPORT
gboolean document_remove_page(guint page_num);

G_MODULE_EXPORT
gboolean document_reload_file(GeanyDocument *doc, const gchar *forced_enc);

G_MODULE_EXPORT
void document_set_encoding(GeanyDocument *doc, const gchar *new_encoding);

G_MODULE_EXPORT
void document_set_text_changed(GeanyDocument *doc, gboolean changed);

G_MODULE_EXPORT
void document_set_filetype(GeanyDocument *doc, GeanyFiletype *type);

G_MODULE_EXPORT
gboolean document_close(GeanyDocument *doc);

G_MODULE_EXPORT
GeanyDocument *document_index(gint idx);

G_MODULE_EXPORT
gboolean document_save_file_as(GeanyDocument *doc, const gchar *utf8_fname);

G_MODULE_EXPORT
void document_rename_file(GeanyDocument *doc, const gchar *new_filename);

G_MODULE_EXPORT
const GdkColor *document_get_status_color(GeanyDocument *doc);

G_MODULE_EXPORT
gchar *document_get_basename_for_display(GeanyDocument *doc, gint length);

G_MODULE_EXPORT
gint document_get_notebook_page(GeanyDocument *doc);

G_MODULE_EXPORT
gint document_compare_by_display_name(gconstpointer a, gconstpointer b);

G_MODULE_EXPORT
gint document_compare_by_tab_order(gconstpointer a, gconstpointer b);

G_MODULE_EXPORT
gint document_compare_by_tab_order_reverse(gconstpointer a, gconstpointer b);

G_END_DECLS

#endif /* GEANY_DOCUMENT_H */
