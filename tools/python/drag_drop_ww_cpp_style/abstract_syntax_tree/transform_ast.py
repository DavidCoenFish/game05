import enum

from . import dsc_token2
from . import dsc_ast2

#CommonPCH.h => common/common_pch.h
def AstTransformRenamePCH(in_ast_node, in_stack):
    for child in in_ast_node._children:
        if child._type != dsc_ast2.AstType.PREPROCESSOR_INCLUDE:
            continue
        for sub_child in child._children:
            if sub_child._type != dsc_ast2.AstType.TOKEN:
                continue
            if sub_child._token._data != "CommonPCH.h":
                continue
            sub_child._token._data = "common/common_pch.h"
    return False

def AdjustInclude(in_path_spec):
    in_path_spec = in_path_spec.replace("\\", "/")
    result = ""
    prev_c = "/"
    for c in in_path_spec:
        if c.isupper():
            if prev_c != "/":
                result += "_"
            c = c.lower()
        result += c
        prev_c = c
    return result

def GetIncludePathSpec(in_ast_node):
    if in_ast_node._type != dsc_ast2.AstType.PREPROCESSOR_INCLUDE:
        return ""
    for child in in_ast_node._children:
        if child._type != dsc_ast2.AstType.TOKEN:
            continue
        return child._token._data
    return ""

def SetIncludePathSpec(in_ast_node, in_path_spec):
    if in_ast_node._type != dsc_ast2.AstType.PREPROCESSOR_INCLUDE:
        return ""
    for child in in_ast_node._children:
        if child._type != dsc_ast2.AstType.TOKEN:
            continue
        child._token._data = in_path_spec

def AstTransformInclude(in_ast_node, in_stack):
    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "pch.h" in path_spec:
            continue
        new_path = AdjustInclude(path_spec)
        SetIncludePathSpec(child, new_path)
    return False

def AstTransformSortInclude(in_ast_node, in_stack):
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
        new_first_char = GetIncludePathSpec(child)[:1]
        if first_char != new_first_char:
            first_char = new_first_char
            new_child_array.append(dsc_ast2.AST(
                dsc_ast2.AstType.WHITE_SPACE, 
                dsc_token2.Token("\n", dsc_token2.TokenType.WHITE_SPACE)
                ))
        new_child_array.append(child)

    if 0 != len(array_to_sort):
        new_child_array.append(dsc_ast2.AST(
            dsc_ast2.AstType.WHITE_SPACE, 
            dsc_token2.Token("\n", dsc_token2.TokenType.WHITE_SPACE)
            ))

    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "" != path_spec:
            continue 
        new_child_array.append(child)

    in_ast_node._children = new_child_array

    return False


class ClassStructVariableType(enum.Enum):
    NONE = 0
    MEMBER_DECLARATION = 1
    MEMBER_INSTANCE = 2
    METHOD_DECLARATION = 3
    METHOD_DEFINITION = 4
    METHOD_PARAM = 5
    DECLARATION = 6

def AstTransformRenameVariables(in_ast_node, in_stack):


    return True

def TransformAst(in_ast):
    in_ast.Visit(AstTransformRenamePCH)
    in_ast.Visit(AstTransformInclude)
    in_ast.Visit(AstTransformSortInclude)
    in_ast.Visit(AstTransformRenameVariables)
    #in_ast.Visit(AstTransformSortClassMethodsMembers)

    in_ast.Dump()