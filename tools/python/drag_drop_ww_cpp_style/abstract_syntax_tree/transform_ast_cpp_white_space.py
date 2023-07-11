from . import export
from . import dsc_ast_cpp
from . import dsc_token_cpp

s_thingsWePutASpaceAfter = set({
    # dsc_ast_cpp.AstType.PREPROCESSOR_PRAGMA,
    # dsc_ast_cpp.AstType.STATEMENT_CLASS,
    # dsc_ast_cpp.AstType.STATEMENT_STRUCT,
    # dsc_ast_cpp.AstType.STATEMENT_CONSTRUCTOR,
    # #dsc_ast_cpp.AstType.STATEMENT_DESTRUCTOR,
    # dsc_ast_cpp.AstType.STATEMENT_METHOD_DECLARATION,
    # dsc_ast_cpp.AstType.STATEMENT_METHOD_DEFINITION,
    # dsc_ast_cpp.AstType.STATEMENT_MEMBER,
    # dsc_ast_cpp.AstType.STATEMENT_FORWARD_DECLARATION,
    # dsc_ast_cpp.AstType.STATEMENT,
    dsc_ast_cpp.AstType.TOKEN,
    })

def AstTransformWhiteSpace(in_ast_node, in_stack_ast_node, in_data):
    history = in_data["history"]

    if in_ast_node._token and in_ast_node._token._type == dsc_token_cpp.TokenType.TOKEN:
        if 0 < len(history) and history[-1]._type in s_thingsWePutASpaceAfter:
            prev_node = history[-1]
            if prev_node._token and prev_node._token._data in set({"~", "::", "."}):
                pass
            else:
                in_ast_node._export_pre_token_format.append(export.ExportFormat.WHITE_SPACE)

    if in_ast_node._type != dsc_ast_cpp.AstType.WHITE_SPACE:
        history.append(in_ast_node)

    if in_ast_node._sub_type == dsc_ast_cpp.SubType.PREPROCESSOR_INCLUDE:
        in_ast_node._export_post_token_format.append(export.ExportFormat.WHITE_SPACE)

    return True
