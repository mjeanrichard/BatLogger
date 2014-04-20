using System.Windows.Media;
using MapControl;

namespace JeanRichard.BatLogger.Wpf
{
	public class SwissMapTileSource : ImageTileSource
	{
		public override ImageSource LoadImage(int x, int y, int zoomLevel)
		{
			return base.LoadImage(x, y, zoomLevel);
		}
	}
}