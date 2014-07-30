using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Sce.PlayStation4
{
    namespace System
    {
        public delegate void UserEventCallback(int userId, int playerIndex);

        public unsafe static partial class UserService
        {
            static UserService()
            {
                _SetLoginCallback(LoginEvent);
                _SetLogoutCallback(LogoutEvent);
            }

            [MonoPInvokeCallback(typeof(UserService._UserCallback))]
            static private void LoginEvent(int userId, int playerIndex)
            {
                if (OnLogin != null)
                    OnLogin(userId, playerIndex);
            }

            [MonoPInvokeCallback(typeof(UserService._UserCallback))]
            static private void LogoutEvent(int userId, int playerIndex)
            {
                if (OnLogout != null)
                    OnLogout(userId, playerIndex);
            }

            static public event UserEventCallback OnLogin;

            static public event UserEventCallback OnLogout;
        }
    }
}
