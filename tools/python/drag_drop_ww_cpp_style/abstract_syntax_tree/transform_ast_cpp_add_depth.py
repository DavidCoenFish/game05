from . import export
from . import dsc_ast_cpp

def AstTransformAddDepth(in_ast_node, in_stack_ast_node, in_data):
    parent = None
    if 0 < len(in_stack_ast_node):
        parent = in_stack_ast_node[-1]

    if (
        parent is not None and
        (
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_CLASS or
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_STRUCT or 
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_CONSTRUCTOR or 
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_DESTRUCTOR or  
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION
        ) and
        in_ast_node._type == dsc_ast_cpp.AstType.SCOPE
        ):
        in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)
    if (
        1 < len(in_stack_ast_node) and
        (
            in_stack_ast_node[-2]._sub_type == dsc_ast_cpp.SubType.STATEMENT_CLASS or
            in_stack_ast_node[-2]._sub_type == dsc_ast_cpp.SubType.STATEMENT_STRUCT or
            in_stack_ast_node[-2]._sub_type == dsc_ast_cpp.SubType.STATEMENT_CONSTRUCTOR or 
            in_stack_ast_node[-2]._sub_type == dsc_ast_cpp.SubType.STATEMENT_DESTRUCTOR or  
            in_stack_ast_node[-2]._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION
        ) and
        in_ast_node._type == dsc_ast_cpp.AstType.SCOPE_END
        ):
        in_ast_node._export_pre_token_format.append(export.ExportFormat.DEPTH_DECREMENT)

    if in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_ACCESS:
        in_ast_node._export_pre_token_format.append(export.ExportFormat.DEPTH_DECREMENT)
        in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)


    # put children of patenthesis on a new line, unless PARENTHESIS_END is the only child
    if (
        in_ast_node._type == dsc_ast_cpp.AstType.PARENTHESIS and
        parent is not None and
        (
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION or
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_CONSTRUCTOR
        )):
        if len(in_ast_node._children) <= 1:
            pass
        else:
            in_ast_node._export_post_token_format.append(export.ExportFormat.DEPTH_INCREMENT)
            for child in in_ast_node._children:
                if child._type == dsc_ast_cpp.AstType.PARENTHESIS_END:
                    child._export_post_token_format.append(export.ExportFormat.DEPTH_DECREMENT)

    return True
