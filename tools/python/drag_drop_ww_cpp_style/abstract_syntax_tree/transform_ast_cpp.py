from . import transform_ast_cpp_add_depth
from . import transform_ast_cpp_add_new_line
from . import transform_ast_cpp_comment_style
from . import transform_ast_cpp_include
from . import transform_ast_cpp_rename_pch
from . import transform_ast_cpp_rename_variables
from . import transform_ast_cpp_sort_class_struct_methods_members
from . import transform_ast_cpp_sort_include
from . import transform_ast_cpp_white_space

def SaveTextFile(in_file_path, in_data):
    text_file = open(in_file_path, "w")
    n = text_file.write(in_data)
    text_file.close()

def TransformAst(in_ast, in_debug):
    in_ast.Visit(transform_ast_cpp_comment_style.AstTransformCommentStyle)
    in_ast.Visit(transform_ast_cpp_rename_pch.AstTransformRenamePCH)
    in_ast.Visit(transform_ast_cpp_include.AstTransformInclude)
    in_ast.Visit(transform_ast_cpp_sort_include.AstTransformSortInclude)
    in_ast.Visit(transform_ast_cpp_sort_class_struct_methods_members.AstTransformSortClassStructMethodsMembers)
    in_ast.Visit(transform_ast_cpp_rename_variables.AstTransformRenameVariables)
    in_ast.Visit(transform_ast_cpp_white_space.AstTransformWhiteSpace, [], {"history":[]})
    in_ast.Visit(transform_ast_cpp_add_new_line.AstTransformAddNewLine)
    in_ast.Visit(transform_ast_cpp_add_depth.AstTransformAddDepth)

    if True == in_debug:
        #print("==========(post transform abstract syntax tree)===========")
        debug_file_data = in_ast.Dump()
        SaveTextFile("build\\debug_ast_post_transform.txt", debug_file_data)

    return