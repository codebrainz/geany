namespace Geany
{

	public abstract class Highlighter : Object, FeatureProvider
	{
		public abstract void activate();
		public abstract void deactivate();
		public abstract void init_styles();
		public abstract void highlight(int start, int end);
	}

}
