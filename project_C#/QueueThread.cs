using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Monitor
{
    class ExecuteCommandThread
    {
        internal ExecuteCommandThread() { this.Thread = new Thread(Body); }
        internal void Start() { this.Thread.Start(); }
        internal void PostMessage(string message)
        {
            this.Queue.Submit(message);
        } //void PostMessage
        #region implementaion
        void Body()
        {
            try
            {
                while (true)
                {
                    if (Queue.DataCount() > 0)
                    {
                        string message = Queue.GetMessage();
                        //safe way to exit, not alway possible
                        //if (message == null)
                        //    break;
                        ProcessMessage(message);
                    }


                } //loop
            }
            catch (ThreadAbortException)
            {
                //Very important in case Abort is used
                //Provide last-will processing here
                //even if Abort is not a designed option
            }
            catch (Exception e)
            { //always needed
                Console.WriteLine("{0}: {1}", e.GetType().Name, e.Message);
            } //exception
        } //Body
        void ProcessMessage(string message)
        {
            Console.WriteLine("message Processed {0}", message);
        } //ProcessMessage
        Thread Thread;
        DataQueue<string> Queue = new DataQueue<string>();
        #endregion implementaion
    } //class QueuedThread
}
