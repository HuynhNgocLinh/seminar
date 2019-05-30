using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using CircularBuffer;
using Newtonsoft.Json.Linq;

namespace Monitor
{
  public enum USB_State_T
  {
    STATE_GET_HEADER,
    STATE_GET_PAYLOAD_SIZE,
    STATE_GET_PAYLOAD_DATA,
    STATE_GET_CRC
  }



  public struct USB_Packet_T
  {
    public uint   Header;
    public uint   Payload_Size;
    public char[] Payload_Data;
    public byte   CRC_8;
    
    public USB_Packet_T(uint Header, uint Payload_Size, char[] Payload_Data, byte CRC_8)
    {
      this.Header       = Header;
      this.Payload_Size = Payload_Size;
      this.Payload_Data = Payload_Data;
      this.CRC_8        = CRC_8;
    }
  }


  public struct USB_Evt_T
  {
    public char[]         Data;

    public USB_Evt_T(char[] Data)
    {
      this.Data = Data;
    }
  }

  

  public partial class Form1 : Form
  {
    
    delegate void Set_Text_CB(string text);
    
    // Circular buffer
    static CircularBuffer<byte> cBuffer;

    static CircularBuffer<USB_Evt_T> cBuffer_Process_Data;

    USB_State_T usbState = USB_State_T.STATE_GET_HEADER;
    USB_Packet_T usbPacket = new USB_Packet_T(0, 0, new char[1000], 0);


    public int countEvt = 0;

    static TCP_IP_Cmd tcpCmd;
    // Thread
    //Thread mThread;

    // Thread threadProcess_Data;

    public Form1()
    {
      InitializeComponent();

      serialPort.DataReceived += new SerialDataReceivedEventHandler(Data_Receive);
      cBuffer                  = new CircularBuffer<byte>(1000, false);
      cBuffer_Process_Data     = new CircularBuffer<USB_Evt_T>(50, false);

      tcpCmd = new TCP_IP_Cmd();

      // Init thread receive data
      threadRecData = new BackgroundWorker();
      threadRecData.WorkerReportsProgress = true;
      threadRecData.WorkerSupportsCancellation = true;

      threadRecData.DoWork += threadRecData_DoWork;
      threadRecData.RunWorkerCompleted += threadRecData_RunWorkerCompleted;

      // Init thread process data
      threadProcData = new BackgroundWorker();
      threadProcData.WorkerReportsProgress = true;
      threadProcData.WorkerSupportsCancellation = true;

      threadProcData.DoWork += threadProcData_DoWork;
      //threadProcData.RunWorkerCompleted += threadProcData_RunWorkerCompleted;

      string[] baudRate = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

      baudRateBox.Items.AddRange(baudRate);

      //string[] COM = { "COM1", "COM2", "COM3", "COM4", "COM5" };

      //comBox.Items.AddRange(COM);
    }

    private void threadRecData_DoWork(object sender, DoWorkEventArgs e)
    {
      while (!cBuffer.IsEmpty)
        {
          switch (usbState)
          {
            case USB_State_T.STATE_GET_HEADER:
            usbPacket.Header = Convert.ToByte(cBuffer.Get());

              // Check header
              if (usbPacket.Header == 0x55)
              {
                usbState = USB_State_T.STATE_GET_PAYLOAD_SIZE;
              }
              else
              {
                usbState = USB_State_T.STATE_GET_HEADER;
              }

              break;

            case USB_State_T.STATE_GET_PAYLOAD_SIZE:
              if (cBuffer.Size > 2)
              {
                //t_Data = Array.ConvertAll(cBuffer.Get(2), uint.Parse);
                
                //usbPacket.Payload_Size = (t_Data[0] << 8) | t_Data[1];
                byte[] t_Data = cBuffer.Get(2);

                usbPacket.Payload_Size =  (uint)((t_Data[0] << 8) | t_Data[1]);

                if (usbPacket.Payload_Size <= 1000)
                {
                  usbState = USB_State_T.STATE_GET_PAYLOAD_DATA;
                }
                else 
                {
                  usbState = USB_State_T.STATE_GET_HEADER;
                }
              }

              break;
            case USB_State_T.STATE_GET_PAYLOAD_DATA:

              
              if (cBuffer.Size >= usbPacket.Payload_Size)
              {
                usbPacket.Payload_Data = System.Text.Encoding.ASCII.GetString(cBuffer.Get((int)usbPacket.Payload_Size)).ToCharArray();
                usbState = USB_State_T.STATE_GET_CRC;
              }
                break;

            case USB_State_T.STATE_GET_CRC:
              usbPacket.CRC_8 = Convert.ToByte(cBuffer.Get());

              // Calculate and verify CRC of packet
              CRC8Calc CRC = new CRC8Calc(CRC8_POLY.CRC8_CCITT);
              byte[] tPacket_Byte_Convert = Encoding.ASCII.GetBytes(usbPacket.Payload_Data);
              byte CRC_Value = CRC.Checksum(tPacket_Byte_Convert);
              if (CRC_Value == usbPacket.CRC_8)
              {
                USB_Update_To_Evt(usbPacket.Payload_Data, usbPacket.Payload_Size);
              }

              // Clear packet data
              usbPacket.Header = 0;
              usbPacket.Payload_Size = 0;
              Array.Clear(usbPacket.Payload_Data, 0, usbPacket.Payload_Data.Length);
              usbPacket.CRC_8 = 0;

              // Change to STATE_GET_HEADER
              usbState = USB_State_T.STATE_GET_HEADER;

            break;
            default:
                break;
          }
        }
    }
    private void threadRecData_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
    {
      countEvt++;
      Set_Text(countEvt.ToString());
    }

    private void threadProcData_DoWork(object sender, DoWorkEventArgs e)
    { 
      while (!cBuffer_Process_Data.IsEmpty)
      {
        USB_Evt_T tUSB = new USB_Evt_T(new char[1000]);
        tUSB = cBuffer_Process_Data.Get();
        TCP_Base_Type.TCP_IP_Status_T Status;

        string json = new string(tUSB.Data);

        JObject jsonParse = JObject.Parse(json);
        string Cmd = (string)jsonParse["Cmd"];

        if (Cmd == "TCP_check_status_connection")
        {
          TCP_Base_Type.TCP_IP_Info_T Data_Receive;
          Data_Receive = tcpCmd.checkStatusConnection();

          JObject response_Value = new JObject();
          response_Value["Data"] = "TCP_check_status_connection";
          response_Value["Status"] = "OK";
          JObject Value_Obj = new JObject();
          Value_Obj["IP_address"] = Data_Receive.ipAddr;
          Value_Obj["Port"] = Data_Receive.port;
          response_Value["Value"] = Value_Obj;

          USB_Convert_And_Send_Data(response_Value.ToString(), (uint)response_Value.ToString().Length);
        }
        else if (Cmd == "TCP_establish_connection")
        {
          JObject ValueObj = (JObject)jsonParse["Value"];

          string IP_Address = (string)ValueObj["IP_address"];
          int Port = (int)ValueObj["Port"];

          Status = tcpCmd.establishConnetion(IP_Address, Port);

          JObject response_Value = new JObject();
          response_Value["Data"] = "TCP_establish_connection";
          response_Value["Status"] = (Status == 0 ? "OK" : "EROR");

          USB_Convert_And_Send_Data(response_Value.ToString(), (uint)response_Value.ToString().Length);
        }
        else if (Cmd == "TCP_send_data")
        {
          JObject ValueObj = (JObject)jsonParse["Value"];

          string Data = (string)ValueObj["Data"];
          //int Len = (int)ValueObj["Len"];

          Status = tcpCmd.sendData(Data);

          JObject response_Value = new JObject();
          response_Value["Data"] = "TCP_send_data";
          response_Value["Status"] = (Status == 0 ? "OK" : "EROR");

          USB_Convert_And_Send_Data(response_Value.ToString(), (uint)response_Value.ToString().Length);
        }
        else if (Cmd == "TCP_receive_data")
        {
          string Data_Receive;
          Data_Receive = tcpCmd.receiveData();

          JObject response_Value = new JObject();
          response_Value["Data"] = "TCP_receive_data";
          response_Value["Status"] = (Data_Receive.Length != 0 ? "OK" : "EROR");
          JObject Value_Obj = new JObject();
          Value_Obj["Data"] = Data_Receive;
          Value_Obj["Len"]  = Data_Receive.Length;
          response_Value["Value"] = Value_Obj;

          USB_Convert_And_Send_Data(response_Value.ToString(), (uint)response_Value.ToString().Length);
        }
        else if (Cmd == "TCP_close_connection")
        {
          Status = tcpCmd.closeConnection();

          JObject response_Value = new JObject();
          response_Value["Data"] = "TCP_close_connection";
          response_Value["Status"] = (Status == 0 ? "OK" : "EROR");

          USB_Convert_And_Send_Data(response_Value.ToString(), (uint)response_Value.ToString().Length);
        }
      }
    }

    private void USB_Update_To_Evt(char[] Data, uint Len)
    {
      USB_Evt_T tEvt = new USB_Evt_T(new char[Len]);
      Array.Copy(Data, tEvt.Data, Len);

      cBuffer_Process_Data.Put(tEvt);

      if (!threadProcData.IsBusy)
      {
        threadProcData.RunWorkerAsync();
      }
    }

    private void USB_Convert_And_Send_Data(string Data, uint Len)
    {
      byte[] tPacket = new byte[Len + 4];

      // Add header
      tPacket[0] = 0x55;
      // Add Paylioad size
      tPacket[1] = (byte)((Len >> 8) & 0xFF);
      tPacket[2] = (byte)((Len) & 0xFF);
      // Add Paylioad data
      Array.Copy(Encoding.ASCII.GetBytes(Data), 0 , tPacket, 3, Len);
       // Calculate and add CRC to packet
      CRC8Calc CRC = new CRC8Calc(CRC8_POLY.CRC8_CCITT);
      byte[] tPacket_Byte_Convert = Encoding.ASCII.GetBytes(Data);
      byte CRC_Value = CRC.Checksum(tPacket_Byte_Convert);
      tPacket[Len + 3] = CRC_Value;

      serialPort.Write(tPacket, 0, (int)(Len + 4));

    }

    private void Form1_Load(object sender, EventArgs e)
    {
        comBox.DataSource = SerialPort.GetPortNames();
        baudRateBox.SelectedIndex = 7;
    }

    private void OpenBtn_Click(object sender, EventArgs e)
    {
        if (!serialPort.IsOpen)
        {
          serialPort.PortName = comBox.Text;
          serialPort.BaudRate = Convert.ToInt32(baudRateBox.Text);

          serialPort.Open();

          JObject response_Value = new JObject();
          response_Value["Data"] = "USB_Handshake";
          response_Value["Status"] = "OK";
          USB_Convert_And_Send_Data(response_Value.ToString(), (uint)response_Value.ToString().Length);
        }
        else
        {
            serialPort.Close();
        }
    }

    private void CloseBtn_Click(object sender, EventArgs e)
    {
        serialPort.Close();
    }

    private void Data_Receive(object obj, SerialDataReceivedEventArgs e)
    {
      //inputData = serialPort.ReadExisting();
      int tLen = serialPort.BytesToRead;
      byte[] inputData = new byte[tLen];
      serialPort.Read(inputData, 0, tLen);
        
      // Write to circular buffer
      cBuffer.Put(inputData);

      if (!threadRecData.IsBusy)
      {
        threadRecData.RunWorkerAsync();
      }
        

      // if (inputData != String.Empty)
      // {
      //     // 
      //     // Test data
      //     Set_Text(inputData);

      // }
    }

    private void Set_Text(string text)
    {
      if (this.viewBox.InvokeRequired)
      {
        Set_Text_CB d = new Set_Text_CB(Set_Text);

        this.Invoke(d, new object[] { text });
      }
      else
      {
        viewBox.Text += text;
      }
    }

    private void Timer_Tick(object sender, EventArgs e)
    {
        
      //comBox.DataSource = SerialPort.GetPortNames();
      if (!serialPort.IsOpen)
      {
          viewStatusLabel.Text = ("Disconnected");
          viewStatusLabel.ForeColor = Color.Red;
      }
      else
      {
          viewStatusLabel.Text = ("Connected");
          viewStatusLabel.ForeColor = Color.Green;
      }
    }
  }
}
