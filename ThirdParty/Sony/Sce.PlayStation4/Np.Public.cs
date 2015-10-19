﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace Sce.PlayStation4.Network
{
    public partial class Np
    {
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
    }
}
