using System;
using MapControl;
using swisstopo.geodesy.reframe;

namespace JeanRichard.BatLogger.Wpf.Coordinates
{
	public struct SwissLocation
	{
		public SwissLocation(int x, int y):this()
		{
			X = x;
			Y = y;
		}

		public int X { get; private set; }
		public int Y { get; private set; }
	}

	public class SwissGrid
	{
		// 0 -> Global geocentric coordinates (CHTRS95/ETRS89/WGS84) and height on GRS80 → Swiss plane coordinates LV95 (CH1903+) and height on Bessel 1841 
		// 1 -> Global geographic coordinates (CHTRS95/ETRS89/WGS84) and height on GRS80 → Swiss plane coordinates LV95 (CH1903+) and height on Bessel 1841 
		// 2 -> Swiss plane coordinates LV95 (CH1903+) and height on Bessel 1841 → Global geocentric coordinates (CHTRS95/ETRS89/WGS84) and height on GRS80 
		// 3 -> Swiss plane coordinates LV95 (CH1903+) and height on Bessel 1841 → Global geographic coordinates (CHTRS95/ETRS89/WGS84) and height on GRS80
		private const int WGS84_LV95 = 1;
		private const int LV95_WGS84 = 3;


		private readonly Reframe _reframe = new Reframe();

		public SwissGrid()
		{
		}

		public Location Convert(SwissLocation location)
		{
			double xTmp = location.X;
			double yTmp = location.Y;
			double zTmp = 555;
			int result = _reframe.ComputeGpsref(ref xTmp, ref yTmp, ref zTmp, LV95_WGS84);

			if (result == -1)
			{
				throw new InvalidOperationException("Coordinates are outside the Swiss boundaries.");
			}
			if (result == -2)
			{
				throw new InvalidOperationException("Unsupported value for “flag” argument.");
			}
			if (result != 1)
			{
				throw new InvalidOperationException("Unknown Error Code from, Reframe!");
			}
			return new Location(yTmp, xTmp);
		}
	}
}