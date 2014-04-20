using System.ComponentModel;
using System.Runtime.CompilerServices;
using JeanRichard.BatLogger.Wpf.Annotations;

namespace JeanRichard.BatLogger.Wpf.ViewModels
{
	public class BaseViewModel : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		[NotifyPropertyChangedInvocator]
		protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
		{
			PropertyChangedEventHandler handler = PropertyChanged;
			if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
		}

	}
}