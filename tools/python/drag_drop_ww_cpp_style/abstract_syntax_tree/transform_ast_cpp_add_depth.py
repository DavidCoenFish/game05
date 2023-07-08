from . import dsc_token_cpp
from . import dsc_ast_cpp

def MakeNewDepthIncrement():
    return dsc_ast_cpp.AST(dsc_ast_cpp.AstType.OUTPUT_DEPTH_INCREMENT)

def MakeNewDepthDecrement():
    return dsc_ast_cpp.AST(dsc_ast_cpp.AstType.OUTPUT_DEPTH_DECREMENT)

def AstTransformAddDepth(in_ast_node, in_stack_ast_node, in_data):
    if (
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_CLASS or
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT
        ):
        for child in in_ast_node._children:
            if child._type != dsc_ast_cpp.AstType.SCOPE:
                continue
            new_child_array = [MakeNewDepthIncrement()]
            for sub_child in child._children:
                if sub_child._type == dsc_ast_cpp.AstType.SCOPE_END:
                    new_child_array.append(MakeNewDepthDecrement())
                new_child_array.append(sub_child)
            child._children = new_child_array

    return True
