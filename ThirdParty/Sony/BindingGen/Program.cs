using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CppSharp;
using CppSharp.AST;
using CppSharp.Passes;

namespace BindingGen
{
    static class Program
    {
        public static void Main(string[] args)
        {
            ConsoleDriver.Run(new ScePlayStation4());
        }
    }
}
