#C:\Python310\python.exe C:\development\game05\tools\python\drag_drop_file_rename_snake_case\drag_drop_file_rename_snake_case.py C:\temp\TestFile00.txt C:\temp\iTestFile01.txt
import sys
import os

def ConvertSnakeCase(in_text):
    result = ""
    for c in in_text:
        upper_case = c.isupper()
        if True == upper_case:
            if "" != result:
                result += "_"
            c = c.lower()
        result += c
    return result

def DealFile(in_file_path):
    stem, leaf = os.path.split(in_file_path)
    leaf = ConvertSnakeCase(leaf)
    new_path = os.path.join(stem, leaf)
    if in_file_path == new_path:
        return
    print(f"{in_file_path} rename as {new_path}")
    os.rename(in_file_path, new_path)
    return

def Main(*in_args):
    if 0 == len(in_args):
        print("usage drag_drop_file_rename_snake_case.py: drag and drop files to rename")
        return
    for iter in in_args:
        DealFile(iter)
    return

if __name__ == "__main__":
    Main(*sys.argv)

