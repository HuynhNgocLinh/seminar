using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace Monitor
{
  class TCP_IP_Cmd
  {
    private TcpClient client;
    private Stream stream;

    public TCP_Base_Type.TCP_IP_Info_T checkStatusConnection()
    {
      TCP_Base_Type.TCP_IP_Info_T mInfo;
      mInfo.ipAddr = ((IPEndPoint)client.Client.RemoteEndPoint).Address.ToString();
      mInfo.port = ((IPEndPoint)client.Client.RemoteEndPoint).Port;

      return mInfo;
    }

    public TCP_Base_Type.TCP_IP_Status_T establishConnetion(string ipAddr, int port)
    {
      try
      {
        client = new TcpClient();
        client.Connect(ipAddr, port);
        stream = client.GetStream();

        return TCP_Base_Type.TCP_IP_Status_T.TCP_IP_STATUS_OK;
      }
      catch (Exception ex)
      {
        //Console.WriteLine("Error: " + ex);
        return TCP_Base_Type.TCP_IP_Status_T.TCP_IP_STATUS_ERR;
      }

    }

    public TCP_Base_Type.TCP_IP_Status_T sendData(string data)
    {
      try
      {
        var writer = new StreamWriter(stream);
        writer.AutoFlush = true;
        // Send
        writer.WriteLine(data);
        return TCP_Base_Type.TCP_IP_Status_T.TCP_IP_STATUS_OK;
      }
      catch (Exception ex)
      {
        return TCP_Base_Type.TCP_IP_Status_T.TCP_IP_STATUS_ERR;
      }
    }

    public string receiveData()
    {
      var reader = new StreamReader(stream);

      return reader.ReadLine();
    }

    public TCP_Base_Type.TCP_IP_Status_T closeConnection()
    {
      try
      {
        stream.Close();
        client.Close();

        return TCP_Base_Type.TCP_IP_Status_T.TCP_IP_STATUS_OK;
      }
      catch (Exception ex)
      {
        return TCP_Base_Type.TCP_IP_Status_T.TCP_IP_STATUS_ERR;
      }
    }
  }
}
