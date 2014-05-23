using System;
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
    }
}
