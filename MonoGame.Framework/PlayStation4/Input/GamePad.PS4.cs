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
                IsConnected = state.IsConnected,
                PacketNumber = state.PacketNumber,
                Buttons = new GamePadButtons((Buttons)state.Buttons),
                DPad = new GamePadDPad((Buttons)state.Buttons),
                ThumbSticks = new GamePadThumbSticks
                {
                    Left = new Vector2(state.LeftStickX, state.LeftStickY),
                    Right = new Vector2(state.RightStickX, state.RightStickY),
                },
                Triggers = new GamePadTriggers(state.LeftTrigger, state.RightTrigger),
            };
        }

        public static bool SetVibration(PlayerIndex playerIndex, float leftMotor, float rightMotor)
        {
            // Make sure the player index is in range.
            var index = (int)playerIndex;
            if (index < (int)PlayerIndex.One || index > (int)PlayerIndex.Four)
                throw new InvalidOperationException();

            return PSGamePad.SetVibration((int)playerIndex, leftMotor, rightMotor);
        }

        public static bool SetColor(PlayerIndex playerIndex, Color color)
        {
            // Make sure the player index is in range.
            var index = (int)playerIndex;
            if (index < (int)PlayerIndex.One || index > (int)PlayerIndex.Four)
                throw new InvalidOperationException();

            return PSGamePad.SetColor((int)playerIndex, color.R, color.G, color.B);
        }
    }
}
