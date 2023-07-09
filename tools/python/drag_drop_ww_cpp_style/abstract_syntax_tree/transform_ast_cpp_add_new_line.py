from . import dsc_token_cpp
from . import dsc_ast_cpp
from . import export

def AstTransformAddNewLine(in_ast_node, in_stack_ast_node, in_data):
    if (
        0 < len(in_stack_ast_node) and
        in_stack_ast_node[-1]._type == dsc_ast_cpp.AstType.DOCUMENT
        ):
        in_ast_node._export_post_children_format.append(export.ExportFormat.NEW_LINE)
    elif (
        in_ast_node._type == dsc_ast_cpp.AstType.PREPROCESSOR_INCLUDE or
        in_ast_node._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER or
        in_ast_node._type == dsc_ast_cpp.AstType.COMMENT or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_CLASS or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_CONSTRUCTOR or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_DESTRUCTOR or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_METHOD_DECLARATION or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_METHOD_DEFINITION or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_MEMBER or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_FORWARD_DECLARATION
        ):
        in_ast_node._export_post_children_format.append(export.ExportFormat.NEW_LINE)

    if (
        0 < len(in_stack_ast_node) and
        (
            in_stack_ast_node[-1]._type == dsc_ast_cpp.AstType.STATEMENT_CLASS or
            in_stack_ast_node[-1]._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT        
        ) and
        in_ast_node._type == dsc_ast_cpp.AstType.SCOPE
        ):
        in_ast_node._export_pre_token_format.append(export.ExportFormat.NEW_LINE)
        in_ast_node._export_post_token_format.append(export.ExportFormat.NEW_LINE)

    return True
