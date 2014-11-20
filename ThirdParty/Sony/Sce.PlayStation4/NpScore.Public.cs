using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Sce.PlayStation4
{
    namespace Network
    {
        public unsafe partial class NpScoreRequest : IDisposable
        {
            public Network.NpCommunityError RecordScore(uint boardId, long score, string scoreComment, byte[] gameInfo, ulong compareDate, out uint tmpRank)
            {
                fixed (byte* pGameInfo = gameInfo)
                {
                    return RecordScore(boardId, score, scoreComment, pGameInfo, gameInfo.Length, compareDate, out tmpRank);
                }
            }
        }
    }
}
