import enum

from . import dsc_token_cpp
from . import dsc_ast_cpp

class ClassStructVariableType(enum.Enum):
    NONE = 0
    MEMBER_DECLARATION = 1
    MEMBER_INSTANCE = 2
    METHOD_DECLARATION = 3
    METHOD_DEFINITION = 4
    METHOD_PARAM = 5
    DECLARATION = 6


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

# "foo bar" => "Foo bar"
def AstTransformCommentStyle(in_ast_node, in_stack_ast_node, in_data):
    if in_ast_node._type != dsc_ast_cpp.AstType.COMMENT:
        return True

    if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_END_LINE:
        data = in_ast_node._token._data[2:].strip()
        new_data = data[:1].upper() + data[1:]
        in_ast_node._token._data = in_ast_node._token._data[:2] + " " + new_data

    if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_MULTI_LINE:
        data = in_ast_node._token._data[2:]
        data = data[:-2]
        split_data = data.split("\n")
        new_data = ""
        for line in split_data:
            if line == "":
                continue
            if new_data != "":
                new_data += "\n"
            new_data += line.strip()
        in_ast_node._token._data = "/*" + new_data + "*/"

    return True

# CommonPCH.h => common/common_pch.h
def AstTransformRenamePCH(in_ast_node, in_stack_ast_node, in_data):
    for child in in_ast_node._children:
        if child._type != dsc_ast_cpp.AstType.PREPROCESSOR_INCLUDE:
            continue
        for sub_child in child._children:
            if sub_child._type != dsc_ast_cpp.AstType.TOKEN:
                continue
            if sub_child._token._data != "CommonPCH.h":
                continue
            sub_child._token._data = "common/common_pch.h"
    return False

# "Common/DrawSystem/DrawSystem.h" => "common/draw_system/draw_system.h"
def AstTransformInclude(in_ast_node, in_stack_ast_node, in_data):
    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "pch.h" in path_spec:
            continue
        new_path = AdjustInclude(path_spec)
        SetIncludePathSpec(child, new_path)
    return False

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
        new_first_char = GetIncludePathSpec(child)[:1]
        if first_char != new_first_char:
            first_char = new_first_char
            new_child_array.append(dsc_ast_cpp.AST(
                dsc_ast_cpp.AstType.WHITE_SPACE, 
                dsc_token_cpp.Token("\n", dsc_token_cpp.TokenType.WHITE_SPACE)
                ))
        new_child_array.append(child)

    if 0 != len(array_to_sort):
        new_child_array.append(dsc_ast_cpp.AST(
            dsc_ast_cpp.AstType.WHITE_SPACE, 
            dsc_token_cpp.Token("\n", dsc_token_cpp.TokenType.WHITE_SPACE)
            ))

    for child in in_ast_node._children:
        path_spec = GetIncludePathSpec(child)
        if "" != path_spec:
            continue 
        new_child_array.append(child)

    in_ast_node._children = new_child_array

    return False

# rename members "m_pCommandList" => "_command_list"
# rename method param "pCommandList" => "in_command_list"
# rename local members "swapChainDesc" => "swap_chain_desc"
# for (auto pIter : m_listResource) => for (auto iter : _list_resource)
def AstTransformRenameVariables(in_ast_node, in_stack_ast_node, in_data):


    return True

def TransformAst(in_ast):
    in_ast.Visit(AstTransformCommentStyle)
    in_ast.Visit(AstTransformRenamePCH)
    in_ast.Visit(AstTransformInclude)
    in_ast.Visit(AstTransformSortInclude)
    in_ast.Visit(AstTransformRenameVariables)
    #in_ast.Visit(AstTransformSortClassMethodsMembers)

    print("post transform abstract syntax tree")
    in_ast.Dump()