namespace Geany
{

	public interface FeatureProvider : Object
	{
		public abstract void activate();
		public abstract void deactivate();
	}

}
