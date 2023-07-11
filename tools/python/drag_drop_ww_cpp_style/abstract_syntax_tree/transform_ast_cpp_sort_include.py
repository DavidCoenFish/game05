from . import dsc_token_cpp
from . import dsc_ast_cpp

def GetIncludePathSpec(in_ast_node):
    if in_ast_node._type != dsc_ast_cpp.AstType.PREPROCESSOR and in_ast_node._sub_type != dsc_ast_cpp.SubType.PREPROCESSOR_INCLUDE:
        return ""
    for child in in_ast_node._children:
        if child._type != dsc_ast_cpp.AstType.TOKEN:
            continue
        return child._token._data
    return ""

def SetIncludePathSpec(in_ast_node, in_path_spec):
    if in_ast_node._type != dsc_ast_cpp.AstType.PREPROCESSOR and in_ast_node._sub_type != dsc_ast_cpp.SubType.PREPROCESSOR_INCLUDE:
        return ""
    for child in in_ast_node._children:
        if child._type != dsc_ast_cpp.AstType.TOKEN:
            continue
        child._token._data = in_path_spec

# leave pch at top, alpha numeric sort other includes to follow
def AstTransformSortInclude(in_ast_node, in_stack_ast_node, in_data):
    new_child_array = []
    array_to_sort = []
    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "" == path_spec:
            continue 
        if "pch.h" in path_spec:
            new_child_array.append(child)
            continue 
        array_to_sort.append(child)
    array_to_sort.sort(key = GetIncludePathSpec)

    first_char = ""
    for child in array_to_sort:
        new_child_array.append(child)

    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "" != path_spec:
            continue 
        new_child_array.append(child)

    in_ast_node._children = new_child_array

    return False
