/*
 * This is the raw/internal VAPI bindings to Geany's C internals. Don't use
 * this VAPI directly except from the actual wrapper code found in this
 * file's directory.
 */

[CCode(cheader_fileanme="geany.h",
       cname="Geany",
       gir_namespace="GeanyInternal",
       gir_version="1.0")]
namespace GeanyInternal
{

	[CCode(cheader_filename="document.h",
	       cname="struct GeanyDocument",
	       cprefix="document_",
	       free_function="",
	       has_type_id=false)]
	[Compact]
	public class Document
	{
		// Fields
		public string file_name;
		public string real_path;
		public bool changed;
		public string encoding;
		public bool has_bom;
		public bool has_tags;
		public int index;
		public bool readonly;
		public Filetype file_type;

		// Methods
		public static unowned Document get_current();
		public bool close(Document doc);
		public string get_basename_for_display(int len=-1);
		public void set_encoding(string new_enc);
		public void set_text_changed(bool changed);
	}

	[CCode(cheader_filename="filetypes.h",
	       cname="struct GeanyFiletype",
	       cprefix="filetypes_",
	       free_function="",
	       has_type_id=false)]
	[Compact]
	public class Filetype
	{
		// Fields
		public string extension;
		public int id;
		public int lang;
		public string name;
		[CCode(array_null_terminated=true)]
		public string[] pattern;
		public string title;

		// Properties
		public string display_name { get; }

		// Static methods
		[CCode(cname="filetypes_index")]
		public static unowned Filetype from_id(int ft_id);
		public static unowned Filetype lookup_by_name(string name);
		public static unowned Filetype detect_from_file(string utf8_fn);
	}

	[CCode(cname="filetypes_array")]
	GLib.PtrArray filetypes_array;

	[CCode(cheader_filename="geanyobject.h", cname="GeanyObject")]
	public class Object
	{
		// Signals
		public signal void document_activate(Document doc);
		public signal void document_before_save(Document doc);
		public signal void document_close(Document doc);
		public signal void document_filetype_set(Document doc, Filetype old_ft);
		public signal void document_new(Document doc);
		public signal void document_open(Document doc);
		public signal void document_reload(Document doc);
		public signal void document_save(Document doc);

		[CCode(cname="geany_object_get_default")]
		public static unowned GeanyInternal.Object instance();
	}

	[CCode(cheader_filename="editor.h",
	       cname="struct GeanyEditor",
	       cprefix="editor_",
	       free_function="",
	       has_type_id=false)]
	[Compact]
	public class Editor
	{
		// Fields
		public bool auto_indent;
		public Document document;
		public bool line_breaking;
		public bool line_wrapping;
		public Scintilla.Object sci;
		public float scroll_percent;

		// Properties
		public string eol_char { get; }
		public int eol_char_len { get; }
		public int eol_char_mode { get; }
		public string eol_char_name { get; }
		// TODO: get_indent_prefs()

		// Methods
		public string editor_find_snippet(string snippet_name);
		public string get_word_at_position(int pos=-1, string? wordchars=null);
		public bool goto_position(int pos, bool mark=false);
		public void indicator_clear(int indic);
		public void indicator_set_on_line(int indic, int line);
		public void indicator_set_on_range(int indic, int start, int end);
		public void insert_snippet(int pos, string snippet);
		public void editor_insert_text_block(string text, int pos,
			int cursor_pos=-1, int newline_indent_size=-1,
			bool replace_newlines=false);
		public void set_indent_type(int type); // FIXME: add GeanyIndentType enum
	}

	[CCode(cheader_filename="ScintillaWidget.h", cprefix="sci_")]
	namespace Scintilla
	{
		[CCode(cname="ScintillaObject")]
		public class Object : Gtk.Widget
		{
			[CCode(cname="scintilla_new")]
			public Object();

			// Properties
			public int current_line { get; }
			public int current_position { get; set; }
			public int length { get; }
			public int lexer { get; }
			public int line_count { get; }
			public int selected_text_length { get; }
			public string selection_contents { owned get; }
			public int selection_end { get; set; }
			public int selection_start { get; set; }
			public int selection_mode { get; set; }
			public int tab_width { get; }
			public bool has_selection { get; }

			// Methods
			[CCode(cname="scintilla_send_message")] // FIXME: wrap uptr_t/sptr_t
			public long send_message(int msg, ulong wparam=0, long lparam=0);
			public void delete_marker_at_line(int line, int marker);
			public void start_undo_action();
			public void end_undo_action();
			public void ensure_line_is_visible(int line);
			public int find_matching_brace(int pos);
			// TODO: sci_find_text
			public char get_char_at(int pos);
			public int col_from_position(int pos);
			public string get_contents(int len=-1);
			public string get_contents_range(int start, int end);
			public string get_line(int line_num);
			public int get_line_end_position(int line);
			public int get_line_from_position(int pos);
			public int get_line_indentation(int line);
			public bool get_line_is_visible(int line);
			public int get_line_length(int line);
			public int get_position_from_line(int line);
			public void get_selected_text(out string text);
			public int get_style_at(int pos);
			public void goto_line(int line);
			public void indicator_clear(int pos, int len);
			public void indicator_set(int indic);
			public void insert_text(int pos, string text);
			public bool is_marker_set_at_line(int line, int marker);
			public void replace_sel(string text);
			public void scroll_caret();
			public void set_font(int style, string font_name, int point_size);
			public void set_line_indentation(int line, int indent);
			public void set_marker_at_line(int line, int marker);
			public void set_text(string text);
		}
	}

	[CCode(cheader_filename="ui_utils.h")]
	namespace UiUtils
	{
		[CCode(cname="ui_builder_get_object")]
		public GLib.Object get_object(string name);
	}

	[CCode(cheader_filename="utils.h")]
	namespace Utils
	{
		[CCode(cname="utils_get_locale_from_utf8")]
		string utf8_to_locale(string utf8string);
	}

} // GeanyInternal
