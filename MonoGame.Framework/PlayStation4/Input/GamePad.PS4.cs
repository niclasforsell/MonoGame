// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PSGamePad = Sce.PlayStation4.Input.GamePad;
using PSGamePadState = Sce.PlayStation4.Input.GamePadState;

namespace Microsoft.Xna.Framework.Input
{
    public static partial class GamePad
    {
        public static GamePadCapabilities GetCapabilities(PlayerIndex playerIndex)
        {
            throw new NotImplementedException();
        }

        public static GamePadState GetState(PlayerIndex playerIndex)
        {
            return GetState(playerIndex, GamePadDeadZone.None);
        }

        public static GamePadState GetState(PlayerIndex playerIndex, GamePadDeadZone deadZoneMode)
        {
            // Make sure the player index is in range.
            var index = (int)playerIndex;
            if (index < (int)PlayerIndex.One || index > (int)PlayerIndex.Four)
                throw new InvalidOperationException();

            var state = PSGamePad.GetState((int)playerIndex);
            return new GamePadState
            {
                IsConnected = state.IsConnected1,
                PacketNumber = state.PacketNumber1,
                Buttons = new GamePadButtons((Buttons)state.Buttons1),
                DPad = new GamePadDPad((Buttons)state.Buttons1),
                ThumbSticks = new GamePadThumbSticks
                {
                    Left = new Vector2(state.LeftStickX1, state.LeftStickY1),
                    Right = new Vector2(state.RightStickX1, state.RightStickY1),
                },
                Triggers = new GamePadTriggers(state.LeftTrigger1, state.RightTrigger1),
            };
        }

        public static bool SetVibration(PlayerIndex playerIndex, float leftMotor, float rightMotor)
        {
            throw new NotImplementedException();
        }
    }
}
