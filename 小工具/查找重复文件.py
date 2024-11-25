import hashlib
import os

# 计算文件的MD5哈希值
def calculate_file_hash(file_path):
    hash_md5 = hashlib.md5()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

# 查找指定目录下的重复文件
def find_duplicate_files(directory, output_filename):
    file_info = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            file_hash = calculate_file_hash(file_path)
            file_info.append((file, file_hash))
    file_info.sort(key=lambda x: x[1])

    duplicate_dict = {}
    unique_files = []  # 存储不重复的文件名
    for info in file_info:
        file_name, file_hash = info
        if file_hash not in duplicate_dict:
            duplicate_dict[file_hash] = [file_name]
            unique_files.append(file_name)  # 添加到不重复文件列表
        else:
            duplicate_dict[file_hash].append(","+file_name)

    # 构建输出内容并写入文件
    with open(output_filename, 'w') as f:
        # 写入不重复的文件名
        f.write("以下是内容不重复的文件名：\n")
        for unique_file in unique_files:
            f.write(unique_file + "\n")
        f.write("\n")
        
        # 写入重复的文件名
        for hash_value, file_list in duplicate_dict.items():
            if len(file_list) > 1:
                first_file = file_list
                if ',' in first_file:
                    first_file = first_file.split(',')
                f.write(f"与{file_list[0]}文件内容相同的有：{"".join(file_list[1:]).replace(","," ")}\n")

# 程序入口
if __name__ == "__main__":
    find_duplicate_files("E:/Code/cpp/DemoProjects/textures", "textures.txt")
    find_duplicate_files("E:/Code/cpp/DemoProjects/models", "models.txt")