// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Microsoft.Xna.Framework.Graphics
{
	public partial class Texture3D : Texture
	{
        private int width;
        private int height;
        private int depth;

        public int Width
        {
            get { return width; }
        }

        public int Height
        {
            get { return height; }
        }

        public int Depth
        {
            get { return depth; }
        }

		public Texture3D(GraphicsDevice graphicsDevice, int width, int height, int depth, bool mipMap, SurfaceFormat format)
            : this(graphicsDevice, width, height, depth, mipMap, format, false)
		{		    
		}

		protected Texture3D (GraphicsDevice graphicsDevice, int width, int height, int depth, bool mipMap, SurfaceFormat format, bool renderTarget)
		{
            if (graphicsDevice == null)
                throw new ArgumentNullException("graphicsDevice");

			this.GraphicsDevice = graphicsDevice;
            this.width = width;
            this.height = height;
            this.depth = depth;
            this._levelCount = 1;
		    this._format = format;

            PlatformConstruct(graphicsDevice, width, height, depth, mipMap, format, renderTarget);
        }

        public void SetData<T>(T[] data) where T : struct
		{
			SetData<T>(data, 0, data.Length);
		}
		
		public void SetData<T> (T[] data, int startIndex, int elementCount) where T : struct
		{
			SetData<T>(0, 0, 0, Width, Height, 0, Depth, data, startIndex, elementCount);
		}

        public void SetData<T>(int level,
                                int left, int top, int right, int bottom, int front, int back,
                                T[] data, int startIndex, int elementCount) where T : struct
        {
            if (data == null || data.Length == 0)
                throw new ArgumentNullException("data");

            if ((data.Length - startIndex) < elementCount)
                throw new ArgumentException("Not enough data in the array given startIndex and elementCount.");

            // Disallow negative box size
            if ((left < 0 || left >= right)
                || (top < 0 || top >= bottom)
                || (front < 0 || front >= back))
                throw new ArgumentException("Neither box size nor box position can be negative");

            PlatformSetData<T>(level, left, top, right, bottom, front, back, data, startIndex, elementCount);
		}

        /// <summary>
        /// Gets a copy of 3D texture data, specifying a mipmap level, source box, start index, and number of elements.
        /// </summary>
        /// <typeparam name="T">The type of the elements in the array.</typeparam>
        /// <param name="level">Mipmap level.</param>
        /// <param name="left">Position of the left side of the box on the x-axis.</param>
        /// <param name="top">Position of the top of the box on the y-axis.</param>
        /// <param name="right">Position of the right side of the box on the x-axis.</param>
        /// <param name="bottom">Position of the bottom of the box on the y-axis.</param>
        /// <param name="front">Position of the front of the box on the z-axis.</param>
        /// <param name="back">Position of the back of the box on the z-axis.</param>
        /// <param name="data">Array of data.</param>
        /// <param name="startIndex">Index of the first element to get.</param>
        /// <param name="elementCount">Number of elements to get.</param>
        public void GetData<T>(int level, int left, int top, int right, int bottom, int front, int back, T[] data, int startIndex, int elementCount) where T : struct
        {
            if (data == null || data.Length == 0)
                throw new ArgumentException("data cannot be null");
            if (data.Length < startIndex + elementCount)
                throw new ArgumentException("The data passed has a length of " + data.Length + " but " + elementCount + " pixels have been requested.");

            // Disallow negative box size
            if ((left < 0 || left >= right)
                || (top < 0 || top >= bottom)
                || (front < 0 || front >= back))
                throw new ArgumentException("Neither box size nor box position can be negative");

            PlatformGetData(level, left, top, right, bottom, front, back, data, startIndex, elementCount);
        }

        /// <summary>
        /// Gets a copy of 3D texture data, specifying a start index and number of elements.
        /// </summary>
        /// <typeparam name="T">The type of the elements in the array.</typeparam>
        /// <param name="data">Array of data.</param>
        /// <param name="startIndex">Index of the first element to get.</param>
        /// <param name="elementCount">Number of elements to get.</param>
        public void GetData<T>(T[] data, int startIndex, int elementCount) where T : struct
        {
            GetData(0, 0, 0, width, height, 0, depth, data, startIndex, elementCount);
        }

        /// <summary>
        /// Gets a copy of 3D texture data.
        /// </summary>
        /// <typeparam name="T">The type of the elements in the array.</typeparam>
        /// <param name="data">Array of data.</param>
        public void GetData<T>(T[] data) where T : struct
        {
            GetData(data, 0, data.Length);
        }
	}
}

