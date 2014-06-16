using System.Collections.Generic;
using System.Linq;
using JeanRichard.BatLogger.Wpf.Coordinates;

namespace JeanRichard.BatLogger.Wpf.Domain
{
	public class LogSessionLocation
	{
		private readonly List<BatSignal> _signals;

		public LogSessionLocation(SwissLocation location, IEnumerable<BatSignal> batSignals)
		{
			Location = location;
			_signals = batSignals.ToList();
		}

		public SwissLocation Location { get; private set; }

		public IEnumerable<BatSignal> Signals
		{
			get { return _signals; }
		}
	}
}