using System.Linq;
using System.Runtime.Caching;
using System.Windows;
using JeanRichard.BatLogger.Wpf.Coordinates;
using JeanRichard.BatLogger.Wpf.Domain;
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

	        LogSessionLocation logSessionLocation = LogImporter.Import(@"C:\Users\Meinrad\Desktop\Log01.csv", new SwissLocation(2646672, 1250533));

			DataContext = new BatModel(new[] { logSessionLocation });
        }
    }
}
