using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JeanRichard.BatLogger.Wpf.Coordinates;

namespace JeanRichard.BatLogger.Wpf.Domain
{
	public class BatSignal
	{
		private readonly int _pulses;
		private readonly int _duration;

		public BatSignal(long time, int pulses, int duration, DateTime startTime)
		{
			TimeMs = time;
			_pulses = pulses;
			_duration = duration;

			Time = startTime.AddMilliseconds(time);

			Frequency = ((_pulses * 16) / (double)_duration) * 1000000;
		}

		public DateTime Time { get; private set; }

		public double Frequency { get; private set; }

		public long TimeMs { get; private set; }
	}

	public class LogImporter
	{
		public static LogSessionLocation Import(string filename, SwissLocation location)
		{
			IEnumerator<string> logLines = File.ReadLines(filename).GetEnumerator();

			logLines.MoveNext();
			string[] datelineParts = logLines.Current.Split(';');
			int timeOffset = int.Parse(datelineParts[0]);
			DateTime startDate = DateTime.Parse(datelineParts[1]);

			//Header Zeile
			logLines.MoveNext();

			List<BatSignal> signals = new List<BatSignal>();
			while(logLines.MoveNext())
			{
				signals.Add(ParseBatSignal(logLines.Current, timeOffset, startDate));
			}
			
			return new LogSessionLocation(location, signals);
		}

		private static BatSignal ParseBatSignal(string logLine, int timeOffset, DateTime startDate)
		{
			//start time;pulses;duration
			string[] parts = logLine.Split(';');
			int time = int.Parse(parts[0]);
			int pulses = int.Parse(parts[1]);
			int duration = int.Parse(parts[2]);
			BatSignal signal = new BatSignal(time - timeOffset, pulses, duration, startDate);
			return signal;
		}
	}
}
