using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using JeanRichard.BatLogger.Wpf.ViewModels;

namespace JeanRichard.BatLogger.Wpf.Controls
{
	public class TimeLineBar
	{
		private readonly List<BatCallSlotModel> _collectedDataPoints;

		public TimeLineBar()
		{
			_collectedDataPoints = new List<BatCallSlotModel>();
		}

		public void Add(BatCallSlotModel dataPoint)
		{
			_collectedDataPoints.Add(dataPoint);
			double s =  _collectedDataPoints.Sum(d => d.Frequency);
		}

		public int TotalCallCount
		{
			get { return _collectedDataPoints.Sum(d => d.Count); }
		}

		public int MeanFrequency
		{
			get { return (int)Math.Round(_collectedDataPoints.Average(d => d.Frequency) / 1000); }
		}

		public int Slot { get; set; }

		public override string ToString()
		{
			return string.Format("{0}: {1}", Slot, TotalCallCount);
		}
	}

	public partial class TimeLine : UserControl
	{
		public static readonly DependencyProperty ItemsSourceProperty;

		public static readonly DependencyProperty InternalItemsSourceProperty;

		public static readonly DependencyProperty SlotCountProperty;

		public static readonly DependencyProperty SelectedItemProperty;

		static TimeLine()
		{
			ItemsSourceProperty = DependencyProperty.Register("ItemsSource", typeof(IEnumerable<BatCallSlotModel>), typeof(TimeLine), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsMeasure | FrameworkPropertyMetadataOptions.AffectsArrange));
			InternalItemsSourceProperty = DependencyProperty.Register("InternalItemsSource", typeof(IEnumerable), typeof(TimeLine), new FrameworkPropertyMetadata(null));
			SlotCountProperty = DependencyProperty.Register("SlotCount", typeof(int), typeof(TimeLine), new FrameworkPropertyMetadata(10));
			SelectedItemProperty = DependencyProperty.Register("SelectedItem", typeof(TimeLineBar), typeof(TimeLine), new FrameworkPropertyMetadata(null));
		}

		public TimeLine()
		{
			InitializeComponent();
			LayoutRoot.DataContext = this;
		}

		public IEnumerable InternalItemsSource
		{
			get { return (IEnumerable)GetValue(InternalItemsSourceProperty); }
			set
			{
				if (value == null)
				{
					ClearValue(InternalItemsSourceProperty);
				}
				else
				{
					SetValue(InternalItemsSourceProperty, value);
				}
			}
		}

		public TimeLineBar SelectedItem
		{
			get { return (TimeLineBar)GetValue(SelectedItemProperty); }
			set { SetValue(SelectedItemProperty, value); }
		}

		public IEnumerable<BatCallSlotModel> ItemsSource
		{
			get { return (IEnumerable<BatCallSlotModel>)GetValue(ItemsSourceProperty); }
			set
			{
				if (value == null)
				{
					ClearValue(ItemsSourceProperty);
				}
				else
				{
					SetValue(ItemsSourceProperty, value);
				}
			}
		}

		public int SlotCount
		{
			get { return (int)GetValue(SlotCountProperty); }
			set { SetValue(SlotCountProperty, value); }
		}

		protected override Size MeasureOverride(Size constraint)
		{
			if (ItemsSource != null)
			{
				DateTime first = DateTime.MaxValue;
				DateTime last = DateTime.MinValue;
				foreach (BatCallSlotModel item in (ItemsSource))
				{
					if (first > item.SlotStartTime)
					{
						first = item.SlotStartTime;
					}
					if (last < item.SlotStartTime)
					{
						last = item.SlotStartTime;
					}
				}

				long timeDiff = (last.Ticks + 1) - first.Ticks;
				SlotCount = (int)constraint.Width / 5;
				double slotTime = timeDiff / (double)SlotCount;

				List<TimeLineBar> bars = new List<TimeLineBar>();
				long nextSlotEnd = 0;
				TimeLineBar bar = null;
				foreach (BatCallSlotModel item in (ItemsSource))
				{
					if (item.SlotStartTime.Ticks < nextSlotEnd)
					{
						bar.Add(item);
					}
					else
					{
						bar = new TimeLineBar();
						bar.Slot = (int)((item.SlotStartTime.Ticks - first.Ticks) / slotTime);
						nextSlotEnd = (long)(first.Ticks + (slotTime * (bar.Slot + 1)));
						bar.Add(item);
						bars.Add(bar);
					}
				}
				InternalItemsSource = bars;
			}
			else
			{
				InternalItemsSource = Enumerable.Empty<TimeLineBar>();
			}

			return base.MeasureOverride(constraint);
		}

		private static void OnItemsSourceChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{}
	}
}