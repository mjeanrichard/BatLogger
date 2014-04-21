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
		private DateTime _timeWindowStart;

		public BatModel(IEnumerable<LoggerLocation> locations)
		{
			Initialize(locations);
		}

		private void Initialize(IEnumerable<LoggerLocation> loggerLocations)
		{
			TimeWindowsSize = TimeSpan.FromSeconds(1);
			BatLocations = loggerLocations.Select(l => new BatLocationModel(l)).ToList();
		}

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
	}

	class BatModelImpl : BatModel
	{
		public BatModelImpl() : base(Enumerable.Empty<LoggerLocation>())
		{
			List<BatLocationModel> locations = new List<BatLocationModel>();
			BatLocationModel batLocationModel = new BatLocationModel(new[]
			{
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 8)}
			}, this);
			batLocationModel.Location = SwissGridHelper.Convert(new SwissLocation(2646772, 1250433));
			locations.Add(batLocationModel);

			batLocationModel = new BatLocationModel(new[]
			{
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 2)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 7)},
				new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 7)}
			}, this);

			batLocationModel.Location = SwissGridHelper.Convert(new SwissLocation(2646672, 1250533));
			locations.Add(batLocationModel);
			
			BatLocations = locations;
			Initialize();
		}


		}
	}
}