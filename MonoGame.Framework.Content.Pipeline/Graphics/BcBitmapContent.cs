// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using Microsoft.Xna.Framework.Graphics;
using System;

namespace Microsoft.Xna.Framework.Content.Pipeline.Graphics
{
    public class BcBitmapContent : BitmapContent
    {
        private SurfaceFormat _format;
        private byte[] _data;

        public BcBitmapContent(int width, int height, SurfaceFormat format)
            : base(width, height)
        {
            _format = format;
        }


        public override byte[] GetPixelData()
        {
            return _data;
        }

        public override void SetPixelData(byte[] sourceData)
        {
            _data = sourceData;
        }

        protected override bool TryCopyFrom(BitmapContent sourceBitmap, Rectangle sourceRegion, Rectangle destinationRegion)
        {
            throw new NotImplementedException();
        }

        protected override bool TryCopyTo(BitmapContent destinationBitmap, Rectangle sourceRegion, Rectangle destinationRegion)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the corresponding GPU texture format for the specified bitmap type.
        /// </summary>
        /// <param name="format">Format being retrieved.</param>
        /// <returns>The GPU texture format of the bitmap type.</returns>
        public override bool TryGetFormat(out SurfaceFormat format)
        {
            format = _format;
            return true;
        }
    }
}
