//----------------------------------------------------------------------------
// This is autogenerated code by CppSharp.
// Do not edit this file or all your changes will be lost after re-generation.
//----------------------------------------------------------------------------
using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Sce.PlayStation4
{
    namespace Graphics
    {
        public unsafe partial class VertexShader : IDisposable
        {
            [StructLayout(LayoutKind.Explicit, Size = 40)]
            public struct Internal
            {
                [FieldOffset(8)]
                internal global::System.IntPtr _shader;

                [FieldOffset(16)]
                internal void* _binary;

                [FieldOffset(24)]
                internal void* _fsMem;

                [FieldOffset(32)]
                internal uint _shaderModifier;

                [SuppressUnmanagedCodeSecurity]
                [DllImport("scePlayStation4.prx", CallingConvention = global::System.Runtime.InteropServices.CallingConvention.Cdecl,
                    EntryPoint="_ZN8Graphics12VertexShaderC2EPKv")]
                internal static extern void ctor_0(global::System.IntPtr instance, void* data);

                [SuppressUnmanagedCodeSecurity]
                [DllImport("scePlayStation4.prx", CallingConvention = global::System.Runtime.InteropServices.CallingConvention.Cdecl,
                    EntryPoint="_ZN8Graphics12VertexShaderC2ERKS0_")]
                internal static extern void ctor_1(global::System.IntPtr instance, global::System.IntPtr _0);

                [SuppressUnmanagedCodeSecurity]
                [DllImport("scePlayStation4.prx", CallingConvention = global::System.Runtime.InteropServices.CallingConvention.Cdecl,
                    EntryPoint="_ZN8Graphics12VertexShaderD2Ev")]
                internal static extern void dtor_0(global::System.IntPtr instance);
            }

            public global::System.IntPtr __Instance { get; protected set; }

            internal VertexShader(VertexShader.Internal* native)
                : this(new global::System.IntPtr(native))
            {
            }

            internal VertexShader(VertexShader.Internal native)
                : this(&native)
            {
            }

            public VertexShader(global::System.IntPtr native)
            {
                __Instance = native;
            }

            public VertexShader(void* data)
            {
                __Instance = Marshal.AllocHGlobal(40);
                var arg0 = data;
                Internal.ctor_0(__Instance, arg0);
            }

            public void Dispose()
            {
                Dispose(disposing: true);
                GC.SuppressFinalize(this);
            }

            protected virtual void Dispose(bool disposing)
            {
                Internal.dtor_0(__Instance);
                Marshal.FreeHGlobal(__Instance);
            }
        }
    }
}
