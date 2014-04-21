using System;
using System.Collections.Generic;
using System.Linq;
using JeanRichard.BatLogger.Wpf.Domain;
using MapControl;

namespace JeanRichard.BatLogger.Wpf.ViewModels
{
	public class BatCallModel : BaseViewModel
	{
		public BatCallModel()
		{}

		public DateTime SlotTime { get; set; }
	}

	public class BatLocationModel : BaseViewModel
	{
		private readonly LoggerLocation _loggerLocation;

		private readonly BatModel _parentModel;
		private IEnumerable<BatCallModel> _calls;
		private BatCallModel _currentCall;

		public BatLocationModel(LoggerLocation location, BatModel parentModel)
		{
			_loggerLocation = location;
			_parentModel = parentModel;
			_parentModel.PropertyChanged += (sender, args) =>
			{
				if (args.PropertyName.Equals("CurrentSlotTime"))
				{
					UpdateFields();
				}
			};
		}

		public IEnumerable<BatCallModel> Calls
		{
			get { return _calls; }
		}

		public BatCallModel CurrentCall
		{
			set
			{
				_currentCall = value;
				OnPropertyChanged();
			}
			get { return _currentCall; }
		}

		public Location Location { get; set; }

		public void UpdateFields()
		{
			_currentCall = _calls.FirstOrDefault(c => c.SlotTime == _parentModel.CurrentSlotTime);
		}
	}
}