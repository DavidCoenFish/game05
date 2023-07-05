#C:\Python310\python.exe C:\development\game05\tools\python\drag_drop_ww_cpp_style\drag_drop_ww_cpp_style.py C:\development\game05\engine\common\source\common\draw_system\custom_command_list.h
import sys
import os
import abstract_syntax_tree.parse_file
import abstract_syntax_tree.export

def DealFile(in_file_path):
    ast = abstract_syntax_tree.parse_file.LoadFile(in_file_path)
    abstract_syntax_tree.export.SaveFile(ast, in_file_path + ".out")
    return

def Main(*in_args):
    if 0 == len(in_args):
        print("usage drag_drop_file_rename_snake_case.py: drag and drop files to rename")
        return
    for iter in in_args[1:]:
        DealFile(iter)
    return

if __name__ == "__main__":
    Main(*sys.argv)

