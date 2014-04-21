// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PSGamePad = Sce.PlayStation4.Input.GamePad;
using PSGamePadState = Sce.PlayStation4.Input.GamePadState;

namespace Microsoft.Xna.Framework.Input
{
    public static partial class GamePad
    {
        private static GamePadCapabilities PlatformGetCapabilities(int index)
        {
            var state = PSGamePad.GetState(index);
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

        private static GamePadState PlatformGetState(int index, GamePadDeadZone deadZoneMode)
        {
            var state = PSGamePad.GetState(index);
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
                TouchCount = state.TouchCount,
                TouchPoint1 = new GamePadTouch
                {
                    Position = new Vector2(state.Touch1X, state.Touch1Y),
                    Id = state.Touch1Id
                },
                TouchPoint2 = new GamePadTouch
                {
                    Position = new Vector2(state.Touch2X, state.Touch2Y),
                    Id = state.Touch2Id
                }
            };
        }

        private static bool PlatformSetVibration(int index, float leftMotor, float rightMotor)
        {
            return PSGamePad.SetVibration(index, leftMotor, rightMotor);
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
