#C:\Python310\python.exe C:\development\game05\tools\python\drag_drop_ww_cpp_style\drag_drop_ww_cpp_style.py C:\development\game05\engine\common\source\common\draw_system\custom_command_list.h
import sys
import os
import abstract_syntax_tree.parse_file
import abstract_syntax_tree.export
import abstract_syntax_tree.transform_ast_cpp

def DealFile(in_file_path, in_debug):
    ast = abstract_syntax_tree.parse_file.LoadFile(in_file_path, in_debug)
    abstract_syntax_tree.transform_ast_cpp.TransformAst(ast)
    abstract_syntax_tree.export.SaveFile(ast, in_file_path + ".out")
    return

def Main(*in_args):
    debug = True
    if 0 == len(in_args):
        print("usage drag_drop_ww_cpp_style.py: drag and drop files to convert style")
        return
    for iter in in_args[1:]:
        DealFile(iter, debug)
        if True == debug:
            break
    return

if __name__ == "__main__":
    Main(*sys.argv)

