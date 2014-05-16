from gi.repository import GObject, Gtk, Geany

class PyDemo(GObject.Object, Geany.WinCmd):
	__gtype_name__ = "PyDemo"
	
	window = GObject.property(type=Gtk.Window)
	
	def __init__(self):
		GObject.Object.__init__(self)
		print(sorted(dir(Geany)))
	
	def do_activate(self):
		pass
	
	def do_deactivate(self):
		pass
	
	def do_update_state(self):
		pass
