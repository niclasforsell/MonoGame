using CppSharp;
using CppSharp.Parser.AST;
using CppSharp.Passes;

namespace BindingGen
{
    class ScePlayStation4 : ILibrary
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
            options.NoStandardIncludes = true;
            options.WriteOnlyWhenChanged = true;
            options.CompileCode = false;

            // Flip on verbose mode if you have an issue to debug.
            options.Verbose = false;
            options.Quiet = false;

            options.GenerateProperties = true;
            options.GenerateFunctionTemplates = true;
            //options.GenerateVirtualTables = true;
            //options.GenerateFinalizers = true;

            options.GenerateLibraryNamespace = true;
            options.LibraryName = "Sce.PlayStation4";
            options.OutputNamespace = "Sce.PlayStation4";
            options.SharedLibraryName = "scePlayStation4.prx";

            options.addDefines("__ORBIS__");
            options.addDefines("CS_BINDING_GEN");
            
            // The paths to search for headers.
            options.addIncludeDirs(@".\");

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
            //options.Headers.Add(@"Graphics\TextureFormat.h");
            options.Headers.Add(@"Graphics\VertexShader.h");
            options.Headers.Add(@"Graphics\FetchShader.h");
            options.Headers.Add(@"Graphics\PixelShader.h");
            options.Headers.Add(@"Graphics\PngReader.h");
            options.Headers.Add(@"Graphics\VertexBuffer.h");
            options.Headers.Add(@"Graphics\IndexBuffer.h");
            options.Headers.Add(@"Input\Camera.h");
            options.Headers.Add(@"Input\GamePadState.h");
            options.Headers.Add(@"Input\GamePad.h");
            options.Headers.Add(@"Input\KeyboardState.h");
            options.Headers.Add(@"Input\Keyboard.h");
            options.Headers.Add(@"Input\MouseState.h");
            options.Headers.Add(@"Input\Mouse.h");
            options.Headers.Add(@"Media\Song.h");
            options.Headers.Add(@"Media\VideoPlayer.h");
            //options.Headers.Add(@"Network\Np.h");
            options.Headers.Add(@"Network\NpCommerce.h");
            options.Headers.Add(@"Network\NpScore.h");
            options.Headers.Add(@"Network\TitleStorage.h");
            options.Headers.Add(@"Network\TrophySet.h");
            options.Headers.Add(@"System\PlayGo.h");
            options.Headers.Add(@"System\RemotePlay.h");
            options.Headers.Add(@"System\SystemService.h");
            options.Headers.Add(@"System\UserService.h");
            options.Headers.Add(@"System\SaveData.h");
            //options.Headers.Add(@"System\CommonDialog.h");
            options.Headers.Add(@"System\NpProfileDialog.h");
            options.Headers.Add(@"System\WebBrowserDialog.h");
            options.Headers.Add(@"System\ErrorDialog.h");
            options.Headers.Add(@"System\MsgDialog.h");
            options.Headers.Add(@"System\ScreenShot.h");
            options.Headers.Add(@"System\VideoRecording.h");

            // Make sure any system headers needed is available.
            var orbisSDK = System.Environment.GetEnvironmentVariable("SCE_ORBIS_SDK_DIR");
            options.addSystemIncludeDirs(orbisSDK + @"\host_tools\lib\clang\include");
            options.addSystemIncludeDirs(orbisSDK + @"\target\include");
            options.addSystemIncludeDirs(orbisSDK + @"\target\include_common");

            // Output it to the C# wrapper project.
            options.OutputDir = @"..\Sce.PlayStation4";
        }

        public void SetupPasses(Driver driver)
        {
            // TODO: The GenerateInlinesCodePass is what creates those inlines.cpp/txt files 
            // in the output folder which we never use.  This gross hack disables them.
            driver.TranslationUnitPasses.Passes.Remove(
                driver.TranslationUnitPasses.FindPass<GenerateInlinesCodePass>());

            driver.AddTranslationUnitPass(new CheckMacroPass());
        }

        public void Preprocess(Driver driver, CppSharp.AST.ASTContext lib)
        {
            // Force it to skip generation of anything in the predecls header.
            lib.IgnoreHeadersWithName("predecls");
        }

        public void Postprocess(Driver driver, CppSharp.AST.ASTContext lib)
        {
        }
    }
}
