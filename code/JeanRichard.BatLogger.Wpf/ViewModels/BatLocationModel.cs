using System;
using System.Collections.Generic;
using System.Linq;
using JeanRichard.BatLogger.Wpf.Controls;
using JeanRichard.BatLogger.Wpf.Coordinates;
using JeanRichard.BatLogger.Wpf.Domain;
using MapControl;

namespace JeanRichard.BatLogger.Wpf.ViewModels
{
	public class BatCallSlotModel : BaseViewModel
	{
		private readonly IEnumerable<BatSignal> _signals;

		public BatCallSlotModel(IEnumerable<BatSignal> signals)
		{
			_signals = signals.ToList();

			double totalFreq = 0;
			DateTime firstSignal = DateTime.MaxValue;
			DateTime lastSignal = DateTime.MinValue;
			long lastDuration = 0;
			foreach (BatSignal signal in _signals)
			{
				totalFreq += signal.Frequency;
				if (firstSignal > signal.Time)
				{
					firstSignal = signal.Time;
				}
				if (lastSignal < signal.Time)
				{
					lastSignal = signal.Time;
					//lastDuration = signal.DurationMs;
				}
			}

			Count = _signals.Count();
			Frequency = totalFreq / Count;
			SlotStartTime = firstSignal;
			SlotEndTime = lastSignal.AddMilliseconds(lastDuration);
		}

		public double Frequency { get; private set; }
		public int Count { get; private set; }

		public DateTime SlotStartTime { get; set; }
		public DateTime SlotEndTime { get; set; }
	}

	public class BatLocationModel : BaseViewModel
	{
		private readonly LogSessionLocation _logSessionLocation;

		private readonly BatModel _parentModel;
		private IEnumerable<BatCallSlotModel> _callSlots;
		private BatCallSlotModel _currentCall;
		private SwissGrid _swissGrid;
		private DateTime _firstCall;
		private DateTime _lastCall;

		public BatLocationModel(LogSessionLocation logSessionLocation, BatModel parentModel)
		{
			_swissGrid = new SwissGrid();
			_logSessionLocation = logSessionLocation;
			_parentModel = parentModel;
			_parentModel.PropertyChanged += (sender, args) =>
			{
				if (args.PropertyName.Equals("CurrentSlotTime"))
				{
					UpdateFields();
				}
			};
			UpdateFields();
		}



		public IEnumerable<BatCallSlotModel> Calls
		{
			get { return _callSlots; }
		}

		public int CallCount
		{
			get { return _callSlots.Count(); }
		}

		public BatCallSlotModel CurrentCall
		{
			set
			{
				_currentCall = value;
				OnPropertyChanged();
			}
			get { return _currentCall; }
		}

		public Location Location
		{
			get { return _swissGrid.Convert(_logSessionLocation.Location); }
		}


		public DateTime FirstCall
		{
			get { return _firstCall; }
			private set
			{
				_firstCall = value;
				OnPropertyChanged();
			}
		}

		public DateTime LastCall
		{
			get { return _lastCall; }
			private set
			{
				_lastCall = value; 
				OnPropertyChanged();
			}
		}

		public void UpdateFields()
		{
			int minSlotSizeMs = 50000;

			List<BatCallSlotModel> slots = new List<BatCallSlotModel>();
			List<BatSignal> groupedSignals = new List<BatSignal>(50);
			long lastTime = long.MinValue;
			foreach (BatSignal signal in _logSessionLocation.Signals)
			{
				if (signal.Frequency > 100000 || signal.Frequency < 5000)
				{
					continue;
				}
				groupedSignals.Add(signal);

				if ((signal.TimeMs - groupedSignals[0].TimeMs) > minSlotSizeMs && signal.TimeMs - lastTime > 500)
				{
					slots.Add(new BatCallSlotModel(groupedSignals));
					groupedSignals.Clear();
				}
				lastTime = signal.TimeMs;
			}
			FirstCall = slots.First().SlotStartTime;
			LastCall = slots.Last().SlotEndTime;
			_callSlots = slots;
		}
	}
}