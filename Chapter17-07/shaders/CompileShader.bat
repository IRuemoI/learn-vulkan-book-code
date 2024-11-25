cd /d %~dp0
%VK_SDK_PATH%\Bin\glslangValidator.exe -V commonTex.vert -o commonTex.vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V commonTex.frag -o commonTex.frag.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V commonTexLoad.vert -o commonTexLoad.vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V commonTexLoad.frag -o commonTexLoad.frag.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V computeBD.comp -o  computeBD.comp.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V computeNormal.comp -o  computeNormal.comp.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V SingleTexLight.vert -o SingleTexLight.vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V SingleTexLight.frag -o SingleTexLight.frag.spv
echo "Shader compilation completed!"

