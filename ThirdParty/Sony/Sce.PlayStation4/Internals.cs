using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Sce.PlayStation4
{
    [AttributeUsage(AttributeTargets.Method)]
    sealed class MonoPInvokeCallbackAttribute : Attribute
    {
        public MonoPInvokeCallbackAttribute(Type t) { }
    }
}
