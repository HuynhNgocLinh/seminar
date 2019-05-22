using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Monitor
{
  public static class TCP_Base_Type
  {
    public enum TCP_IP_Status_T
    {
      TCP_IP_STATUS_OK,
      TCP_IP_STATUS_ERR,
    }

    public struct TCP_IP_Info_T
    {
      public string ipAddr;
      public int port;
    }
  }
}
