using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CppSharp;
using CppSharp.AST;
using CppSharp.Passes;

namespace BindingGen
{
    class ScePlayStation4Tools : ILibrary
    {
        public void Setup(Driver driver)
        {
            var options = driver.Options;

            // This sets the correct calling convention 
            // and target architecture for PS4.
            //options.Abi = CppAbi.Microsoft;
            options.TargetTriple = "x86_64";

            options.MicrosoftMode = true;
            //options.NoBuiltinIncludes = true;
            options.WriteOnlyWhenChanged = true;

            // Flip on verbose mode if you have an issue to debug.
            options.Verbose = true;
            options.Quiet = false;

            options.GenerateProperties = true;
            options.GenerateFunctionTemplates = true;
            //options.GenerateVirtualTables = true;
            //options.GenerateFinalizers = true;

            options.GenerateLibraryNamespace = true;
            options.LibraryName = "Sce.PlayStation4.Tools.Texture";
            options.OutputNamespace = "Sce.PlayStation4.Tools.Texture";
            options.SharedLibraryName = "libSceTextureTool.dll";

            //options.Defines.Add("WIN32");
            options.Defines.Add("_WIN32");
            options.Defines.Add("__cplusplus");
            options.Defines.Add("_CONSOLE");
            options.Defines.Add("_WINDOWS");
            options.Defines.Add("_CRT_SECURE_NO_WARNINGS");
            options.Defines.Add("NDEBUG");
            options.Defines.Add("SCE_GNM_RELEASE");
            options.Defines.Add("_MSC_VER=1700");
            options.Defines.Add("_MSC_FULL_VER=170061030");
            options.Defines.Add("_M_X86");
            options.Defines.Add("_M_X64");
            options.Defines.Add("_SIZE_T_DEFINED");
            
            //
            //options.Defines.Add("_WIN64");
            //options.Defines.Add("_WCHAR_T_DEFINED");
             
            // The paths to search for headers.
            var orbisSDK = System.Environment.GetEnvironmentVariable("SCE_ORBIS_SDK_DIR");
            options.IncludeDirs.Add(orbisSDK + @"\target\include_common"); 

            // The headers to process.
            //
            // NOTE: The casing here will influence the final .cs file.
            //
            options.Headers.Add(@"texture_tool.h");

            // Make sure any system headers needed is available.
            //options.SystemIncludeDirs.Add(orbisSDK + @"\host_tools\lib\clang\include");
            //options.SystemIncludeDirs.Add(orbisSDK + @"\target\include");
            //options.SystemIncludeDirs.Add(orbisSDK + @"\target\include_common");

            // Output it to the C# wrapper project.
            options.OutputDir = @"..\Sce.PlayStation4.Tools";
        }

        public void SetupPasses(Driver driver)
        {
            // TODO: The GenerateInlinesCodePass is what creates those inlines.cpp/txt files 
            // in the output folder which we never use.  This gross hack disables them.
            driver.TranslationUnitPasses.Passes.Remove(
                driver.TranslationUnitPasses.FindPass<GenerateInlinesCodePass>());

            //driver.AddTranslationUnitPass(new CheckMacroPass());
        }

        public void Preprocess(Driver driver, ASTContext lib)
        {
            lib.IgnoreHeadersWithName("tools_assert");
        }

        public void Postprocess(Driver driver, ASTContext lib)
        {
        }
    }
}
