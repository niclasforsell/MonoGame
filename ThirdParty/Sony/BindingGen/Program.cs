﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CppSharp;
using CppSharp.AST;
using CppSharp.Passes;

namespace BindingGen
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
            options.WriteOnlyWhenChanged = true;

            options.Verbose = true;
            options.GenerateProperties = true;
            options.GenerateFunctionTemplates = true;
            //options.GenerateVirtualTables = true;

            options.GenerateLibraryNamespace = true;
            options.LibraryName = "Sce.PlayStation4";           
            options.OutputNamespace = "Sce.PlayStation4";
            options.SharedLibraryName = "scePlayStation4.prx";

            // The paths to search for headers.
            options.IncludeDirs.Add(@".\");

            // The headers to process.
            //
            // NOTE: The casing here will influence the final .cs file.
            //
            options.Headers.Add(@"Graphics\Texture.h");

            // Make sure any system headers needed is available.
            var orbisSDK = System.Environment.GetEnvironmentVariable("SCE_ORBIS_SDK_DIR");
            options.SystemIncludeDirs.Add(orbisSDK + @"\host_tools\lib\clang\include");
            options.SystemIncludeDirs.Add(orbisSDK + @"\target\include");
            options.SystemIncludeDirs.Add(orbisSDK + @"\target\include_common");

            // Output it to the C# wrapper project.
            options.OutputDir = @"..\Sce.PlayStation4";
        }

        public void SetupPasses(Driver driver)
        {
            // TODO: The GenerateInlinesCodePass is what creates those inlines.cpp/txt files 
            // in the output folder which we never use.  This gross hack disables them.
            driver.TranslationUnitPasses.Passes.Remove(
                driver.TranslationUnitPasses.FindPass<GenerateInlinesCodePass>());
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
