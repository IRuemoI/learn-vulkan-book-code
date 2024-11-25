import noise
import numpy as np

# 图像尺寸
width = 512
height = 512

# 创建一个空的图像数组
image = np.zeros((height, width), dtype=np.float32)

# 缩放因子
scale = 100.0

# 生成Perlin噪声图像
for y in range(height):
    for x in range(width):
        # 计算每个像素点的Perlin噪声值
        n = noise.pnoise2(x / scale, y / scale)
        # 将噪声值映射到0-255范围
        image[y][x] = (n + 1) / 2 * 255

# 将图像数据类型转换为uint8
image = image.astype(np.uint8)

# 图像文件名
output_filename = 'output.noise.raw'

# 手动二进制写入
try:
    with open(output_filename, 'wb') as f:
        f.write(image.tobytes())
    print(f"Image saved as {output_filename}")
except Exception as e:
    print(f"Error occurred while saving the image: {e}")

