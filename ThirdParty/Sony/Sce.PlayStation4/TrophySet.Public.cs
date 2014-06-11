using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Sce.PlayStation4.Network
{
    public partial class TrophySet
    {
        public TrophySet(int userId, uint serviceLabel = 0)
            : this()
        {
            var result = this.CreateContext(userId, serviceLabel);
            if (result != TrophyResult.Ok)
                throw new Exception(result.ToString());
        }

        public bool IsUnlocked(int trophyId)
        {
            bool isUnlocked;
            var result = IsUnlocked(trophyId, out isUnlocked);
            if (result != TrophyResult.Ok)
                throw new Exception(result.ToString());

            return isUnlocked;
        }

        public void Unlock(int trophyId)
        {
            bool gotPlatinum;
            var result = Unlock(trophyId, out gotPlatinum);
            if (result != TrophyResult.Ok && result != TrophyResult.ErrorTrophyAlreadyUnlocked)
                throw new Exception(result.ToString());
        }
    }
}
