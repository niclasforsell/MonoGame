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
    class Library : ILibrary
    {
        public void Setup(Driver driver)
        {
            var options = driver.Options;

            // This sets the correct calling convention 
            // and target architecture for PS4.
            options.Abi = CppAbi.Itanium;
            options.TargetTriple = "x86_64";
   
            options.MicrosoftMode = false;
            options.NoBuiltinIncludes = true;
            options.WriteOnlyWhenChanged = true;

            // Flip on verbose mode if you have an issue to debug.
            options.Verbose = false;
            options.Quiet = false;

            options.GenerateProperties = true;
            options.GenerateFunctionTemplates = true;
            //options.GenerateVirtualTables = true;
            options.GenerateFinalizers = true;

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
            options.Headers.Add(@"Game.h");
            options.Headers.Add(@"Audio\AudioBuffer.h");
            options.Headers.Add(@"Audio\SamplerVoice.h");
            options.Headers.Add(@"Audio\SoundSystem.h");
            options.Headers.Add(@"Graphics\Texture.h");
            options.Headers.Add(@"Graphics\RenderTarget.h");
            options.Headers.Add(@"Graphics\GraphicsSystem.h");
            options.Headers.Add(@"Graphics\TextureFormat.h");
            options.Headers.Add(@"Graphics\VertexShader.h");
            options.Headers.Add(@"Graphics\PixelShader.h");
            options.Headers.Add(@"Graphics\VertexBuffer.h");
            options.Headers.Add(@"Graphics\IndexBuffer.h");
            options.Headers.Add(@"Input\GamePadState.h");
            options.Headers.Add(@"Input\GamePad.h");
            options.Headers.Add(@"UserService.h");

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
            // Force it to skip generation of anything in the predecls header.
            lib.IgnoreHeadersWithName("predecls");

            lib.SetClassAsValueType("Input::GamePadState");
        }

        public void Postprocess(Driver driver, ASTContext lib)
        {
        }

        static class Program
        {
            public static void Main(string[] args)
            {
                ConsoleDriver.Run(new Library());

                // HACK: It would be nice if we could just disable the compile
                // step instead of doing hardcoded cleanup here.
                try { File.Delete("Sce.PlayStation4.dll"); } catch {}
                try { File.Delete("Sce.PlayStation4.pdb"); } catch {}
                try { File.Delete("Sce.PlayStation4.xml"); } catch {}
            }
        }
    }
}
