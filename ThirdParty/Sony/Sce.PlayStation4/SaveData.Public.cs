using System;
using System.IO;

namespace Sce.PlayStation4.System
{
    public partial class SaveData
    {
        /// <summary>
        /// Set the save data icon from a buffer containing a png image.
        /// </summary>
        public unsafe SaveDataResult SaveIcon(byte[] buffer)
        {
            fixed(void *ptr = buffer)
                return _SaveIcon(ptr, (ulong)buffer.Length);
        }

        /// <summary>
        /// Set the save game icon from the app0 path to a png image.
        /// </summary>
        public unsafe SaveDataResult SaveIcon(string pathToSaveDataPng)
        {
            var bytes = File.ReadAllBytes(pathToSaveDataPng);
            fixed (void* ptr = bytes)
                return _SaveIcon(ptr, (ulong)bytes.Length);
        }
    }
}
