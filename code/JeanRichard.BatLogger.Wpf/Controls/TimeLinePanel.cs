using System.Windows;
using System.Windows.Controls;

namespace JeanRichard.BatLogger.Wpf.Controls
{
	public class TimeLinePanel : Panel
	{
		public static readonly DependencyProperty SlotProperty = DependencyProperty.RegisterAttached("Slot", typeof(int), typeof(TimeLinePanel), new FrameworkPropertyMetadata(0, FrameworkPropertyMetadataOptions.AffectsParentArrange));

		public static void SetSlot(UIElement element, int value)
		{
			element.SetValue(SlotProperty, value);
		}

		public static int GetSlot(UIElement element)
		{
			return (int)element.GetValue(SlotProperty);
		}

		public static readonly DependencyProperty ItemValueProperty = DependencyProperty.RegisterAttached("ItemValue", typeof(int), typeof(TimeLinePanel), new FrameworkPropertyMetadata(0, FrameworkPropertyMetadataOptions.AffectsParentArrange));

		public static void SetItemValue(UIElement element, int value)
		{
			element.SetValue(ItemValueProperty, value);
		}

		public static int GetItemValue(UIElement element)
		{
			return (int)element.GetValue(ItemValueProperty);
		}

		public static readonly DependencyProperty SlotCountProperty = DependencyProperty.Register("SlotCount", typeof(int), typeof(TimeLinePanel), new FrameworkPropertyMetadata(10, FrameworkPropertyMetadataOptions.AffectsRender | FrameworkPropertyMetadataOptions.AffectsArrange));

		public int SlotCount
		{
			get { return (int)GetValue(SlotCountProperty); }
			set { SetValue(SlotCountProperty, value); }
		}

		private int _minItemValue;
		private int _maxItemValue;

		protected override Size MeasureOverride(Size availableSize)
		{
			_minItemValue = int.MaxValue;
			_maxItemValue = 0;

			if (Children.Count == 0)
			{
				return availableSize;
			}

			double itemWidth = availableSize.Width / SlotCount;

			foreach (UIElement child in InternalChildren)
			{
				int itemValue = GetItemValue(child);
				if (_minItemValue > itemValue)
				{
					_minItemValue = itemValue;
				}
				if (_maxItemValue < itemValue)
				{
					_maxItemValue = itemValue;
				}
				child.Measure(new Size(itemWidth, availableSize.Height));
			}
			return availableSize;
		}

		protected override Size ArrangeOverride(Size finalSize)
		{
			if (Children.Count == 0)
			{
				return finalSize;
			}

			double itemWidth = finalSize.Width / SlotCount;
			double heightFactor = finalSize.Height / _maxItemValue;
			
			foreach (UIElement child in InternalChildren)
			{
				int slot = GetSlot(child);
				if (slot >= SlotCount || slot < 0)
				{
					continue;
				}
				double itemHeight = GetItemValue(child) * heightFactor;
				double x = slot * itemWidth;
				child.Arrange(new Rect(x, finalSize.Height - itemHeight, itemWidth, itemHeight));
			}
			return finalSize;
		}
	}
}