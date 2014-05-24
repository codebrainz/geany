using GeanyInternal;

namespace Geany
{
	public class View : GLib.Object
	{
		unowned Editor ed;

		internal View(Editor editor)
		{
			ed = editor;
		}
	}
}
