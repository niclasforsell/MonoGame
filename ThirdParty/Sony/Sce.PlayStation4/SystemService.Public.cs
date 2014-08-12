using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Sce.PlayStation4
{
    namespace System
    {
        public delegate void SystemServiceEventCallback(SystemServiceEvent eventType);

        public unsafe static partial class SystemService
        {
            /// <summary>
            /// Called from your update loop to fire off any pending system service events.
            /// </summary>
            public static void FireEvents()
            {
                var count = NumRecieveEvents;
                while (count-- > 0)
                {
                    SystemServiceEvent eventType;
                    if (ReceiveEvent(out eventType))
                    {
                        if (OnEvent != null)
                            OnEvent(eventType);
                    }
                }
            }

            /// <summary>
            /// Triggered when a system service event occurs.
            /// </summary>
            static public event SystemServiceEventCallback OnEvent;
        }
    }
}
