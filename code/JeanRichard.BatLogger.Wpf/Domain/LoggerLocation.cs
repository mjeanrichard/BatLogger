using System;
using System.Collections.Generic;
using System.Linq;
using JeanRichard.BatLogger.Wpf.Coordinates;

namespace JeanRichard.BatLogger.Wpf.Domain
{
	public class LoggerLocation
	{
		public LoggerLocation(SwissLocation location, IEnumerable<BatCall> batCalls)
		{
			Location = location;
			BatCalls = batCalls.ToList();

			FirstCall = BatCalls.Min(c => c.Time);
			LastCall = BatCalls.Max(c => c.Time);
		}

		public SwissLocation Location { get; set; }
		public IEnumerable<BatCall> BatCalls { get; set; }

		public DateTime FirstCall { get; set; }
		public DateTime LastCall { get; set; }
	}
}