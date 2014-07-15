using System;
using System.IO;

namespace TwoMGFX
{
    partial class EffectObject
    {
        private static byte[] CompilePSSL(ShaderInfo shaderInfo, string shaderFunction, string shaderProfile, ref string errorsAndWarnings)
        {
            var compiler = new sce.PlayStation4.Tools.ShaderCompiler();

            try
            {
                string buildOutput;
                compiler.Compile(   shaderInfo.FilePath,
                                    shaderInfo.FileContent,
                                    shaderFunction,
                                    shaderProfile,
                                    shaderInfo.Debug,
                                    out buildOutput);

                // Store all the errors and warnings to log out later.
                errorsAndWarnings += buildOutput;

                if (shaderInfo.Debug && compiler.ShaderDebugData != null)
                {
                    var baseFile = Path.GetFileName(shaderInfo.FilePath);
                    var baseFolder = Path.GetDirectoryName(shaderInfo.OutputFilePath);
                    var shaderDebugFile = Path.Combine( baseFolder,
                                                        baseFile + "." + shaderFunction + ".sdb");
                    File.WriteAllBytes(shaderDebugFile, compiler.ShaderDebugData);

                    // Register the additional output file
                    // so that MGCB can do cleanup later.
                    shaderInfo.AdditionalOutputFiles.Add(shaderDebugFile);
                }
            }
            catch (Exception ex)
            {
                errorsAndWarnings += ex.Message;
                throw new ShaderCompilerException();
            }

            return compiler.Bytecode;
        }
    }
}
