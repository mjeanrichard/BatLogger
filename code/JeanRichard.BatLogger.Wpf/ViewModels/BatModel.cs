using System;
using System.Collections.Generic;
using System.Linq;
using JeanRichard.BatLogger.Wpf.Coordinates;
using JeanRichard.BatLogger.Wpf.Domain;
using MapControl;

namespace JeanRichard.BatLogger.Wpf.ViewModels
{
	public class BatModel : BaseViewModel
	{
		private static readonly SwissGrid SwissGridHelper = new SwissGrid();

		private int _time;
		private DateTime _dataStartTime;
		private DateTime _dataEndTime;

		private DateTime _currentSlotTime;

		public BatModel(IEnumerable<LogSessionLocation> locations)
		{
			Initialize(locations);
		}

		protected void Initialize(IEnumerable<LogSessionLocation> loggerLocations)
		{
			TimeWindowsSize = TimeSpan.FromSeconds(1);

			_dataEndTime = DateTime.MinValue;
			_dataStartTime = DateTime.MaxValue;
			BatLocations = loggerLocations.Select(l => new BatLocationModel(l, this));
		}

		public IEnumerable<BatLocationModel> BatLocations { get; set; }
		
		public TimeSpan TimeWindowsSize { get; set; }

		public DateTime CurrentSlotTime
		{
			get { return _currentSlotTime; }
			set
			{
				_currentSlotTime = value;
				OnPropertyChanged();
			}
		}

		public int Time
		{
			get { return _time; }
			set
			{
				_time = value;
				CurrentSlotTime = new DateTime(2014, 4, 20).AddSeconds(value);
			}
		}
	}
}