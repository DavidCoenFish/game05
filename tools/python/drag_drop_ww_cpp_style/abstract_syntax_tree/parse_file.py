from . import dsc_token_cpp
from . import dsc_ast_cpp


def LoadFile(in_file_path, in_debug = False):
    print(f"LoadFile:{in_file_path}")
    file = open(in_file_path, "r")
    data = file.read()
    file.close()

    token_array = dsc_token_cpp.TokenizeData(data, in_debug)
    ast = dsc_ast_cpp.MakeAst(token_array, in_debug)

    return ast