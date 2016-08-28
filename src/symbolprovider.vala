namespace Geany
{

	public abstract class SymbolProvider : Object, FeatureProvider
	{
		public abstract void activate();
		public abstract void deactivate();
		public abstract void symbolize(Gtk.TreeModel model);
	}

}
