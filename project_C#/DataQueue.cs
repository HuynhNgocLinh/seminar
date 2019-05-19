using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Monitor
{
    public class DataQueue<ITEM>
    {

        public DataQueue() { this.IsBlocking = true; }
        public DataQueue(bool blocking) { this.IsBlocking = blocking; }

        public EventWaitHandle WaitHandle { get { return FWaitHandle; } }

        public void Close()
        {
            this.FWaitHandle.Set();
        } //Close

        public void Submit(ITEM item)
        {
            lock (LockObject)
                Queue.Enqueue(item);
            FWaitHandle.Set();
        } //Submit

        public ITEM GetMessage()
        {
            if (IsBlocking)
                FWaitHandle.WaitOne();
            ITEM result = default(ITEM);
            lock (LockObject)
            {
                if (Queue.Count > 0)
                    result = Queue.Dequeue();
                if (IsBlocking && (Queue.Count < 1))
                    FWaitHandle.Reset();
                return result;
            } //lock
        } //GetMessage

        public int DataCount()
        {
            return Queue.Count;
        }

        public void Clear()
        {
            lock (LockObject)
                Clear(Queue);
        } //Clear
        public void Clear(Func<ITEM, bool> removalCriterion)
        {
            lock (LockObject)
                Clear(Queue, removalCriterion);
        } //Clear

        #region implementation

        static void Clear(Queue<ITEM> queue) { queue.Clear(); }
        static void Clear(Queue<ITEM> queue, Func<ITEM, bool> removalCriterion)
        {
            if (removalCriterion == null)
            {
                queue.Clear();
                return;
            } //if
            Queue<ITEM> copy = new Queue<ITEM>();
            while (queue.Count > 0)
            {
                ITEM item = queue.Dequeue();
                if (!removalCriterion(item))
                    copy.Enqueue(item);
            } //loop
            while (copy.Count > 0)
                queue.Enqueue(copy.Dequeue());
        } //Clear

        bool IsBlocking;
        Queue<ITEM> Queue = new Queue<ITEM>();

        //SA!!! important ManualReset.
        //See GetMessage for re-setting
        EventWaitHandle FWaitHandle = new EventWaitHandle(false, EventResetMode.ManualReset);
        Object LockObject = new object();

        #endregion implementation

    } //DataQueue
}
