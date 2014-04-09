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
            //ConsoleDriver.Run(new ScePlayStation4Tools());

            // HACK: It would be nice if we could just disable the compile
            // step instead of doing hardcoded cleanup here.
            try { File.Delete("Sce.PlayStation4.dll"); } catch {}
            try { File.Delete("Sce.PlayStation4.pdb"); } catch {}
            try { File.Delete("Sce.PlayStation4.xml"); } catch {}
            try { File.Delete("Sce.PlayStation4.Tools.Texture.dll"); } catch {}
            try { File.Delete("Sce.PlayStation4.Tools.Texture.pdb"); } catch {}
            try { File.Delete("Sce.PlayStation4.Tools.Texture.xml"); } catch {}
        }
    }
}
