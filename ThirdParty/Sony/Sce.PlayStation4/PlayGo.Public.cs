using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Sce.PlayStation4.System
{
    partial class ChunkList
    {
        public ChunkList(IEnumerable<ushort> items)
        {
            foreach (var item in items)
                Add(item);
        }

        public void Add(ushort value)
        {
            if (!TryAdd(value))
                throw new InvalidOperationException();
        }
    }

    partial class LociResult : IEnumerable<PlayGoLocus>
    {
        public PlayGoLocus this[int index]
        {
            get
            {
                sbyte value;
                if (!TryGet(index, out value))
                    throw new IndexOutOfRangeException();

                return (PlayGoLocus)value;
            }
        }

        public IEnumerator<PlayGoLocus> GetEnumerator()
        {
            var size = this.Count;
            for (var i = 0; i < size; i++)
                yield return this[i];
        }

        global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }
    }

    partial class ToDoList : IEnumerable<ToDoList.Entry>
    {
        public struct Entry
        {
            public ushort ChunkId;
            public PlayGoLocus Locus;
        }

        public ToDoList(IEnumerable<Entry> items)
        {
            foreach (var pair in items)
                Add(pair.ChunkId, pair.Locus);
        }

        public Entry this[int index]
        {
            get
            {
                ushort chunkId;
                sbyte locus;
                if (!TryGet(index, out chunkId, out locus))
                    throw new IndexOutOfRangeException();

                return new Entry
                {
                    ChunkId = chunkId,
                    Locus = (PlayGoLocus)locus
                };
            }
        }

        public IEnumerator<Entry> GetEnumerator()
        {
            var size = this.Count;
            for (var i = 0; i < size; i++)
                yield return this[i];
        }

        global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }

        public void Add(ushort chunkId, PlayGoLocus locus)
        {
            if (!TryAdd(chunkId, (sbyte)locus))
                throw new InvalidOperationException();
        }
    }

    partial class ProgressResult : IEnumerable<ProgressResult.Entry>
    {
        public struct Entry
        {
            public ulong ProgressSize;
            public ulong TotalSize;
        }

        public Entry this[int index]
        {
            get
            {
                // TODO: Switch these to ulong after 64-bit fix
                uint progressSize;
                uint totalSize;
                if (!TryGet(index, out progressSize, out totalSize))
                    throw new IndexOutOfRangeException();

                return new Entry
                {
                    ProgressSize = progressSize,
                    TotalSize = totalSize
                };
            }
        }

        public IEnumerator<Entry> GetEnumerator()
        {
            var size = this.Count;
            for (var i = 0; i < size; i++)
                yield return this[i];
        }

        global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }
    }
}
