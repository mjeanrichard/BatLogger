using System;
using System.Collections.Generic;
using JeanRichard.BatLogger.Wpf.Coordinates;

namespace JeanRichard.BatLogger.Wpf.ViewModels
{
	public class BatModel : BaseViewModel
	{
		private int _time;
		private DateTime _timeWindowStart;
		public IEnumerable<BatLocationModel> BatLocations { get; set; }
		public TimeSpan TimeWindowsSize { get; set; }

		public DateTime TimeWindowStart
		{
			get { return _timeWindowStart; }
			set
			{
				_timeWindowStart = value; 
				OnPropertyChanged();
			}
		}


		public int Time
		{
			get { return _time; }
			set
			{
				_time = value;
				TimeWindowStart = new DateTime(2014, 4, 20).AddSeconds(value);
			}
		}


		public BatModel()
		{
			TimeWindowsSize = TimeSpan.FromSeconds(1);

			SwissGrid swiss = new SwissGrid();
			List<BatLocationModel> locations = new List<BatLocationModel>();

			BatLocationModel batLocationModel = new BatLocationModel(new[]
			{
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 1)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 1)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 1)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 8)}
			}, this);
			batLocationModel.Location = swiss.Convert(new SwissLocation(2646772, 1250433));
			locations.Add(batLocationModel);

			batLocationModel = new BatLocationModel(new[]
			{
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 1)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 2)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 2)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 2)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 7)},
				new BatSighting {Time = new DateTime(2014, 4, 20, 0, 0, 7)}
			}, this);
			batLocationModel.Location = swiss.Convert(new SwissLocation(2646672, 1250533));
			locations.Add(batLocationModel);

			BatLocations = locations;
		}
	}
}