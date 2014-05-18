from gi.repository import GObject, Gtk, Geany

class PyDemo(GObject.Object, Geany.WinCmd):
	__gtype_name__ = "PyDemo"
	
	window = GObject.property(type=Gtk.Window)
	
	def __init__(self):
		GObject.Object.__init__(self)
		for a in sorted(dir(Geany)):
			print("Attr: ", a)
		Geany.ui_set_statusbar(False, "PyDemo.__init__()")
	
	def activate(self):
		Geany.ui_set_statusbar(False, "PyDemo.activate()")
	
	def deactivate(self):
		Geany.ui_set_statusbar(False, "PyDemo.deactivate()")
	
	def update_state(self):
		Geany.ui_set_statusbar(False, "PyDemo.update_state()")
