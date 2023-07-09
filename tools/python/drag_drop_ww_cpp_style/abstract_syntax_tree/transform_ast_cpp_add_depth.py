from . import export
from . import dsc_ast_cpp

def AstTransformAddDepth(in_ast_node, in_stack_ast_node, in_data):
    if (
        0 < len(in_stack_ast_node) and
        (
            in_stack_ast_node[-1]._type == dsc_ast_cpp.AstType.STATEMENT_CLASS or
            in_stack_ast_node[-1]._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT
        ) and
        in_ast_node._type == dsc_ast_cpp.AstType.SCOPE
        ):
        in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)
    if (
        1 < len(in_stack_ast_node) and
        (
            in_stack_ast_node[-2]._type == dsc_ast_cpp.AstType.STATEMENT_CLASS or
            in_stack_ast_node[-2]._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT        
        ) and
        in_ast_node._type == dsc_ast_cpp.AstType.SCOPE_END
        ):
        in_ast_node._export_pre_token_format.append(export.ExportFormat.DEPTH_DECREMENT)

    if in_ast_node._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER:
        in_ast_node._export_pre_token_format.append(export.ExportFormat.DEPTH_DECREMENT)
        in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)


    return True
