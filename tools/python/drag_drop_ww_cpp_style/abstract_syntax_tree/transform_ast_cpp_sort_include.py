from . import dsc_token_cpp
from . import dsc_ast_cpp

def GetIncludePathSpec(in_ast_node):
    if in_ast_node._type != dsc_ast_cpp.AstType.PREPROCESSOR or in_ast_node._sub_type != dsc_ast_cpp.SubType.PREPROCESSOR_INCLUDE:
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
    remaining = []
    array_to_sort = []
    before_includes = True
    depth = 0
    for child in in_ast_node._children:

        found = True
        if child._type == dsc_ast_cpp.AstType.PREPROCESSOR:
            if child._token._sub_type == dsc_token_cpp.PreprocessorType.IF:
                depth += 1
            elif child._token._sub_type == dsc_token_cpp.PreprocessorType.IFDEF:
                depth += 1
            elif child._token._sub_type == dsc_token_cpp.PreprocessorType.IFNDEF:
                depth += 1
            elif child._token._sub_type == dsc_token_cpp.PreprocessorType.ENDIF:
                depth -= 1
            else:
                found = False
        else:
            found = False

        if depth != 0 or found == True:
            if True == before_includes:
                new_child_array.append(child)
            else:
                remaining.append(child) 
            continue

        path_spec = GetIncludePathSpec(child)
        if "" == path_spec:
            if True == before_includes:
                new_child_array.append(child)
            else:
                remaining.append(child) 
        elif "pch.h" in path_spec:
            before_includes = False
            new_child_array.append(child)
        else:
            before_includes = False
            array_to_sort.append(child)

    array_to_sort.sort(key = GetIncludePathSpec)

    for child in array_to_sort:
        new_child_array.append(child)

    for child in remaining:
        new_child_array.append(child)

    in_ast_node._children = new_child_array

    return False
