from . import dsc_token_cpp
from . import dsc_ast_cpp

def GetIncludePathSpec(in_ast_node):
    if in_ast_node._type != dsc_ast_cpp.AstType.PREPROCESSOR_INCLUDE:
        return ""
    for child in in_ast_node._children:
        if child._type != dsc_ast_cpp.AstType.TOKEN:
            continue
        return child._token._data
    return ""

def SetIncludePathSpec(in_ast_node, in_path_spec):
    if in_ast_node._type != dsc_ast_cpp.AstType.PREPROCESSOR_INCLUDE:
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
        # new_first_char = GetIncludePathSpec(child)[:1]
        # if first_char != new_first_char:
        #     first_char = new_first_char
        #     new_child_array.append(dsc_ast_cpp.AST(
        #         dsc_ast_cpp.AstType.WHITE_SPACE, 
        #         dsc_token_cpp.Token("\n", dsc_token_cpp.TokenType.WHITE_SPACE)
        #         ))
        new_child_array.append(child)

    # if 0 != len(array_to_sort):
    #     new_child_array.append(dsc_ast_cpp.AST(
    #         dsc_ast_cpp.AstType.WHITE_SPACE, 
    #         dsc_token_cpp.Token("\n", dsc_token_cpp.TokenType.WHITE_SPACE)
    #         ))

    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "" != path_spec:
            continue 
        new_child_array.append(child)

    in_ast_node._children = new_child_array

    return False
