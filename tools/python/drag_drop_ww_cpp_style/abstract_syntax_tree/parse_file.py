from . import dsc_token
from . import dsc_ast


def LoadFile(in_file_path):
    print(f"LoadFile:{in_file_path}")
    file = open(in_file_path, "r")
    data = file.read()
    file.close()

    token_array = dsc_token.TokenizeData(data)

    ast = dsc_ast.MakeAst(token_array)

    ast.Dump()

    return ast