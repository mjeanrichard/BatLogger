using System;
using System.Collections.Generic;
using System.Linq;

namespace JeanRichard.BatLogger.Wpf.Domain
{
	public class BatCall
	{
		private readonly List<BatSignal> _signals;

		public BatCall(List<BatSignal> signals)
		{
			if (signals.Count == 0)
			{
				throw new ArgumentException("No Signals");
			}
			_signals = signals;

			Frequency = _signals.Average(s => s.Frequency);
			TimeMs = _signals.Sum(s => s.TimeMs);
			Time = _signals.Min(s => s.Time);
		}

		public DateTime Time { get; private set; }

		public double Frequency { get; private set; }

		public long TimeMs { get; private set; }

		public int Count
		{
			get { return _signals.Count; }
		}
	}
}