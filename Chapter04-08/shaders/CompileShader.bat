cd /d %~dp0
%VK_SDK_PATH%\Bin\glslangValidator.exe -V shader.vert -o shader.vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V shader.frag -o shader.frag.spv
echo "Shader compilation completed!"

