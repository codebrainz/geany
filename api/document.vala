using GeanyInternal;

namespace Geany
{

	public class Doc : GLib.Object
	{
		unowned Document doc;
		FileType ft;

		internal Doc(Document? gdoc=null)
		{
			if (gdoc == null)
				doc = Document.get_current();
			else
				doc = gdoc;
			ft = new FileType(doc.file_type);
		}

		public signal void closed();
		public signal void reloaded();
		public signal void saved();
		public signal void activated();
		public signal void filetype_changed(FileType old_ft);

		public bool is_valid
		{
			get { return Window.get_default().is_doc_valid(this); }
		}

		public Document doc_ptr { get { return doc; } }
		public FileType file_type { get { return ft; } }

		public File file
		{
			owned get
			{
				if (doc.real_path != null)
					return File.new_for_path(doc.real_path);
				else
					return File.new_for_path(Utils.utf8_to_locale(doc.file_name));
			}
		}

		public string display_name
		{
			owned get { return doc.get_basename_for_display(); }
		}

		public bool has_bom { get { return doc.has_bom; } }
		public bool has_tags { get { return doc.has_tags; } }
		public int index { get { return doc.index; } }
		public bool is_readonly { get { return doc.readonly; } }

		public bool is_changed
		{
			get { return doc.changed; }
			set { doc.set_text_changed(value); }
		}

		public string encoding
		{
			get { return doc.encoding; }
			set { doc.set_encoding(value); }
		}
	}

}
