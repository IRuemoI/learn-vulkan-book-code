cd /d %~dp0
%VK_SDK_PATH%\Bin\glslangValidator.exe -V shader.vert -o shader.vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V shader.frag -o shader.frag.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V dashboard2D.vert -o  dashboard2D.vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V dashboard2D.frag -o  dashboard2D.frag.spv
echo "Shader compilation completed!"

