from . import dsc_token_cpp
from . import dsc_ast_cpp
from . import export

def GetIncludePathSpec(in_ast_node):
    if in_ast_node._sub_type != dsc_ast_cpp.SubType.PREPROCESSOR_INCLUDE:
        return ""
    for child in in_ast_node._children:
        if child._type != dsc_ast_cpp.AstType.TOKEN:
            continue
        return child._token._data
    return ""


def DoesParenthesisChildrenNeedNewline(in_ast_node, in_stack_ast_node):
    if in_ast_node._type != dsc_ast_cpp.AstType.PARENTHESIS:
        return False
    for item in in_ast_node._children:
        if item._token and item._token._data == ",":
            return True
    return False

def AstTransformAddNewLine(in_ast_node, in_stack_ast_node, in_data):
    parent = None
    if 0 < len(in_stack_ast_node):
        parent = in_stack_ast_node[-1]
    grand_parent = None
    if 1 < len(in_stack_ast_node):
        grand_parent = in_stack_ast_node[-2]

    if (
        parent is not None and
        parent._type == dsc_ast_cpp.AstType.DOCUMENT
        ):
        in_ast_node._export_post_children_format.append(export.ExportFormat.NEW_LINE)
    elif (
        in_ast_node._type == dsc_ast_cpp.AstType.COMMENT or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.PREPROCESSOR_INCLUDE or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_ACCESS or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_CLASS or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_STRUCT or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_CONSTRUCTOR or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_DESTRUCTOR or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DECLARATION or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_MEMBER or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_FORWARD_DECLARATION
        ):
        in_ast_node._export_post_children_format.append(export.ExportFormat.NEW_LINE)
    elif (
        in_ast_node._type == dsc_ast_cpp.AstType.SCOPE_END
        ):
        if grand_parent and grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION:
            in_ast_node._export_post_token_format.append(export.ExportFormat.NEW_LINE)
        else:
            in_ast_node._export_pre_token_format.append(export.ExportFormat.NEW_LINE)

    # extra newline for include groups
    if in_ast_node._type == dsc_ast_cpp.AstType.DOCUMENT:
        prev_include = None
        prev_path_spec = ""
        for child in in_ast_node._children:
            path_spec = GetIncludePathSpec(child)
            if "" == path_spec:
                continue
            if "pch.h" in path_spec:
                child._export_post_children_format.append(export.ExportFormat.NEW_LINE)
                continue
            if prev_path_spec != "" and prev_path_spec[:2] != path_spec[:2]:
                child._export_post_children_format.append(export.ExportFormat.NEW_LINE)
            prev_path_spec = path_spec
            prev_include = child
        if None != prev_include:
            prev_include._export_post_children_format.append(export.ExportFormat.NEW_LINE)

    # if i am not the first ACCESS_SPECIFIER of the parent scope, add a blank line above
    if in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_ACCESS and parent is not None:
        is_first_accessor = True
        for child in parent._children:
            if child._sub_type == dsc_ast_cpp.SubType.STATEMENT_ACCESS:
                is_first_accessor = id(in_ast_node) == id(child)
                break
        if False == is_first_accessor:
            in_ast_node._export_pre_token_format.append(export.ExportFormat.NEW_LINE)

    # a blank line after forward declarations
    if in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_FORWARD_DECLARATION and parent is not None:
        is_last = False
        for child in reversed(parent._children):
            if child._sub_type == dsc_ast_cpp.SubType.STATEMENT_FORWARD_DECLARATION:
                is_last = id(in_ast_node) == id(child)
                break
        if True == is_last:
            in_ast_node._export_post_children_format.append(export.ExportFormat.NEW_LINE)

    # put the { on a new line
    if (
        in_ast_node._type == dsc_ast_cpp.AstType.SCOPE and
        parent is not None and
        (
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_CLASS or
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_STRUCT or
            parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION 
        )):
        in_ast_node._export_pre_token_format.append(export.ExportFormat.NEW_LINE)
        in_ast_node._export_post_token_format.append(export.ExportFormat.NEW_LINE)

    # Put children of first parenthesis of a statement on a new line
    if True == DoesParenthesisChildrenNeedNewline(in_ast_node, in_stack_ast_node):
        in_ast_node._export_post_token_format.append(export.ExportFormat.NEW_LINE)
        for child in in_ast_node._children:
            if child._token and child._token._data == ",":
                child._export_post_token_format.append(export.ExportFormat.NEW_LINE)
            elif child._type == dsc_ast_cpp.AstType.PARENTHESIS_END:
                child._export_pre_token_format.append(export.ExportFormat.NEW_LINE)

    # Deal formal paramater list
    if in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION:
        depth_incremented = False
        for child in in_ast_node._children:
            if child._token and child._token._data == ":":
                child._export_pre_token_format.append(export.ExportFormat.DEPTH_INCREMENT)
                child._export_pre_token_format.append(export.ExportFormat.NEW_LINE)
                depth_incremented = True
            elif child._token and child._token._data == ",":
                child._export_pre_token_format.append(export.ExportFormat.NEW_LINE)
        if True == depth_incremented:
            in_ast_node._children[-1]._export_pre_token_format.append(export.ExportFormat.DEPTH_DECREMENT)


    return True
