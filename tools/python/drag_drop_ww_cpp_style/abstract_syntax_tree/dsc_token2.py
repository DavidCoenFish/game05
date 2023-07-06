import enum

class TokenType(enum.Enum):
    NONE = 0
    COMMENT_END_LINE = 1
    COMMENT_MULTI_LINE = 2
    SPECIAL_SYMBOL = 3
    STRING = 4
    TOKEN = 5
    STATEMENT_END = 6
    WHITE_SPACE = 7
    SCOPE_RESOLUTION_OPERATOR = 8
    INCLUDE_PATH_SPEC = 9

#s_special_symbols = set(("[","]","(",")","{","}",",","#","*","~",".", "'","?","<",">",";","&","/","+","-", "!", "%", "^", "|","=",":", "\\"))
s_special_symbols = set(("[","]","(",")","{","}",",","*","~",".", "'","?","<",">",";","&","/","+","-", "!", "%", "^", "|","=", "\\"))

def IsEscapedStringComplete(in_string):
    prev_was_escape = False
    last_was_true_quote = False
    for c in in_string[1:]:
        if True == prev_was_escape:
            prev_was_escape = False
        elif c == "\\":
            prev_was_escape = True
        elif c == "\"":
            last_was_true_quote = True

    return last_was_true_quote

def CalculateCouldBeIncludePathSpec(in_array_tokens, in_c):
    if in_c != "<":
        return False
    if 1 <= len(in_array_tokens):
        if in_array_tokens[-1]._data == "#include":
            return True
    if 2 <= len(in_array_tokens):
        if in_array_tokens[-2]._data == "#include":
            return True
    return False

class Token:
    def __init__(self, in_data = "", in_type = TokenType.NONE):
        self._data = in_data
        self._type = in_type

    def __str__(self):
        data = self._data.replace("\n", "\\n")
        return f"The token data is ({data}) for type ({self._type.name})"

    def __repr__(self):
        return f"Token(\'{self._data}\', \'{self._type.name}\')"

    def IsComment(self):
        return self._type == TokenType.COMMENT_END_LINE or self._type == TokenType.COMMENT_MULTI_LINE

    def IsWhiteSpace(self):
        return self._type == TokenType.WHITE_SPACE

    def DealChar(self, in_array_tokens, in_prev_c, in_c):
        trace_self = self
        if trace_self._data[:1] == "\"":
            trace_self._type = TokenType.STRING
            trace_self._data += in_prev_c
            if True == IsEscapedStringComplete(trace_self._data):
                #trim off the quotes
                trace_self._data = trace_self._data[1:]
                trace_self._data = trace_self._data[:-1]
                in_array_tokens.append(Token())
            return

        could_be_include_path_spec = CalculateCouldBeIncludePathSpec(in_array_tokens, in_prev_c)
        if True == could_be_include_path_spec:
            if trace_self._data != "":
                in_array_tokens.append(Token("", TokenType.INCLUDE_PATH_SPEC))
            else:
                trace_self._type = TokenType.INCLUDE_PATH_SPEC
                trace_self._data = in_prev_c
            return
        
        if trace_self._type == TokenType.INCLUDE_PATH_SPEC:
            if in_prev_c == ">":
                in_array_tokens.append(Token())
                return
            trace_self._data += in_prev_c

        char_pair = in_prev_c + in_c

        if char_pair == "//":
            if trace_self._data != "":
                in_array_tokens.append(Token(in_prev_c, TokenType.COMMENT_END_LINE))
            else:
                trace_self._type = TokenType.COMMENT_END_LINE
                trace_self._data = in_prev_c
            return

        if trace_self._type == TokenType.COMMENT_END_LINE:
            if in_prev_c == "\n":
                in_array_tokens.append(Token("\n", TokenType.WHITE_SPACE))
                return
            trace_self._data += in_prev_c
            return

        if char_pair == "/*":
            if trace_self._data != "":
                in_array_tokens.append(Token(in_prev_c, TokenType.COMMENT_MULTI_LINE))
            else:
                trace_self._type = TokenType.COMMENT_MULTI_LINE
                trace_self._data = in_prev_c
            return

        if trace_self._type == TokenType.COMMENT_MULTI_LINE:
            trace_self._data += in_prev_c
            if trace_self._data[-2:] == "*/":
                in_array_tokens.append(Token())
            return
        
        if char_pair == "::":
            if trace_self._data != "":
                in_array_tokens.append(Token(in_prev_c, TokenType.SCOPE_RESOLUTION_OPERATOR))
            else:
                trace_self._type = TokenType.SCOPE_RESOLUTION_OPERATOR
                trace_self._data = in_prev_c
            return
        
        if trace_self._type == TokenType.SCOPE_RESOLUTION_OPERATOR:
            trace_self._data += in_prev_c
            in_array_tokens.append(Token())
            return

        if in_prev_c == ";":
            if trace_self._data == "":
                trace_self._data = ";"
                trace_self._type = TokenType.STATEMENT_END
            else:
                in_array_tokens.append(Token(";", TokenType.STATEMENT_END))
            in_array_tokens.append(Token())
            return

        c_is_space = in_prev_c.isspace()
        data_is_space = trace_self._data.isspace()
        if c_is_space == True:
            if trace_self._data == "" or True == data_is_space:
                trace_self._data += in_prev_c
                trace_self._type = TokenType.WHITE_SPACE
                return
            in_array_tokens.append(Token(in_prev_c, TokenType.WHITE_SPACE))
            return
        elif data_is_space == True:
            self._type = TokenType.WHITE_SPACE
            trace_self = Token()
            in_array_tokens.append(trace_self)
            # drop through to use following "in_prev_c in s_special_symbols" ie, we have yet to add in_prev_c, so no return

        if in_prev_c in s_special_symbols:
            if trace_self._data == "":
                trace_self._data = in_prev_c
                trace_self._type = TokenType.SPECIAL_SYMBOL
            else:
                in_array_tokens.append(Token(in_prev_c, TokenType.SPECIAL_SYMBOL))
            in_array_tokens.append(Token())
            return

        trace_self._data += in_prev_c
        trace_self._type = TokenType.TOKEN

        return

# def CleanUp(in_token_array):
#     token_array = [in_token_array[:1]]
#     for token in in_token_array[1:]:

def TokenizeData(in_data):
    token_array = [Token()]
    data = in_data + "\n"

    prev_c = in_data[:1]
    for c in in_data[1:]:
        token_array[-1].DealChar(token_array, prev_c, c)
        prev_c = c

    if token_array[-1]._data == "":
        token_array.pop() #[:-1]

    #token_array = CleanUp(token_array)

    #for token in token_array:
    #    print(str(token))

    return token_array
