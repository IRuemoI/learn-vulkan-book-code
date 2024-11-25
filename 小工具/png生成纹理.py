from PIL import Image
import math

def next_power_of_2(x):
    return 1 if x == 0 else 2**(x - 1).bit_length()

def png_to_custom_format_with_padding(png_file_path, custom_file_path):
    # 打开PNG图片
    img = Image.open(png_file_path)
    
    # 确保图片是RGBA模式
    if img.mode != 'RGBA':
        img = img.convert('RGBA')
    
    # 获取图片尺寸
    original_width, original_height = img.size
    
    # 找到大于或等于原始尺寸的最小2的幂
    new_width = next_power_of_2(original_width)
    new_height = next_power_of_2(original_height)
    
    # 创建一个新的透明图片
    new_img = Image.new('RGBA', (new_width, new_height), (0, 0, 0, 0))
    
    # 计算原始图片在新图片中的位置
    offset_x = (new_width - original_width) // 2
    offset_y = (new_height - original_height) // 2
    
    # 将原始图片粘贴到新图片中
    new_img.paste(img, (offset_x, offset_y))
    
    # 获取像素数据
    pixels = list(new_img.getdata())
    
    # 写入自定义文件
    with open(custom_file_path, 'wb') as f:
        # 写入像素数据
        for pixel in pixels:
            # 写入RGBA值
            f.write(pixel[0].to_bytes(1, 'little'))  # R
            f.write(pixel[1].to_bytes(1, 'little'))  # G
            f.write(pixel[2].to_bytes(1, 'little'))  # B
            f.write(pixel[3].to_bytes(1, 'little'))  # A

# 使用示例
png_file_path = './胖丁.png'  # 你的PNG图片路径
custom_file_path = './output.texture'  # 输出的自定义文件路径
png_to_custom_format_with_padding(png_file_path, custom_file_path)
