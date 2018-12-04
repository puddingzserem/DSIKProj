using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using ClientApp.Models;
using System.Windows.Forms;
using System.Net.Sockets;

namespace ClientApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        string videoCache;
        List<string> logs = new List<string>();

        public MainWindow()
        {
            InitializeComponent();
        }

        #region server logic
        static void Connect(String server, String message, string ip, string pport)
        {
            try
            {
                // Create a TcpClient.
                // Note, for this client to work you need to have a TcpServer 
                // connected to the same address as specified by the server, port
                // combination.
                Int32 port = 13000;
                TcpClient client = new TcpClient(server, port);

                // Translate the passed message into ASCII and store it as a Byte array.
                Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

                // Get a client stream for reading and writing.
                //  Stream stream = client.GetStream();

                NetworkStream stream = client.GetStream();

                // Send the message to the connected TcpServer. 
                stream.Write(data, 0, data.Length);

                Console.WriteLine("Sent: {0}", message);

                // Receive the TcpServer.response.

                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                // Read the first batch of the TcpServer response bytes.
                Int32 bytes = stream.Read(data, 0, data.Length);
                responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                Console.WriteLine("Received: {0}", responseData);

                // Close everything.
                stream.Close();
                client.Close();
            }
            catch (ArgumentNullException e)
            {
                Console.WriteLine("ArgumentNullException: {0}", e);
            }
            catch (SocketException e)
            {
                Console.WriteLine("SocketException: {0}", e);
            }

            Console.WriteLine("\n Press Enter to continue...");
            Console.Read();
        }

        #endregion

        #region interaction logic
        private void DownloadEntity()
        {

        }
        private void UploadEntity(Entity entity)
        {

        }
        private void ConnectButtonClick(object sender, RoutedEventArgs e)
        {
            LogEvent("Requested connecting");
            ConnectButton.IsEnabled = false;
            CreateCache();
            SearchButton.IsEnabled = true;

        }
        private void LogEvent(string message)
        {
            logs.Add(message);
            LogBox.Items.Add(message);
        }
        #endregion

        #region window logic
        private void CreateCache()
        {
            LogEvent("Creating Cache");
            try
            {
                Directory.CreateDirectory("./_CacheDirectory");
            }
            catch(Exception e)
            {
                System.Windows.MessageBox.Show($"Error: {e.Data.ToString()}");
            }
        }

        private void SearchButtonClick(object sender, RoutedEventArgs e)
        {
            using (var folderDialog = new FolderBrowserDialog())
            {
                if (folderDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    UploadFolderPath.Text = folderDialog.SelectedPath;
                    UploadButton.IsEnabled = true;
                }
            }
        }

        private void Preview(Entity entityObject)
        {
            if (!String.IsNullOrEmpty(entityObject.GetEntityName()))
                ItemName.Text = entityObject.GetEntityName();
            else
                ItemName.Text = null;
            if (!String.IsNullOrEmpty(entityObject.GetEntityDescription()))
                ItemDescription.Text = File.ReadAllText(entityObject.GetEntityDescription());
            else
                ItemDescription.Text = null;
            if (!String.IsNullOrEmpty(entityObject.GetEntityImage()))
            {
                var uriSource = new Uri(entityObject.GetEntityImage());
                ItemThumbnail.Source = new BitmapImage(uriSource);
            }
            else
                ItemThumbnail.Source = null;
            if (!String.IsNullOrEmpty(entityObject.GetEntityVideo()))
            {
                ItemVideoButton.IsEnabled = true;
                videoCache = entityObject.GetEntityVideo();
            }
            else
            {
                ItemVideoButton.IsEnabled = false;
                videoCache = null;
            }
        }

        private void UploadButtonClick(object sender, RoutedEventArgs e)
        {
            LogEvent("Uploading entity data");
            String pathToFiles = UploadFolderPath.Text;
            String name = pathToFiles.Substring(pathToFiles.LastIndexOfAny(new char[] { '\\', '/' }) + 1);
            Entity newObject = new Entity(name);
            List <string> files = Directory.GetFiles(pathToFiles).ToList();
            //description
            string description = files.Find(p => p.Contains(".txt"));
            if (!String.IsNullOrEmpty(description)) newObject.SetEntityDescription(description);
            //image
            string image = files.Find(p => p.Contains(".jpg"));
            if (!String.IsNullOrEmpty(image)) newObject.SetEntityImage(image);
            //video
            string video = files.Find(p => p.Contains(".mp4"));
            if (!String.IsNullOrEmpty(video)) newObject.SetEntityVideo(video);
            Preview(newObject);
            ColorBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(253, 106, 2));
            UploadEntity(newObject);
        }

        private void ItemVideoButtonClick(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(videoCache);
        }
        #endregion
    }
}
