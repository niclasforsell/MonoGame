using System;

namespace TwoMGFX
{
    partial class EffectObject
    {
        private static byte[] CompilePSSL(ShaderInfo shaderInfo, string shaderFunction, string shaderProfile)
        {
            var compiler = new sce.PlayStation4.Tools.ShaderCompiler();

            try
            {
                string buildOutput;
                compiler.Compile(   shaderInfo.fileContent,
                                    shaderFunction,
                                    shaderProfile,
                                    out buildOutput);
            }
            catch (Exception ex)
            {
                throw ex;
            }

            return compiler.Bytecode;
        }
    }
}
