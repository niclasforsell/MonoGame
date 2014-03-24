using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CppSharp.AST;
using CppSharp.Passes;

namespace GenBindings
{
    class Library : ILibrary
    {
        public void Setup(Driver driver)
        {
            var options = driver.Options;

            options.Abi = CppAbi.Itanium;
            options.TargetTriple = "x86_64"; //-pc-freebsd";      
            options.MicrosoftMode = false;
            options.NoBuiltinIncludes = true;

            options.Verbose = true;
            options.GenerateProperties = true;
            options.GenerateFunctionTemplates = true;
            //options.GenerateVirtualTables = true;

            options.GenerateLibraryNamespace = true;
            options.LibraryName = "Sce.PlayStation4";           
            options.OutputNamespace = "Sce.PlayStation4";
            options.SharedLibraryName = "scePlayStation4.prx";

            options.IncludeDirs.Add(@"..\");
            //options.Headers.Add("MonoGame.Native.h");
            //options.Libraries.Add(@"..\bin\Debug\MonoGame.Native_stub.a");

            var orbisSDK = System.Environment.GetEnvironmentVariable("SCE_ORBIS_SDK_DIR");
            options.SystemIncludeDirs.Add(orbisSDK + @"\host_tools\lib\clang\include");
            options.SystemIncludeDirs.Add(orbisSDK + @"\target\include");
            options.SystemIncludeDirs.Add(orbisSDK + @"\target\include_common");

            options.OutputDir = @"..\..\Sce.PlayStation4";
        }

        public void SetupPasses(Driver driver)
        {
        }

        public void Preprocess(Driver driver, ASTContext lib)
        {
        }

        public void Postprocess(Driver driver, ASTContext lib)
        {
        }

        static class Program
        {
            public static void Main(string[] args)
            {
                ConsoleDriver.Run(new Library());
            }
        }
    }
}
