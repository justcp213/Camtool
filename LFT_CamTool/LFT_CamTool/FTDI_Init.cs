using System;
using System.Collections.Generic;
//using System.Linq;
using System.Text;
//using System.Threading.Tasks;
using System.Runtime.InteropServices;
using FTD2XXPASKALI;

using FTC_STATUS = System.UInt32;
using LPDWORD = System.UInt32;
using FTC_HANDLE = System.UInt32;
using DWORD = System.UInt32;
using LPSTR = System.Char;

namespace Camera
{
    public class FTDI_Init
    {
        public FTC_STATUS Ftstat;
        public FTD2XX_NET.FTDI ft2232 = new FTD2XX_NET.FTDI(); //INIT des .NET 
        IntPtr ftHandle = IntPtr.Zero;


        FTD2XXPASKALI.FTDI.FT_DEVICE_INFO_NODE[] DEVICEINFO = new FTD2XXPASKALI.FTDI.FT_DEVICE_INFO_NODE[100];

        #region LOAD_LIBRARIES

        [DllImportAttribute("FTDI_I2C.dll", EntryPoint = "I2C_GetDllVersion")]
        static extern FTC_STATUS GetDllVersion(byte[] pDllVersion, uint buufferSize);

        [DllImportAttribute("FTDI_I2C.dll", EntryPoint = "I2C_GetNumHiSpeedDevices")]
        static extern FTC_STATUS I2C_GetNumHiSpeedDevices(ref uint NumDevices);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS I2C_OpenHiSpeedDevice( string devname, uint locationID, 
                                                  string channel,  ref IntPtr pftHandle);
        [DllImportAttribute("FTDI_I2C.dll")]
        static extern int MeineTestFunktion();

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS I2C_GetHiSpeedDeviceNameLocIDChannel(uint deviceNameIndex, byte[] pDeviceName, 
                                                                uint deviceNameBufferSize, ref uint locationID, byte[] pChannel,
                                                                uint channelBufferSize, ref uint hiSpeedDeviceType);
        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS I2C_GetNumDevices(ref LPDWORD NumDevices);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS I2C_GetDllVersion(byte[] Versionbuffer, DWORD dwBufferSize);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS I2C_Open(ref IntPtr ftHandle);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS I2C_InitDevice(ref IntPtr ftHandle, DWORD Clockdivisor);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS ftdi_Open(int devicenumber, ref IntPtr ftHandle);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS ftdi_GetDriverVersion(IntPtr handle, ref LPDWORD Version);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS ftdi_Close(IntPtr ftHandle);

        [DllImportAttribute("FTDI_I2C.dll")]
        static extern FTC_STATUS ftdi_OpenEx(string arg1 , ref IntPtr ftHandle);

      //  [DllImportAttribute("ftci2c.dll")]
        //static extern FTC_STATUS ftdi_GetDeviceInfoList(ref _ft_device_list_info_node arg1, ref uint numdevs);

        //[DllImportAttribute("libMPSSE.dll", EntryPoint ="I2C_GetNumChannels", CallingConvention = CallingConvention.Cdecl)]
        //static extern uint I2C_GetNumChannels(ref uint numchannels);

        //[DllImportAttribute("libMPSSE.dll", EntryPoint = "Init_libMPSSE", CallingConvention = CallingConvention.Cdecl)]
        //static extern void Init_libMPSSE();


        #endregion

        public void TestFunktionen()
        {
            LPDWORD Num = 0;
            FTC_STATUS Status;
            DWORD length = 10;
            
           
            DWORD Clockdivis = 0;
            FTC_HANDLE ftHandle = 0;
            IntPtr handle = IntPtr.Zero;
            uint NumDevice = 0;

            //qwertz newclass = new qwertz();
            //Wirft Fehlermeldung aus:
            //System.IO.FileLoadException: Die Datei oder Assembly "FTDI_I2C, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null" oder eine Abhängigkeit davon wurde nicht gefunden. Die gefundene Manifestdefinition der Assembly stimmt nicht mit dem Assemblyverweis überein. (Ausnahme von HRESULT: 0x80131040)
            //bei Camera.FTDI_Init.TestFunktionen()

           
            


            FTDI paskali = new FTDI();
            FTDI.FT_STATUS ftstats = new FTDI.FT_STATUS();
            //  Status = I2C_GetNumDevices(ref Num);
            ftstats = paskali.GetDeviceList(DEVICEINFO);
            //Status = ftdi_GetDeviceInfoList(ref FT_DEVICE_LIST_INFO_NODE, ref NumDevice);
            Status = ftdi_OpenEx(DEVICEINFO[1].Description, ref handle);
           // Status = ftdi_Open(0, ref handle);

            

            Status = I2C_GetNumHiSpeedDevices(ref NumDevice);
            //  Status = ftdi_GetDriverVersion( handle, ref Num);
            byte[] pDevicename = new byte[100];
            char[] PDevicename = new char[100];
            byte[] pchannel = new byte[5];
            uint hispeeddevtype = 0;
            uint LocID = 0;


           
           

            Status =  I2C_GetHiSpeedDeviceNameLocIDChannel(1, pDevicename, 100, ref LocID, pchannel, 5, ref hispeeddevtype);

            string devname = Encoding.ASCII.GetString(pDevicename);
            devname = devname.Substring(0, devname.IndexOf("\0"));

            string hispeedchannel = Encoding.ASCII.GetString(pchannel);
            hispeedchannel = hispeedchannel.Substring(0, hispeedchannel.IndexOf("\0"));
            
            Status = I2C_OpenHiSpeedDevice(devname, LocID , hispeedchannel, ref handle);
            // devname ist ein char[] mit ""USB <-> Serial Converter A" Inhalt
            //LocID = 0
            //hispeedchannel char[5] mit inhalt "A"
            //handle ist 0


            Console.WriteLine("Status: " + Status);
            //byte[] bytepuffer = new byte[10];
            // Ftstat = I2C_GetDllVersion(ref bytebuffer, 10);
            // Console.WriteLine("Char " + bytebuffer);
            Status = ftdi_Close(handle);
        }

        public void ftdipaskali()
        {

            FTD2XXPASKALI.FTDI ftpaskali = new FTD2XXPASKALI.FTDI();
            FTD2XXPASKALI.FTDI.FT_STATUS ftstat = new FTDI.FT_STATUS();
            uint driverversion = 0;
            uint LibraryVersion = 0;

            ftstat = ftpaskali.OpenByIndex(0);

            ftstat = ftpaskali.GetDriverVersion(ref driverversion);
            
            ftstat = ftpaskali.GetLibraryVersion(ref LibraryVersion);

           
            Console.WriteLine(ftstat);

        }

        public void NET_LIB_OPENBYINDEX()
        {
            FTD2XX_NET.FTDI.FT_STATUS Status;
            FTD2XX_NET.FTDI.FT_DEVICE_INFO_NODE[] DevList = new FTD2XX_NET.FTDI.FT_DEVICE_INFO_NODE[10];
            string Description;
            uint LibVer = 0;
            uint DevCount = 0;
            string SerialNo;
            uint bytes2read = 1024;
            byte[] data = new byte[bytes2read];

           

            Status = ft2232.OpenByIndex(0);
            
            Status = ft2232.GetDescription(out Description);
            Status = ft2232.GetLibraryVersion(ref LibVer);
            Status = ft2232.GetNumberOfDevices(ref DevCount);
            Status = ft2232.GetSerialNumber(out SerialNo);
            Status = ft2232.GetDeviceList(DevList);

            //SETUP I2C HERE


            //Status = ft2232.Read(data, 1024, ref bytes2read);

            Console.WriteLine("Status " + Status + " Description " + Description);

            //Write PIX_CLK_INIT
            //Reset_Cam '\x0D'
            //ResetBit = 1;
            //dByte0 ist lower_Byte2Write -> LOW_BYTE(ResetBit) ->
        }


        public string _GetDllVersion(byte[] pDllVersion, uint bufferSize)
        {
            //Bibliotheksversion 
            string version = "";
            int x = MeineTestFunktion();   //Selbst in der DLL geschrieben! Soll x=2103 ergeben
            uint int_version =  GetDllVersion(pDllVersion, bufferSize);

            version = Encoding.ASCII.GetString(pDllVersion);
            Console.WriteLine("VERSION " + version);
            return version;
        }

        public uint _GetNumDevices(uint Numdevices)
        {
            //Zeigt die Anzahl der I2C Teilnehmer am FT2232H
            Ftstat = I2C_GetNumHiSpeedDevices(ref Numdevices);

            return Numdevices;
        }

        public void GetHiSpeedDeviceInfo()
        {
            uint deviceNameIndex = 0;
            byte[] pDeviceName = new byte[100];
            uint deviceNameBufferSize = 100;
            uint locationID = 0;
            byte[] pChannel = new byte[5];   //5 moegliche Kanaele
            uint channelBufferSize = 5;
            uint hiSpeedDeviceType = 0;
            
            uint ftHandle = 0;

            //Abfrage///////////////////////////////////////////////////////////////////////////////////////////////////////////
           // Ftstat = I2C_GetHiSpeedDeviceNameLocIDChannel(deviceNameIndex, pDeviceName, deviceNameBufferSize, ref locationID, //
           //                                                 pChannel, channelBufferSize, ref hiSpeedDeviceType);              //

            //                                                                                                                //
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            //Auswertung
            string DevName = Encoding.ASCII.GetString(pDeviceName).Substring(0, Encoding.ASCII.GetString(pDeviceName).IndexOf("\0"));
            string ChannelName = Encoding.ASCII.GetString(pChannel).Substring(0, Encoding.ASCII.GetString(pChannel).IndexOf("\0"));
            string s =  deviceNameIndex.ToString() + " " + DevName + " " +  locationID.ToString() + " " +
                         ChannelName + " "+ hiSpeedDeviceType.ToString();
            Console.WriteLine("HispeedDev: " + s);

           
        }

        //public void GetChannelsI2C()
        //{
        //    uint channel = 0;
        //    uint FSTAT = 0;

        //    Init_libMPSSE();
        //    FSTAT = I2C_GetNumChannels(ref channel);
        //    Console.WriteLine(channel);
        //}
        
        


    }


}
