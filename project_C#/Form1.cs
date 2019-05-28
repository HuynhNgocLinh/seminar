﻿using System;
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

namespace Monitor
{
  public enum USB_State_T
  {
    STATE_GET_HEADER,
    STATE_GET_TYPE,
    STATE_GET_PAYLOAD_SIZE,
    STATE_GET_PAYLOAD_DATA,
  }

  public enum USB_Type_T
  {
    USB_TYPE_CHECK_CONNECTION,
    USB_TYPE_ESTABLISH_TCP_CONNECTION,
    USB_TYPE_SEND_DATA_TO_TCP_SERVER,
    USB_TYPE_REC_DATA_FROM_TCP_SERVER,
    USB_TYPE_CLOSE_TCP_CONNECTION,
    USB_TYPE_MAX
  }


  public struct USB_Packet_T
  {
    public uint   Header;
    public uint   Type;
    public uint   Payload_Size;

    public char[] Payload_Data;
    
    public USB_Packet_T(uint Header, uint Type, uint Payload_Size, char[] Payload_Data)
    {
      this.Header       = Header;
      this.Type         = Type;
      this.Payload_Size = Payload_Size;
      this.Payload_Data = Payload_Data;
    }
  }

  public enum USB_Evt_Type_T
  {
    USB_EVT_TYPE_CHECK_STATUS_CONNECTION,
    USB_EVT_TYPE_ESTABLISH_TCP_CONNECTION,
    USB_EVT_TYPE_SEND_DATA_TO_TCP_SERVER,
    USB_EVT_TYPE_REC_DATA_FROM_TCP_SERVER,
    USB_EVT_TYPE_CLOSE_TCP_CONNECTION,
    USB_EVT_TYPE_MAX
  }

  public struct USB_Evt_T
  {
    public USB_Evt_Type_T Type;
    public char[]         Data;

    public USB_Evt_T(USB_Evt_Type_T Type, char[] Data)
    {
      this.Type = Type;
      this.Data = Data;
    }
  }

  

  public partial class Form1 : Form
  {
    string inputData = string.Empty;
    delegate void Set_Text_CB(string text);
    
    // Circular buffer
    static CircularBuffer<char> cBuffer;

    static CircularBuffer<USB_Evt_T> cBuffer_Process_Data;

    USB_State_T usbState = USB_State_T.STATE_GET_HEADER;
    USB_Packet_T usbPacket = new USB_Packet_T(0, 0, 0, new char[1000]);


    public char[] tArr;
    public string tChar;

    public int countEvt = 0;

    static TCP_IP_Cmd tcpCmd;
    // Thread
    //Thread mThread;

    // Thread threadProcess_Data;

    public Form1()
    {
      InitializeComponent();

      serialPort.DataReceived += new SerialDataReceivedEventHandler(Data_Receive);
      cBuffer                  = new CircularBuffer<char>(1000, false);
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
                usbState = USB_State_T.STATE_GET_TYPE;
              }
              else
              {
                usbState = USB_State_T.STATE_GET_HEADER;
              }

              break;
            case USB_State_T.STATE_GET_TYPE:
              usbPacket.Type = Convert.ToByte(cBuffer.Get());

              // Check type
              if (usbPacket.Type <= 0x04)
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
                char[] t_Data = cBuffer.Get(2);

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
                tArr = new char[20];
                tChar = "OK";
                usbPacket.Payload_Data = cBuffer.Get((int)usbPacket.Payload_Size);
                // usbPacket.Payload_Data = Array.ConvertAll(cBuffer.Get((int)usbPacket.Payload_Size), char.Parse);
                switch(usbPacket.Type)
                {
                  case 0x00:
                    USB_Update_To_Evt(USB_Evt_Type_T.USB_EVT_TYPE_CHECK_STATUS_CONNECTION, 
                                      usbPacket.Payload_Data, 
                                      usbPacket.Payload_Size);
                   
                  break;

                  case 0x01:
                    USB_Update_To_Evt(USB_Evt_Type_T.USB_EVT_TYPE_ESTABLISH_TCP_CONNECTION, 
                                      usbPacket.Payload_Data, 
                                      usbPacket.Payload_Size);
                    
                  break;

                  case 0x02:
                    USB_Update_To_Evt(USB_Evt_Type_T.USB_EVT_TYPE_SEND_DATA_TO_TCP_SERVER, 
                                      usbPacket.Payload_Data, 
                                      usbPacket.Payload_Size);

                  break;
                  
                  case 0x03:
                    USB_Update_To_Evt(USB_Evt_Type_T.USB_EVT_TYPE_REC_DATA_FROM_TCP_SERVER, 
                                      usbPacket.Payload_Data, 
                                      usbPacket.Payload_Size);
                    
                  break;

                  case 0x04:
                    USB_Update_To_Evt(USB_Evt_Type_T.USB_EVT_TYPE_CLOSE_TCP_CONNECTION, 
                                      usbPacket.Payload_Data, 
                                      usbPacket.Payload_Size);

                  break;
                
                  default:break;
                }
                
                // Clear packet data
                usbPacket.Header = 0;
                usbPacket.Type = 0;
                usbPacket.Payload_Size = 0;
                Array.Clear(usbPacket.Payload_Data, 0, usbPacket.Payload_Data.Length);

                // Change to STATE_GET_HEADER
                usbState = USB_State_T.STATE_GET_HEADER;

              }
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
        USB_Evt_T tUSB = new USB_Evt_T(0, new char[1000]);
        tUSB = cBuffer_Process_Data.Get();
        TCP_Base_Type.TCP_IP_Status_T Status;
        switch (tUSB.Type)
        {
          case USB_Evt_Type_T.USB_EVT_TYPE_CHECK_STATUS_CONNECTION:

            tArr[0] = (char)0x55;
            tArr[1] = (char)0x01;
            tArr[2] = (char)tUSB.Type;
            tArr[3] = (char)0x03;
            tArr[4] = (char)0x04;
            Array.Copy(tUSB.Data, 0, tArr, 5, 3);

            serialPort.Write(tArr, 0, 8);
              //Console.WriteLine("USB_EVT_TYPE_CHECK_STATUS_CONNECTION");
            break;
          case USB_Evt_Type_T.USB_EVT_TYPE_ESTABLISH_TCP_CONNECTION:
            //tArr[0] = (char)0x55;
            //tArr[1] = (char)0x01;
            //tArr[2] = (char)tUSB.Type;
            //tArr[3] = (char)0x03;
            //tArr[4] = (char)0x04;
            //Array.Copy(tUSB.Data, 0, tArr, 5, 3);

            //serialPort.Write(tArr, 0, 8);
            
            char[] tIP_Addr = new char[9];
            Array.Copy(tUSB.Data, tIP_Addr, 9);
            string IP_Addr = new string(tIP_Addr);

            char[] tPort = new char[4];
            Array.Copy(tUSB.Data, 15, tPort, 0, 4);

            int Port;
            Port = (tPort[0] - '0') * 1000 + (tPort[1] - '0') * 100 + (tPort[2] - '0') * 10 + (tPort[3] - '0');
            

            Status = tcpCmd.establishConnetion(IP_Addr, Port);

            tArr[0] = (char)0x55;
            tArr[1] = (char)tUSB.Type;
            tArr[2] = (char)0x00;
            tArr[3] = (char)0x01;
            tArr[4] = (char)Status;
            serialPort.Write(tArr, 0, 5);


            break;
          case USB_Evt_Type_T.USB_EVT_TYPE_SEND_DATA_TO_TCP_SERVER:
            //tArr[0] = (char)0x55;
            //tArr[1] = (char)0x01;
            //tArr[2] = (char)tUSB.Type;
            //tArr[3] = (char)0x03;
            //tArr[4] = (char)0x04;
            //Array.Copy(tUSB.Data, 0, tArr, 5, 3);

            //serialPort.Write(tArr, 0, 8);

            char[] tData_Send = new char[100];
            Array.Copy(tUSB.Data, tData_Send, 4);

            string Data_send = new string(tData_Send);

            Status = tcpCmd.sendData(Data_send);

            tArr[0] = (char)0x55;
            tArr[1] = (char)tUSB.Type;
            tArr[2] = (char)0x00;
            tArr[3] = (char)0x01;
            tArr[4] = (char)Status;
            serialPort.Write(tArr, 0, 5);

            //Console.WriteLine("USB_EVT_TYPE_SEND_DATA_TO_TCP_SERVER");
            break;
          case USB_Evt_Type_T.USB_EVT_TYPE_REC_DATA_FROM_TCP_SERVER:
            //tArr[0] = (char)0x55;
            //tArr[1] = (char)0x01;
            //tArr[2] = (char)tUSB.Type;
            //tArr[3] = (char)0x03;
            //tArr[4] = (char)0x04;
            //Array.Copy(tUSB.Data, 0, tArr, 5, 3);

            //serialPort.Write(tArr, 0, 8);
            string Rec_Data;

            Rec_Data = tcpCmd.receiveData();
            tArr[0] = (char)0x55;
            tArr[1] = (char)tUSB.Type;
            tArr[2] = (char)0x00;
            tArr[3] = (char)0x0A;
            Array.Copy(Rec_Data.ToCharArray(), 0, tArr, 4, 10);
            serialPort.Write(tArr, 0, 14);

            //Console.WriteLine("USB_EVT_TYPE_REC_DATA_FROM_TCP_SERVER");
            break;
          case USB_Evt_Type_T.USB_EVT_TYPE_CLOSE_TCP_CONNECTION:
            tArr[0] = (char)0x55;
            tArr[2] = (char)tUSB.Type;
            tArr[3] = (char)0x03;
            tArr[4] = (char)0x04;
            Array.Copy(tUSB.Data, 0, tArr, 5, 3);

            serialPort.Write(tArr, 0, 8);
              //Console.WriteLine("USB_EVT_TYPE_CLOSE_TCP_CONNECTION");
              break;
          default: break;
        }
      }
    }

    private void USB_Update_To_Evt(USB_Evt_Type_T Type, char[] Data, uint Len)
    {
      USB_Evt_T tEvt = new USB_Evt_T(0, new char[1000]);
      tEvt.Type = Type;
      Array.Copy(Data, tEvt.Data, Len);

      cBuffer_Process_Data.Put(tEvt);

      if (!threadProcData.IsBusy)
      {
        threadProcData.RunWorkerAsync();
      }
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
      inputData = serialPort.ReadExisting();
        
      // Write to circular buffer
      cBuffer.Put(inputData.ToCharArray());

      int tLen = cBuffer.Size;
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
