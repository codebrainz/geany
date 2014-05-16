namespace Geany
{
	[CCode(cheader_filename="wincmd.h", type_cname="GeanyWinCmdInterface")]
	public interface WinCmd
	{
		public abstract void activate();
		public abstract void deactivate();
		public abstract void update_state();
	}
	[CCode(cheader_filename="geanywrap.h", cname="ui_set_statusbar")]
	void ui_set_statusbar(bool log, string fmt);
}
