using System.Runtime.Caching;
using System.Windows;
using JeanRichard.BatLogger.Wpf.ViewModels;
using MapControl;

namespace JeanRichard.BatLogger.Wpf
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
	        TileImageLoader.Cache = MemoryCache.Default;
			
			InitializeComponent();
            DataContext = new BatModel();
			
        }
    }
}
