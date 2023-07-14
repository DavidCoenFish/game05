#C:\Python310\python.exe C:\development\game05\tools\python\drag_drop_ww_cpp_style\drag_drop_ww_cpp_style.py C:\development\game05\engine\common\source\common\draw_system\custom_command_list.h
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\custom_command_list.cpp",
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\custom_command_list.h"
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\d3dx12 - Copy.h"
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\heap_wrapper\\heap_wrapper_item.h"
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\heap_wrapper\\heap_wrapper_item.cpp"

import sys
import os
import abstract_syntax_tree.parse_file
import abstract_syntax_tree.export
import abstract_syntax_tree.transform_ast_cpp

def DealFile(in_file_path, in_debug):
    if in_file_path.endswith(".out"):
        print(f"Skip file:{in_file_path}")
        return
    ast = abstract_syntax_tree.parse_file.LoadFile(in_file_path, in_debug)
    abstract_syntax_tree.transform_ast_cpp.TransformAst(ast, in_debug)
    abstract_syntax_tree.export.SaveFile(ast, in_file_path + ".out")
    return

def Main(*in_args):
    debug = False #True #
    if 0 == len(in_args):
        print("usage drag_drop_ww_cpp_style.py: drag and drop files to convert style")
        return
    for iter in in_args[1:]:
        if os.path.isdir(iter):
            for root, dirs, files in os.walk(iter):
                for file in files:
                    path = os.path.join(root, file)
                    DealFile(path, debug)
        else:
            DealFile(iter, debug)
        if True == debug:
            break
    return

if __name__ == "__main__":
    Main(*sys.argv)

