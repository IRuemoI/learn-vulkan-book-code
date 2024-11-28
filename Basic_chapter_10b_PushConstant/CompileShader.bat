cd /d %~dp0
%VK_SDK_PATH%\Bin\glslangValidator.exe -V PushConstant.vert -o PushConstant-vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V PushConstant.frag -o PushConstant-frag.spv
echo "Shader compilation completed!"
