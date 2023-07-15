#C:\Python310\python.exe C:\development\game05\tools\python\drag_drop_file_rename_snake_case\drag_drop_file_rename_snake_case.py C:\temp\TestFile00.txt C:\temp\iTestFile01.txt
import sys
import os
import dsc.common


def DealFile(in_file_path, in_debug):
    stem, leaf = os.path.split(in_file_path)
    leaf = dsc.common.SnakeCase(leaf)
    new_path = os.path.join(stem, leaf)
    if in_file_path == new_path:
        return
    print(f"{in_file_path} rename as {new_path}")
    if True == in_debug:
        return
    os.rename(in_file_path, new_path)
    return

def Main(*in_args):
    debug = False #True
    if 0 == len(in_args):
        print("usage drag_drop_file_rename_snake_case.py: drag and drop files to rename")
        return
    for iter in in_args:
        DealFile(iter, debug)
    return

if __name__ == "__main__":
    Main(*sys.argv)

