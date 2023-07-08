from . import transform_ast_cpp_add_depth
from . import transform_ast_cpp_add_new_line
from . import transform_ast_cpp_comment_style
from . import transform_ast_cpp_include
from . import transform_ast_cpp_rename_pch
from . import transform_ast_cpp_rename_variables
from . import transform_ast_cpp_sort_class_struct_methods_members
from . import transform_ast_cpp_sort_include

def TransformAst(in_ast):
    in_ast.Visit(transform_ast_cpp_comment_style.AstTransformCommentStyle)
    in_ast.Visit(transform_ast_cpp_rename_pch.AstTransformRenamePCH)
    in_ast.Visit(transform_ast_cpp_include.AstTransformInclude)
    in_ast.Visit(transform_ast_cpp_sort_include.AstTransformSortInclude)
    in_ast.Visit(transform_ast_cpp_sort_class_struct_methods_members.AstTransformSortClassStructMethodsMembers)
    in_ast.Visit(transform_ast_cpp_rename_variables.AstTransformRenameVariables)
    in_ast.Visit(transform_ast_cpp_add_new_line.AstTransformAddNewLine)
    in_ast.Visit(transform_ast_cpp_add_depth.AstTransformAddDepth)

    print("post transform abstract syntax tree")
    in_ast.Dump()