/* geany-1.0.vapi generated by valac 0.20.1, do not modify. */

namespace Geany {
	[CCode (cheader_filename = "geanyapi.h")]
	public class Doc : GLib.Object {
		public string display_name { owned get; }
		public GeanyInternal.Document doc_ptr { get; }
		public string encoding { get; set; }
		public GLib.File file { owned get; }
		public Geany.FileType file_type { get; }
		public bool has_bom { get; }
		public bool has_tags { get; }
		public int index { get; }
		public bool is_changed { get; set; }
		public bool is_readonly { get; }
		public bool is_valid { get; }
		public signal void activated ();
		public signal void closed ();
		public signal void filetype_changed (Geany.FileType old_ft);
		public signal void reloaded ();
		public signal void saved ();
	}
	[CCode (cheader_filename = "geanyapi.h")]
	public class FileType : GLib.Object {
		public string extension { get; }
		public int id { get; }
		public int language { get; }
		public string name { get; }
		public string[] pattern { get; }
		public string title { get; }
	}
	[CCode (cheader_filename = "geanyapi.h")]
	public class FileTypeManager : GLib.Object {
		public unowned Geany.FileType detect_from_file (string utf8_fn);
		public static Geany.FileTypeManager get_default ();
		public unowned Geany.FileType lookup_by_name (string name);
	}
	[CCode (cheader_filename = "geanyapi.h")]
	public class View : GLib.Object {
	}
	[CCode (cheader_filename = "geanyapi.h")]
	public class Window : GLib.Object {
		public static Geany.Window get_default ();
		public Geany.Doc active_document { get; }
		public GLib.List<weak Geany.Doc> documents { owned get; }
		public GLib.List<weak Geany.Doc> unsaved_documents { owned get; }
		public Gtk.Window win_ptr { get; }
		public signal void document_activated (Geany.Doc doc);
		public signal void document_added (Geany.Doc doc);
		public signal void document_closed (Geany.Doc doc);
		public signal void document_filetype_changed (Geany.Doc doc, Geany.FileType old_ft);
		public signal void document_new (Geany.Doc doc);
		public signal void document_opened (Geany.Doc doc);
		public signal void document_reloaded (Geany.Doc doc);
		public signal void document_saved (Geany.Doc doc);
	}
}
