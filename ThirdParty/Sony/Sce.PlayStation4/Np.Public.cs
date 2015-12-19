using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace Sce.PlayStation4.Network
{
    public enum NpState
    {
        Unknown = 0,
        SignedOut,
        SignedIn
    }

    public partial class Np
    {
        public static void SetNpTitleId(string titleId, string titleSecret)
        {
            var count = titleSecret.Length / 2;
            var titleBytes = new byte[count];
            for (var i = 0; i < count; i++)
            {
                var b = Convert.ToByte(titleSecret.Substring(i * 2, 2), 16);
                titleBytes[i] = b;
            }

            SetNpTitleId(titleId, titleBytes);
        }

        public static void SetNpTitleId(string titleId, byte[] titleSecret)
        {
            if (titleId.Length != 12)
                throw new ArgumentOutOfRangeException("titleId", "Title ID must be exactly 12 characters.");

            if (titleSecret.Length != 128)
                throw new ArgumentOutOfRangeException("titleSecret", "Title Secret must be exactly 128 bytes.");

            var secretHandle = GCHandle.Alloc(titleSecret, GCHandleType.Pinned);
            var secretPtr = (IntPtr)(secretHandle.AddrOfPinnedObject().ToInt64());
            NpResult result;
            unsafe
            {
                result = _SetNpTitleId(titleId, (byte*)secretPtr);
            }
            secretHandle.Free();

            if (result != NpResult.Ok)
                throw new Exception(result.ToString());
        }

        /// <summary>
        /// To be used by multiplayer games whilst the PS Plus feature is in use.
        /// For Realtime multiplayer, it needs to be called at least once per second - each frame is ok.
        /// </summary>
        public static void NotifyPlusFeature(int userId, NpPlusFeature feature)
        {
            var result = _NotifyPlusFeature(userId, feature);
            if (result != NpResult.Ok)
                throw new Exception(result.ToString());
        }

        /// <summary>
        /// Checks if the user is entitled to use PS Plus feature.
        /// WARNING: Only Async mode supported and may block for some time.
        /// </summary>
        public static bool CheckPlus(int userId, NpPlusFeature features)
        {
            bool checkResult;
            NpResult result;
            unsafe
            {
                result = _CheckPlus(userId, features, &checkResult);
            }

            if (result != NpResult.Ok)
                throw new Exception(result.ToString());

            return checkResult;
        }

        /// <summary>
        /// Sets age based content restriction.
        /// TODO: support per-region restrictions.
        /// </summary>
        /// <param name="defaultAgeRestriction">The strictest age restriction of all regions</param>
        public static void SetContentRestriction(int defaultAgeRestriction)
        {
            _SetContentRestriction(defaultAgeRestriction);
        }

        /// <summary>
        /// Returns true if it's OK to use network features based on
        /// currently set age restriction.
        /// https://ps4.scedev.net/resources/documents/TRC/1.5/TRC/R4109.html
        /// </summary>
        public static bool CheckNpAvailability(int userId)
        {
            NpResult result = _CheckNpAvailability(userId);

            if (result != NpResult.Ok &&
                result != NpResult.ErrorAgeRestriction)
            {
                throw new Exception(result.ToString());
            }

            return result == NpResult.Ok;
        }

        public struct ParentalControlInfo
        {
            public int Age;
            public bool ChatRestriction;
            public bool UgcRestriction;
        }

        public static ParentalControlInfo GetParentalControlInfo(int userId)
        {
            var controlInfo = new ParentalControlInfo();
            NpResult result;
            unsafe
            {
                result = _GetParentalControlInfo(userId, &controlInfo.Age, &controlInfo.ChatRestriction, &controlInfo.UgcRestriction);
            }

            if (result != NpResult.Ok)
                throw new Exception(result.ToString());

            return controlInfo;
        }

        public static NpState GetState(int userId)
        {
            int state = 0;
            unsafe
            {
                _GetState(userId, &state);
            }
            return (NpState)state;
        }
    }
}
