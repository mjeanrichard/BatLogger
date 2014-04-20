using System;
using System.Collections.Generic;
using System.Linq;
using MapControl;

namespace JeanRichard.BatLogger.Wpf.ViewModels
{
	public class BatLocationModel : BaseViewModel
	{
		private readonly IEnumerable<BatSighting> _sightings;

		public IEnumerable<BatSighting> Sightings
		{
			get { return _sightings; }
		}

		private readonly BatModel _parentModel;
		private int _callCount;

		public BatLocationModel(IEnumerable<BatSighting> sightings, BatModel parentModel)
		{
			_sightings = sightings;
			_parentModel = parentModel;
			_parentModel.PropertyChanged += (sender, args) =>
			{
				if (args.PropertyName.Equals("TimeWindowStart"))
				{
					UpdateFields();
				}
			};
		}

		public Location Location { get; set; }

		public void UpdateFields()
		{
			DateTime endDate = _parentModel.TimeWindowStart.Add(_parentModel.TimeWindowsSize);
			CallCount = _sightings.Count(s => s.Time >= _parentModel.TimeWindowStart && s.Time < endDate);
		}

		public int CallCount
		{
			get { return _callCount; }
			set
			{
				_callCount = value;
				OnPropertyChanged();
			}
		}
	}
}