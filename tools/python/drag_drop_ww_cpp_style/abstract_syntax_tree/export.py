from . import output_file
from . import dsc_ast_cpp
from . import dsc_token_cpp
from . import output_file

import enum

class ExportFormat(enum.Enum):
    NONE = 0
    NEW_LINE = 1
    DEPTH_INCREMENT = 2
    DEPTH_DECREMENT = 3
    WHITE_SPACE = 4

def PerformFormatting(in_array_format, in_output):
    for item in in_array_format:
        if item == ExportFormat.NEW_LINE:
             in_output.NewLine()
        elif item == ExportFormat.DEPTH_INCREMENT:
             in_output.IncrementDepth()
        elif item == ExportFormat.DEPTH_DECREMENT:
             in_output.DecrementDepth()
        elif item == ExportFormat.WHITE_SPACE:
             in_output.AddTokkenToCurrent(" ")


def BuildOutput(in_ast_node, in_output):
    PerformFormatting(in_ast_node._export_pre_token_format, in_output)

    if in_ast_node._type == dsc_ast_cpp.AstType.COMMENT:
        if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_END_LINE:
            in_output.AddEndLineComment(in_ast_node._token._data)
        if in_ast_node._token._type == dsc_token_cpp.TokenType.COMMENT_MULTI_LINE:
           in_output.AddMultiLineComment(in_ast_node._token._data)
    elif in_ast_node._type == dsc_ast_cpp.AstType.WHITE_SPACE:
        in_output.AddTokkenToCurrent(" ")
    elif in_ast_node._token != None:
        in_output.AddTokkenToCurrent(in_ast_node._token._data)

    PerformFormatting(in_ast_node._export_post_token_format, in_output)

    for child in in_ast_node._children:
        BuildOutput(child, in_output)

    PerformFormatting(in_ast_node._export_post_children_format, in_output)

    return

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
