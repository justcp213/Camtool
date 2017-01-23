using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
//using System.Linq;
using System.Text;
//using System.Threading.Tasks;
using System.Windows.Forms;
using FTD2XXPASKALI;



namespace LFT_CamTool
{
    
    public partial class Form1 : Form
    {
      // qwertz testclass = new qwertz();

        Camera.FTDI_Init CamControl = new Camera.FTDI_Init();
        
        public Form1()
        {
            
            InitializeComponent();           
            pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
            
           // int x  = testclass.return_value();
          //  Console.WriteLine("X: " + x);         
            
            
        }

        private void button_connect_Click(object sender, EventArgs e)
        {
            //  CamControl.GetChannelsI2C();
  

            CamControl.TestFunktionen();
            CamControl.ftdipaskali();
            CamControl.NET_LIB_OPENBYINDEX();
            // Set_DLLVersionStatus();
            //Get_NumberofDevices();
            CamControl.GetHiSpeedDeviceInfo();

           
           
            
        }


        private void Set_DLLVersionStatus()
        {
            
            byte[] byteDLLVersion = new byte[10];

            string s = CamControl._GetDllVersion(byteDLLVersion, 10);
            label1.Text = "ftci2c.dll Version " + s.TrimEnd('\0');
        }
        private void Get_NumberofDevices()
        {
            uint nodevices = 0;
            nodevices =  CamControl._GetNumDevices(nodevices);
            Console.WriteLine("Anzahl gefundener I2C Geraetscahften: " + nodevices);
            label2.Text = "Gefundene Slaves: " + nodevices.ToString();


            



        }




    }
}
