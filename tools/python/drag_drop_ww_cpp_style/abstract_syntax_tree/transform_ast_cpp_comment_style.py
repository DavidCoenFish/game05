from . import dsc_token_cpp
from . import dsc_ast_cpp

# "//foobar" => "// Foobar"
def AstTransformCommentStyle(in_ast_node, in_stack_ast_node, in_data):
    if in_ast_node._type != dsc_ast_cpp.AstType.COMMENT:
        return True

    if in_ast_node._token._sub_type == dsc_token_cpp.CommentType.END_OF_LINE:
        data = in_ast_node._token._data[2:].strip()
        new_data = data[:1].upper() + data[1:]
        in_ast_node._token._data = in_ast_node._token._data[:2] + " " + new_data

    if in_ast_node._token._sub_type == dsc_token_cpp.CommentType.MULTI_LINE:
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
