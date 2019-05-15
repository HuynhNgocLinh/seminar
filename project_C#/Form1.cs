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


namespace Monitor
{
  public enum USB_State_T
  {
    STATE_GET_HEADER,
    STATE_GET_TYPE,
    STATE_GET_CMD,
    STATE_GET_PAYLOAD_SIZE,
    STATE_GET_PAYLOAD_DATA,
  }


  public struct USB_Packet_T
  {
    public uint   Header;
    public uint   Type;
    public uint   Cmd;
    public uint   Payload_Size;

    public char[] Payload_Data;
    
    public USB_Packet_T(uint Header, uint Type, uint Cmd, uint Payload_Size, char[] Payload_Data)
    {
      this.Header       = Header;
      this.Type         = Type;
      this.Cmd          = Cmd;
      this.Payload_Size = Payload_Size;
      this.Payload_Data = Payload_Data;
    }
  } 

  public partial class Form1 : Form
  {
    string inputData = string.Empty;
    delegate void Set_Text_CB(string text);
    
    // Circular buffer
    static CircularBuffer<char> cBuffer;

    // Thread
    Thread mThread;
    
    public Form1()
    {
      InitializeComponent();

      serialPort.DataReceived += new SerialDataReceivedEventHandler(Data_Receive);
      cBuffer = new CircularBuffer<char>(1000, false);

      // Create thread
      mThread = new Thread(USB_Process_Data);

      // Start thread
      mThread.Start();

      string[] baudRate = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

      baudRateBox.Items.AddRange(baudRate);

      //string[] COM = { "COM1", "COM2", "COM3", "COM4", "COM5" };

      //comBox.Items.AddRange(COM);
    }

    public static void USB_Process_Data()
    {
      USB_State_T usbState = USB_State_T.STATE_GET_HEADER;
      USB_Packet_T usbPacket = new USB_Packet_T(0, 0, 0, 0, new char[1000]);
      for ( ; ; )
      {
        if (!cBuffer.IsEmpty)
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
              if (usbPacket.Type <= 0x02)
              {
                usbState = USB_State_T.STATE_GET_CMD;
              }
              else
              {
                usbState = USB_State_T.STATE_GET_HEADER;
              }

              break;

            case USB_State_T.STATE_GET_CMD:
              usbPacket.Cmd = Convert.ToByte(cBuffer.Get());

              // Check Cmd
              if (usbPacket.Cmd <= 0x04)
              {
                usbState = usbState = USB_State_T.STATE_GET_PAYLOAD_SIZE;
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

                usbPacket.Payload_Size =  (uint)((t_Data[0] - '0') << 8) | t_Data[1];

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

              usbPacket.Payload_Data = cBuffer.Get((int)usbPacket.Payload_Size);
              if (cBuffer.Size >= usbPacket.Payload_Size)
              {
                // usbPacket.Payload_Data = Array.ConvertAll(cBuffer.Get((int)usbPacket.Payload_Size), char.Parse);
                
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

        //temp = serialPort.ReadByte();
        if (inputData != String.Empty)
        {
            // 
            // Test data
            Set_Text(inputData);

        }
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
            this.viewBox.Text += text;
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
