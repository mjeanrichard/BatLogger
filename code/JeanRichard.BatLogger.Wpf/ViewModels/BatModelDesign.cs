using System;
using System.Collections.Generic;
using System.Linq;
using JeanRichard.BatLogger.Wpf.Coordinates;
using JeanRichard.BatLogger.Wpf.Domain;

namespace JeanRichard.BatLogger.Wpf.ViewModels
{
	public class BatModelDesign : BatModel
	{
		public BatModelDesign() : base(Enumerable.Empty<LogSessionLocation>())
		{
			//List<LoggerLocation> locations = new List<LoggerLocation>();
			//locations.Add(new LoggerLocation(new SwissLocation(2646772, 1250433), new[]
			//{
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 50},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4), Frequency = 25},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 4), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 8), Frequency = 15}
			//}));
			//locations.Add(new LoggerLocation(new SwissLocation(2646672, 1250533), new[]
			//{
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 50},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 0), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 2), Frequency = 15},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 7), Frequency = 5},
			//	new BatCall {Time = new DateTime(2014, 4, 20, 0, 0, 7), Frequency = 5}
			//}));

			//Initialize(locations);
		}


	}
}