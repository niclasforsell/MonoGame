using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace Sce.PlayStation4
{
    namespace Network
    {
        public unsafe partial class NpScoreRankings : IDisposable
        {
            public void GetGameInfo(byte[] data)
            {
                var temp = BinaryGameInfoIndex;
                for (int i = 0; i < data.Length && i < 189; ++i)
                {
                    data[i] = temp[i];
                }
            }
        }

        public unsafe partial class NpScoreRequest : IDisposable
        {
            public Network.NpCommunityError RecordScore(uint boardId, long score, string scoreComment, byte[] gameInfo, ulong compareDate, out uint tmpRank)
            {
                fixed (byte* pGameInfo = gameInfo)
                    return RecordScore(boardId, score, scoreComment, pGameInfo, gameInfo.Length, compareDate, out tmpRank);
            }
        }
    }
}
