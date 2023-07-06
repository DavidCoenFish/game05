#from . import dsc_token
from . import dsc_token2
#from . import dsc_ast
from . import dsc_ast2


def LoadFile(in_file_path):
    print(f"LoadFile:{in_file_path}")
    file = open(in_file_path, "r")
    data = file.read()
    file.close()

    token_array = dsc_token2.TokenizeData(data)

    ast = dsc_ast2.MakeAst(token_array)

    #ast.Dump()

    return ast