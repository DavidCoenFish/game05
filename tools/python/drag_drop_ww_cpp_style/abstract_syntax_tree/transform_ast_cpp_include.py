from . import dsc_token_cpp
from . import dsc_ast_cpp
from . import common

def AdjustInclude(in_path_spec):
    in_path_spec = in_path_spec.replace("\\", "/")
    result = in_path_spec[:1]
    last = in_path_spec[-1:]
    data = in_path_spec[1:]
    data = data[:-1]

    first = True
    array_step = data.split("/")
    for step in array_step:
        if True == first:
            first = False
        else:
            result += "/"
        result += common.SnakeCase(step)

    result += last
    return result

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

# "Common/DrawSystem/DrawSystem.h" => "common/draw_system/draw_system.h"
def AstTransformInclude(in_ast_node, in_stack_ast_node, in_data):
    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "" != path_spec:
            if "pch.h" in path_spec:
                continue
            new_path = AdjustInclude(path_spec)
            SetIncludePathSpec(child, new_path)
    return False