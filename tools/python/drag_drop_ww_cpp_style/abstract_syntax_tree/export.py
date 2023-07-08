from . import output_file
from . import dsc_ast_cpp
from . import dsc_token_cpp

def BuildOutput(in_ast_node, in_output):
    if in_ast_node._type == dsc_ast_cpp.AstType.NEW_LINE:
        in_output.NewLine()
    elif in_ast_node._type == dsc_ast_cpp.AstType.COMMENT:
        if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_END_LINE:
            in_output.AddEndLineComment(in_ast_node._token._data)
        if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_MULTI_LINE:
            in_output.AddMultiLineComment(in_ast_node._token._data)
    elif in_ast_node._type == dsc_ast_cpp.AstType.OUTPUT_DEPTH_INCREMENT:
        in_output.IncrementDepth()
    elif in_ast_node._type == dsc_ast_cpp.AstType.OUTPUT_DEPTH_DECREMENT:
        in_output.DecrementDepth()
    elif in_ast_node._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER:
        in_output.DecrementDepth()
        in_output.AddTokkenToCurrent(in_ast_node._token._data)
    elif in_ast_node._type == dsc_ast_cpp.AstType.WHITE_SPACE:
        in_output.AddTokkenToCurrent(" ")
    elif in_ast_node._token and "" != in_ast_node._token._data:
        in_output.AddTokkenToCurrent(in_ast_node._token._data)


    for child in in_ast_node._children:
        BuildOutput(child, in_output)

    if in_ast_node._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER:
        in_output.IncrementDepth()

    # if in_ast_node._type == dsc_ast_cpp.AstType.COMMENT:
    #     if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_END_LINE:
    #         in_output.AddEndLineComment(in_ast_node._token._data)
    #     if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_MULTI_LINE:
    #         in_output.AddMultiLineComment(in_ast_node._token._data)
    # elif in_ast_node._type == dsc_ast_cpp.AstType.SCOPE:
    #     in_output.NewLineIfNeeded()
    #     in_output.AddTokkenToCurrent(in_ast_node._token._data)
    #     in_output.IncrementDepth()
    #     in_output.NewLine()
    # elif in_ast_node._type == dsc_ast_cpp.AstType.SCOPE_END:
    #     in_output.DecrementDepth()
    #     in_output.NewLine()
    #     in_output.AddTokkenToCurrent(in_ast_node._token._data)
    #     in_output.NewLine()
    # elif in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT_END:
    #     in_output.AddTokkenToCurrent(in_ast_node._token._data)
    #     in_output.NewLine()
    # elif in_ast_node._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER:
    #     in_output.DecrementDepth()
    #     in_output.NewLine()
    #     in_output.AddTokkenToCurrent(in_ast_node._token._data)
    #     in_output.IncrementDepth()
    #     in_output.NewLine()
    # elif in_ast_node._token:
    #     in_output.AddTokkenToCurrent(in_ast_node._token._data)

def MakeString(in_ast):
    output = output_file.OutputFile()
    BuildOutput(in_ast, output)
    return output.GetData()

def SaveFile(in_ast,in_file_path):
    print(f"SaveFile:{in_file_path}")

    data = MakeString(in_ast)

    text_file = open(in_file_path, "w")
    n = text_file.write(data)
    text_file.close()

    return
