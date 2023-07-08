from . import dsc_token_cpp
from . import dsc_ast_cpp

def MakeNewLineAst():
    return dsc_ast_cpp.AST(dsc_ast_cpp.AstType.NEW_LINE)

def AddNewLine(in_ast_node_array):
    in_ast_node_array.append(MakeNewLineAst())

def AddNewLineNoDuplicate(in_ast_node_array):
    if 0 < len(in_ast_node_array):
        if in_ast_node_array[-1]._type == dsc_ast_cpp.AstType.NEW_LINE:
            return
    AddNewLine(in_ast_node_array)

def AstTransformAddNewLine(in_ast_node, in_stack_ast_node, in_data):
    if in_ast_node._type == dsc_ast_cpp.AstType.COMMENT:
        if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_END_LINE:
            AddNewLineNoDuplicate(in_ast_node._children)
    if (
        in_ast_node._type == dsc_ast_cpp.AstType.PREPROCESSOR_INCLUDE or
        in_ast_node._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_END
        ):
        AddNewLineNoDuplicate(in_ast_node._children)

    if (
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_CLASS or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT
        ):
        #new_child_array = []
        for child in in_ast_node._children:
            if child._type == dsc_ast_cpp.AstType.SCOPE:
                child._children = [MakeNewLineAst()] + child._children
    #            #AddNewLineNoDuplicate(new_child_array)
    #             accessor_found = False
    #             new_sub_children = []
    #             for sub_child in child._children:
    #                 if sub_child._type == dsc_ast_cpp.AstType.NEW_LINE:
    #                     continue
    #                 if (
    #                     sub_child._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER or
    #                     sub_child._type == dsc_ast_cpp.AstType.SCOPE_END
    #                     ):
    #                     if accessor_found == True:
    #                         new_sub_children.append(MakeNewLineAst())
    #                     accessor_found = True
    #                 new_sub_children.append(sub_child)
    #             sub_child._children = new_sub_children

    #         #new_child_array.append(child)
    #     #in_ast_node._children = new_child_array

    if in_ast_node._type == dsc_ast_cpp.AstType.DOCUMENT:
        for child in in_ast_node._children:
            if (
                in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_CLASS or
                in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT
                ):
                pass
            else:
                AddNewLineNoDuplicate(child._children)

    return True
