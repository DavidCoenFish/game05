#C:\Python310\python.exe C:\development\game05\tools\python\drag_drop_ww_cpp_style\drag_drop_ww_cpp_style.py C:\development\game05\engine\common\source\common\draw_system\custom_command_list.h
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\custom_command_list.cpp",
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\custom_command_list.h"
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\d3dx12 - Copy.h"
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\heap_wrapper\\heap_wrapper_item.h"
#"C:\\development\\game05\\engine\\common\\source\\common\\draw_system\\heap_wrapper\\heap_wrapper_item.cpp"

#C:\development\game05\tools\python\drag_drop_ww_cpp_style\drag_drop_ww_cpp_style.exe C:\development\game05\engine\common\source\common\draw_system

import sys
import os
import abstract_syntax_tree.parse_file
import abstract_syntax_tree.export
import abstract_syntax_tree.transform_ast_cpp
import abstract_syntax_tree.common

def MakeNewFilePath(in_file_path):
    stem, leaf = os.path.split(in_file_path)
    leaf = abstract_syntax_tree.common.SnakeCase(leaf)
    new_path = os.path.join(stem, leaf)
    return new_path

def DealFile(in_file_path, in_debug):
    if in_file_path.endswith(".out"):
        print(f"Delete file:{in_file_path}")
        os.remove(in_file_path)
        return

    ast = abstract_syntax_tree.parse_file.LoadFile(in_file_path, in_debug)
    abstract_syntax_tree.transform_ast_cpp.TransformAst(ast, in_debug)
    if True == in_debug:
        in_file_path += ".out"
    else:
        os.remove(in_file_path)

    new_path = MakeNewFilePath(in_file_path)
    abstract_syntax_tree.export.SaveFile(ast, new_path)

    return

def Main(*in_args):
    debug = False #True #
    if 0 == len(in_args):
        print("usage drag_drop_ww_cpp_style.py: drag and drop files to convert style")
        return
    for iter in in_args[1:]:
        if os.path.isdir(iter):
            folder_rename_dict = []
            for root, dirs, files in os.walk(iter):
                for file in files:
                    path = os.path.join(root, file)
                    DealFile(path, debug)
                for dir in dirs:
                    old_dir_path = os.path.join(root, dir)
                    new_dir_path = os.path.join(root, abstract_syntax_tree.common.SnakeCase(dir))
                    if old_dir_path != new_dir_path:
                        folder_rename_dict.append((old_dir_path, new_dir_path))
            new_path = MakeNewFilePath(iter)
            if iter != new_path:
                folder_rename_dict.append((iter, new_path))
            for pair in folder_rename_dict:
                print(f"rename:{pair[0]} => {pair[1]}")
                if False == debug:
                    os.rename(pair[0], pair[1])
        else:
            DealFile(iter, debug)
        if True == debug:
            break
    return

if __name__ == "__main__":
    Main(*sys.argv)

