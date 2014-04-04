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
            var state = PSGamePad.GetState((int)playerIndex);
            return new GamePadCapabilities
            {
                IsConnected = state.IsConnected,
                HasAButton = true,
                HasBackButton = false, // No equivalent for PS4 as far as I can tell
                HasBButton = true,
                HasDPadDownButton = true,
                HasDPadLeftButton = true,
                HasDPadRightButton = true,
                HasDPadUpButton = true,
                HasLeftShoulderButton = true,
                HasLeftStickButton = true,
                HasRightShoulderButton = true,
                HasRightStickButton = true,
                HasStartButton = true, // TODO: Verify this maps semantics with Options button
                HasXButton = true,
                HasYButton = true,
                HasBigButton = false,
                HasLeftXThumbStick = true,
                HasLeftYThumbStick = true,
                HasRightXThumbStick = true,
                HasRightYThumbStick = true,
                HasLeftTrigger = true,
                HasRightTrigger = true,
                HasLeftVibrationMotor = true,
                HasRightVibrationMotor = true,
                HasVoiceSupport = true,
                GamePadType = GamePadType.GamePad
            };
        }

        public static GamePadState GetState(PlayerIndex playerIndex)
        {
            return GetState(playerIndex, GamePadDeadZone.None);
        }

        public static GamePadState GetState(PlayerIndex playerIndex, GamePadDeadZone deadZoneMode)
        {
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
                Orientation = new Quaternion
                {
                    X = state.OrientationX,
                    Y = state.OrientationY,
                    Z = state.OrientationZ,
                    W = state.OrientationW
                },
                Acceleration = new Vector3
                {
                    X = state.AccelerationX,
                    Y = state.AccelerationY,
                    Z = state.AccelerationZ
                },
                AngularVelocity = new Vector3
                {
                    X = state.AngularVelocityX,
                    Y = state.AngularVelocityY,
                    Z = state.AngularVelocityZ
                },
            };
        }

        public static bool SetVibration(PlayerIndex playerIndex, float leftMotor, float rightMotor)
        {
            return PSGamePad.SetVibration((int)playerIndex, leftMotor, rightMotor);
        }

        public static bool SetLightBar(PlayerIndex playerIndex, Color color)
        {
            return PSGamePad.SetLightBar((int)playerIndex, color.R, color.G, color.B);
        }

        public static bool ResetLightBar(PlayerIndex playerIndex)
        {
            return PSGamePad.ResetLightBar((int)playerIndex);
        }

        public static bool SetMotionEnabled(PlayerIndex playerIndex, bool value)
        {
            return PSGamePad.SetMotionEnabled((int)playerIndex, value);
        }

        public static bool SetVelocityDeadbandEnabled(PlayerIndex playerIndex, bool value)
        {
            return PSGamePad.SetVelocityDeadbandEnabled((int)playerIndex, value);
        }

        public static bool SetTiltCorrectionEnabled(PlayerIndex playerIndex, bool value)
        {
            return PSGamePad.SetTiltCorrectionEnabled((int)playerIndex, value);
        }

        public static bool ResetOrientation(PlayerIndex playerIndex)
        {
            return PSGamePad.ResetOrientation((int)playerIndex);
        }
    }
}
