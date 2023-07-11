from . import dsc_token_cpp
from . import dsc_ast_cpp

# CommonPCH.h => common/common_pch.h
def AstTransformRenamePCH(in_ast_node, in_stack_ast_node, in_data):
    for child in in_ast_node._children:
        if (
            child._type != dsc_ast_cpp.AstType.PREPROCESSOR and
            child._sub_type != dsc_ast_cpp.SubType.PREPROCESSOR_INCLUDE
            ):
            continue
        for sub_child in child._children:
            if sub_child._token._type != dsc_token_cpp.TokenType.INCLUDE_PATH_SPEC:
                continue
            if sub_child._token._data != "\"CommonPCH.h\"":
                continue
            sub_child._token._data = "\"common/common_pch.h\""
    return False
