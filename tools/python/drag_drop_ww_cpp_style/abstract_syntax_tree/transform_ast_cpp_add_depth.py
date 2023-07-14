from . import export
from . import dsc_ast_cpp
from . import dsc_token_cpp

def DoesParenthesisChildrenNeedNewline(in_ast_node, in_stack_ast_node):
    if in_ast_node._type != dsc_ast_cpp.AstType.PARENTHESIS:
        return False
    for item in in_ast_node._children:
        if item._token and item._token._data == ",":
            return True
    return False

def AstTransformAddDepth(in_ast_node, in_stack_ast_node, in_data):
    # scope depth
    if in_ast_node._type == dsc_ast_cpp.AstType.SCOPE:
        in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)
        for child in in_ast_node._children:
            if child._type == dsc_ast_cpp.AstType.SCOPE_END:
                child._export_pre_token_format.append(export.ExportFormat.DEPTH_DECREMENT)
                break

    # access specifier one tab to the left
    if in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_ACCESS:
        in_ast_node._export_pre_token_format.append(export.ExportFormat.DEPTH_DECREMENT)
        in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)


    # Put children of first parenthesis of a statement on a new line
    if True == DoesParenthesisChildrenNeedNewline(in_ast_node, in_stack_ast_node):
        in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)
        for child in in_ast_node._children:
            if child._type == dsc_ast_cpp.AstType.PARENTHESIS_END:
                child._export_post_token_format.append(export.ExportFormat.DEPTH_DECREMENT)
                break

    # Put certain preprocessor blocks to the left
    if (
        in_ast_node._token and 
        in_ast_node._token._type == dsc_token_cpp.TokenType.PREPROCESSOR and
        in_ast_node._token._sub_type in set({
            dsc_token_cpp.PreprocessorType.IF,
            dsc_token_cpp.PreprocessorType.IFDEF,
            dsc_token_cpp.PreprocessorType.IFNDEF,
            dsc_token_cpp.PreprocessorType.ELSE,
            dsc_token_cpp.PreprocessorType.ELIF,
            dsc_token_cpp.PreprocessorType.ENDIF,
        })
        ):
        in_ast_node._export_pre_token_format.append(export.ExportFormat.DEPTH_PUSH)
        in_ast_node._export_post_children_format.append(export.ExportFormat.DEPTH_POP)

    return True
