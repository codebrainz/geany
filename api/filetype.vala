using GeanyInternal;

namespace Geany
{
	public class FileType : GLib.Object
	{
		unowned Filetype ft;

		internal FileType(Filetype? ftype=null)
		{
			if (ftype == null)
				ft = Filetype.from_id(0 /* NONE */);
			else
				ft = ftype;
		}

		public string extension { get { return ft.extension; } }
		public int id { get { return ft.id; } }
		public int language { get { return ft.lang; } }
		public string name { get { return ft.name; } }
		public string[] pattern { get { return ft.pattern; } }
		public string title { get { return ft.title; } }
	}

	public class FileTypeManager : GLib.Object
	{
		HashTable<unowned Filetype?, FileType> fts =
			new HashTable<unowned Filetype?, FileType>(direct_hash, direct_equal);

		internal unowned FileType lookup_ft(Filetype gft)
		{
			return fts.lookup(gft);
		}

		public unowned FileType detect_from_file(string utf8_fn)
		{
			unowned Filetype gft = Filetype.detect_from_file(utf8_fn);
			return fts.lookup(gft);
		}

		public unowned FileType lookup_by_name(string name)
		{
			unowned Filetype gft = Filetype.lookup_by_name(name);
			return fts.lookup(gft);
		}

		// Singleton stuff
		FileTypeManager()
		{
			for (size_t i = 0; i < filetypes_array.len; i++)
			{
				unowned Filetype ft = (Filetype) filetypes_array.pdata[i];
				fts.insert(ft, new FileType(ft));
			}
		}
		static FileTypeManager inst = new FileTypeManager();
		public static FileTypeManager get_default() { return inst; }
	}
}
