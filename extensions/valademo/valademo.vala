using Geany;

namespace Vala
{
	public class DemoExt : GLib.Object, Geany.WinCmd
	{
		public DemoExt() {
			Geany.ui_set_statusbar(false, "DemoExt.DemoExt()");
		}

		~DemoExt() {
			Geany.ui_set_statusbar(false, "DemoExt.~DemoExt()");
		}

		public void activate() {
			Geany.ui_set_statusbar(false, "DemoExt.activate()");
		}

		public void deactivate() {
			Geany.ui_set_statusbar(false, "DemoExt.deactivate()");
		}

		public void update_state() {
			Geany.ui_set_statusbar(false, "DemoExt.update_state()");
		}

		public Gtk.Window window { get; set; }
	}
}

[ModuleInit]
public void peas_register_types(TypeModule module)
{
	var objmodule = module as Peas.ObjectModule;
	objmodule.register_extension_type(typeof(Geany.WinCmd), typeof(Vala.DemoExt));
}
