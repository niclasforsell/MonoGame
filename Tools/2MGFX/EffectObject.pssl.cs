using System;

namespace TwoMGFX
{
    partial class EffectObject
    {
        private static byte[] CompilePSSL(ShaderInfo shaderInfo, string shaderFunction, string shaderProfile)
        {
            var compiler = new sce.PlayStation4.Tools.ShaderCompiler();

            compiler.Compile(   shaderInfo.fileContent,
                                shaderFunction,
                                shaderProfile);

            return compiler.Bytecode;
        }
    }
}
