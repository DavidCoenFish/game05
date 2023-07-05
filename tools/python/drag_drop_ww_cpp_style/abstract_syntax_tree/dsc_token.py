import enum

class ParseState(enum.Enum):
    NONE = 0
    COMMENT_END_LINE = 2
    COMMENT_MULTI_LINE = 3
    COMMENT_MULTI_LINE_TERMINATE = 4
    SPECIAL_SYMBOL = 5
    STRING = 6
    PRAGMA = 7
    INCLUDE = 8
    SCOPE_RESOLUTION = 9

s_special_symbols = set(("[","]","(",")","{","}",",","*","~",".",  "<",">",";","&"))

class Token:
    def __init__(self, in_data = ""):
        self._data = in_data

    def AddChar(self, in_state, in_c): 
        if in_state == ParseState.NONE:
            is_space = in_c.isspace()
            if is_space == True:
                if self._data == "":
                    return ParseState.NONE, False
                return ParseState.NONE, True
            
            self._data += in_c

            if in_c == "\"":
                return ParseState.STRING, True
            if self._data == "/*":
                return ParseState.COMMENT_MULTI_LINE, True

            if self._data == "//":
                return ParseState.COMMENT_END_LINE, True

            if self._data == "#pragma":
                return ParseState.PRAGMA, True

            if self._data == "#include":
                return ParseState.INCLUDE, True
            
            if self._data[-2:] == "::":
                self._data = self._data[:-2]
                return ParseState.SCOPE_RESOLUTION, True
            
            if in_c in s_special_symbols:
                self._data = self._data[:-1]
                return ParseState.SPECIAL_SYMBOL, True

            return ParseState.NONE, False

        if in_state == ParseState.COMMENT_MULTI_LINE:
            self._data += in_c
            if self._data[-2:] == "*/":
                self._data = self._data[:-2]
                return ParseState.COMMENT_MULTI_LINE_TERMINATE, True
            return ParseState.COMMENT_MULTI_LINE, False

        if in_state == ParseState.STRING:
            self._data += in_c
            if self._data[-1:] == "\"":
                return ParseState.NONE, True
            return ParseState.STRING, False

        if in_state == ParseState.COMMENT_END_LINE:
            if in_c == "\n" or in_c == "\r":
                return ParseState.NONE, True
            self._data += in_c
            return ParseState.COMMENT_END_LINE, False

        if in_state == ParseState.PRAGMA:
            if in_c == "\n" or in_c == "\r":
                return ParseState.NONE, True
            self._data += in_c
            return ParseState.PRAGMA, False

        if in_state == ParseState.INCLUDE:
            if in_c == "\n" or in_c == "\r":
                self._data = self._data.strip()
                return ParseState.NONE, True
            self._data += in_c
            return ParseState.INCLUDE, False

def TokenizeData(in_data):
    result = []
    trace_token = Token()
    state = ParseState.NONE
    for c in in_data:
        state, needs_new = trace_token.AddChar(state, c)
        if True == needs_new:
            if trace_token._data != "":
                result.append(trace_token)
            if state == ParseState.SPECIAL_SYMBOL:
                result.append(Token(c))
                state = ParseState.NONE
            elif state == ParseState.COMMENT_MULTI_LINE_TERMINATE:
                #result.append(Token("*/"))
                state = ParseState.NONE
            elif state == ParseState.SCOPE_RESOLUTION:
                result.append(Token("::"))
                state = ParseState.NONE
            trace_token = Token()

    #for item in result:
    #    print(f"  token({item._data})")

    return result 
