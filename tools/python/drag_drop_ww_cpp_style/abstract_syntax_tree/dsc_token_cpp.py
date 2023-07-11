import enum

class TokenType(enum.Enum):
    NONE = 0
    COMMENT = 1
    WHITE_SPACE = 2 # newline, space, tab
    INCLUDE_PATH_SPEC = 3 # "foo.h" or <foo.h>
    STRING_LITERAL = 4 # ["foo", ...
    CHARACTER_LITERAL = 5 # ['\n', ...
    KEYWORD = 6 # [ "for", "while", "const", "public:", ...
    OPERATOR = 7 # [ "->", "<<=", "+", ".", ",", ...
    PREPROCESSOR = 8 # [ #include, #ifdef, #if, #else, #endif, #pragma, #define, ...
    TOKEN = 9 # class name, function name, member name

class CommentType(enum.Enum):
    NONE = 0
    END_OF_LINE = 1
    MULTI_LINE = 2

class OperatorType(enum.Enum):
    NONE = 0
    ADDITION = 1 # +
    AND = 2 # &&
    AND_EQ = 3 # &= 
    ARRAY_INDEX_END = 4 # ]
    ARRAY_INDEX_START = 5 # [
    ARROW_OPERATOR = 6 # ->
    ASSIGNMENT = 7 # =
    BITAND = 8 # &
    PREFIX_REFERENCE = 9 # &
    PREFIX_DEREFERENCE = 10 # *
    BITOR = 11 # |
    COMMA = 12 # ,
    DECREMENT = 13 # --
    DECREMENT_EQ = 14 # -=
    DIVISION = 15 # /
    EQUAL_TO = 16 # ==
    GREATER_THAN = 17 # >
    GREATER_THAN_EQ = 18 # >=
    INCREMENT = 19 # ++
    INCREMENT_EQ = 20 # +=
    INLINE_CONDITIONAL = 21 # ?
    LEFT_EQ = 22 # <<=
    LEFT_SHIFT = 23 # <<
    LESS_THAN = 24 # <
    LESS_THAN_EQ = 25 # <=
    MEMBER_ACCESS = 26 # .
    MODULO = 27 # %
    MULTIPLICATION = 28 # *
    NOT = 29 # !
    NOT_EQ = 30 # !=
    OR = 31 # ||
    OR_EQ = 32 # |=
    PARENTHISIS_END = 33 # )
    PARENTHISIS_START = 34 # (
    RIGHT_EQ = 35 # >>=
    RIGHT_SHIFT = 36 # >>
    SCOPE_END = 37 # }
    SCOPE_RESOLUTION_OPERATOR = 38 # ::
    SCOPE_START = 39 # {
    STATEMENT_END = 40 # ;
    SUBTRACTION = 41 # -
    XOR = 42 # ^
    XOR_EQ = 43 # ^=
    COLON = 44 # :
    HASH = 45 # #

class KeywordType(enum.Enum):
    NONE = 0
    ALIGNAS = 1 # alignas
    ALIGNOF = 2 # alignof
    ASM = 3 # asm
    AUTO = 4 # auto
    BOOL = 5 # bool
    BREAK = 6 # break
    CASE = 7 # case
    CATCH = 8 # catch
    CHAR = 9 # char
    CHAR16_T = 10 # char16_t
    CHAR32_T = 11 # char32_t
    CHAR8_T = 12 # char8_t
    CLASS = 13 # class
    COMPL = 14 # compl
    CONST = 15 # const
    CONST_CAST = 16 # const_cast
    CONSTEVAL = 17 # consteval
    CONSTEXPR = 18 # constexpr
    CONSTINIT = 19 # constinit
    CONTINUE = 20 # continue
    DECLTYPE = 21 # decltype
    DEFAULT = 22 # default
    DELETE = 23 # delete
    DO = 24 # do
    DOUBLE = 25 # double
    DYNAMIC_CAST = 26 # dynamic_cast
    ELSE = 27 # else
    ENUM = 28 # enum
    EXPLICIT = 29 # explicit
    EXPORT = 30 # export
    EXTERN = 31 # extern
    FALSE = 32 # false
    FLOAT = 33 # float
    FOR = 34 # for
    FRIEND = 35 # friend
    GOTO = 36 # goto
    IF = 37 # if
    INLINE = 38 # inline
    INT = 39 # int
    LONG = 40 # long
    MUTABLE = 41 # mutable
    NAMESPACE = 42 # namespace
    NEW = 43 # new
    NOEXCEPT = 44 # noexept
    NULLPTR = 45 # nullptr
    OPERATOR = 46 # operator
    PRIVATE = 47 # private:
    PROTECTED = 48 # protected:
    PUBLIC = 49 # public
    REGISTER = 50 # register
    REINTERPRET_CAST = 51 # reinterpret_cast
    RETURN = 52 # return
    SHORT = 53 # short
    SIZEOF = 54 # sizeof
    STATIC_ASSERT = 55 # static_assert
    STATIC_CAST = 56 # static_cast
    STRUCT = 57 # struct
    SWITCH = 58 # switch
    TEMPLATE = 59 # template
    THIS = 60 # this
    THREAD_LOCAL = 61 # thread_local
    THROW = 62 # throw
    TRUE = 63 # true
    TRY = 64 # try
    TYPEDEF = 65 # typedef
    TYPENAME = 66 # typename
    UNION = 67 # union
    UNSIGNED = 68 # unsigned
    USING = 69 # using
    VIRTUAL = 70 # virtual
    VOID = 71 # void
    VOLATILE = 72 # volatile
    WCHAR_T = 73 # wchar_t
    WHILE = 74 # while

class PreprocessorType(enum.Enum):
    NONE = 0
    INCLUDE = 1 
    IFDEF = 2
    IF = 3
    ELIF = 4
    ELSE = 5
    ENDIF = 6
    PRAGMA = 7
    DEFINE = 8
    UNDEF = 9
    LINE = 10
    ERROR = 11


# ("#", ":") current not in special symbol set
#s_special_symbols = set(("[","]","(",")","{","}",",","*","~",".", "'","?","<",">",";","&","/","+","-", "!", "%", "^", "|","=", "\\"))

# assume meaning of "*","&","<<",">>" are cleaned up latter, note: template<foo<bar>>
s_operator_dict = {
    "+" : OperatorType.ADDITION,
    "&&" : OperatorType.AND,
    "&=" : OperatorType.AND_EQ, 
    "]" : OperatorType.ARRAY_INDEX_END,
    "[" : OperatorType.ARRAY_INDEX_START,
    "->" : OperatorType.ARROW_OPERATOR,
    "=" : OperatorType.ASSIGNMENT,
    "&" : OperatorType.BITAND,
#    "&" : OperatorType.PREFIX_REFERENCE = 9 # &
#    "*" : OperatorType.PREFIX_DEREFERENCE = 10 # *
    "|" : OperatorType.BITOR,
    "," : OperatorType.COMMA,
    "--" : OperatorType.DECREMENT,
    "-=" : OperatorType.DECREMENT_EQ,
    "/" : OperatorType.DIVISION,
    "==" : OperatorType.EQUAL_TO,
    ">" : OperatorType.GREATER_THAN,
    ">=" : OperatorType.GREATER_THAN_EQ,
    "++" : OperatorType.INCREMENT,
    "+=" : OperatorType.INCREMENT_EQ,
    "<<=" : OperatorType.LEFT_EQ,
    "<<" : OperatorType.LEFT_SHIFT,
    "<" : OperatorType.LESS_THAN,
    "<=" : OperatorType.LESS_THAN_EQ,
    "." : OperatorType.MEMBER_ACCESS,
    "%" : OperatorType.MODULO,
    "*" : OperatorType.MULTIPLICATION,
    "!" : OperatorType.NOT,
    "!=" : OperatorType.NOT_EQ,
    "||" : OperatorType.OR,
    "|=" : OperatorType.OR_EQ,
    ")" : OperatorType.PARENTHISIS_END,
    "(" : OperatorType.PARENTHISIS_START,
    ">>=" : OperatorType.RIGHT_EQ,
    ">>" : OperatorType.RIGHT_SHIFT,
    "}" : OperatorType.SCOPE_END,
    "::" : OperatorType.SCOPE_RESOLUTION_OPERATOR,
    "{" : OperatorType.SCOPE_START,
    ";" : OperatorType.STATEMENT_END,
    "-" : OperatorType.SUBTRACTION,
    "^" : OperatorType.XOR,
    "^=" : OperatorType.XOR_EQ,
    ":" : OperatorType.COLON,
    "#" : OperatorType.HASH,
}

s_keyword_dict = {
    "auto" : KeywordType.AUTO,
    "alignas" : KeywordType.ALIGNAS,
    "alignof" : KeywordType.ALIGNOF, 
    "asm" : KeywordType.ASM,
    "bool" : KeywordType.BOOL,
    "break" : KeywordType.BREAK,
    "case" : KeywordType.CASE,
    "catch" : KeywordType.CATCH,
    "char" : KeywordType.CHAR,
    "char8_t" : KeywordType.CHAR8_T,
    "char16_t" : KeywordType.CHAR16_T,
    "char32_t" : KeywordType.CHAR32_T,
    "class" : KeywordType.CLASS,
    "compl" : KeywordType.COMPL,
    "const" : KeywordType.CONST,
    "consteval" : KeywordType.CONSTEVAL,
    "constexpr" : KeywordType.CONSTEXPR,
    "constinit" : KeywordType.CONSTINIT,
    "continue" : KeywordType.CONTINUE,
    "const_cast" : KeywordType.CONST_CAST,
    "decltype" : KeywordType.DECLTYPE,
    "default" : KeywordType.DEFAULT,
    "delete" : KeywordType.DELETE,
    "do" : KeywordType.DO,
    "double" : KeywordType.DOUBLE,
    "dynamic_cast" : KeywordType.DYNAMIC_CAST,
    "else" : KeywordType.ELSE,
    "enum" : KeywordType.ENUM,
    "explicit" : KeywordType.EXPLICIT,
    "export" : KeywordType.EXPORT,
    "extern" : KeywordType.EXTERN,
    "false" : KeywordType.FALSE,
    "float" : KeywordType.FLOAT,
    "for" : KeywordType.FOR,
    "friend" : KeywordType.FRIEND,
    "goto" : KeywordType.GOTO,
    "if" : KeywordType.IF,
    "inline" : KeywordType.INLINE,
    "int" : KeywordType.INT,
    "long" : KeywordType.LONG,
    "mutable" : KeywordType.MUTABLE,
    "namespace" : KeywordType.NAMESPACE,
    "new" : KeywordType.NEW,
    "nullptr" : KeywordType.NULLPTR,
    "noexept" : KeywordType.NOEXCEPT,
    "operator" : KeywordType.OPERATOR,
    "private" : KeywordType.PRIVATE,
    "protected" : KeywordType.PROTECTED,
    "public" : KeywordType.PUBLIC,
    "register" : KeywordType.REGISTER,
    "return" : KeywordType.RETURN,
    "reinterpret_cast" : KeywordType.REINTERPRET_CAST,
    "static_assert" : KeywordType.STATIC_ASSERT,
    "static_cast" : KeywordType.STATIC_CAST,
    "short" : KeywordType.SHORT,
    "sizeof" : KeywordType.SIZEOF,
    "struct" : KeywordType.STRUCT,
    "switch" : KeywordType.SWITCH,
    "template" : KeywordType.TEMPLATE,
    "this" : KeywordType.THIS,
    "thread_local" : KeywordType.THREAD_LOCAL, 
    "throw" : KeywordType.THROW,
    "true" : KeywordType.TRUE,
    "try" : KeywordType.TRY,
    "typedef" : KeywordType.TYPEDEF,
    "typename" : KeywordType.TYPENAME,
    "union" : KeywordType.UNION,
    "unsigned" : KeywordType.UNSIGNED,
    "using" : KeywordType.USING,
    "virtual" : KeywordType.VIRTUAL,
    "void" : KeywordType.VOID,
    "volatile" : KeywordType.VOLATILE,
    "wchar_t" : KeywordType.WCHAR_T,
    "while" : KeywordType.WHILE,
}

s_preprocessor_dict = {
    "#include" : PreprocessorType.INCLUDE,
    "#ifdef" : PreprocessorType.IFDEF,
    "#if" : PreprocessorType.IF,
    "#elif" : PreprocessorType.ELIF,
    "#else" : PreprocessorType.ELSE,
    "#endif" : PreprocessorType.ENDIF,
    "#pragma" : PreprocessorType.PRAGMA,
    "#define" : PreprocessorType.DEFINE,
    "#undef" : PreprocessorType.UNDEF,
    "#line" : PreprocessorType.LINE,
    "#error" : PreprocessorType.ERROR,
    
}

def IsEscapedStringComplete(in_data):
    prev_was_escape = False
    last_was_true_quote = False
    for c in in_data[1:]:
        if True == prev_was_escape:
            prev_was_escape = False
        elif c == "\\":
            prev_was_escape = True
        elif c == "\"":
            last_was_true_quote = True

    return last_was_true_quote

def IsEscapedCharacterComplete(in_data):
    prev_was_escape = False
    last_was_true_quote = False
    for c in in_data[1:]:
        if True == prev_was_escape:
            prev_was_escape = False
        elif c == "\\":
            prev_was_escape = True
        elif c == "\'":
            last_was_true_quote = True

    return last_was_true_quote

def CalculateCouldBeIncludePathSpec(in_array_tokens, in_c):
    if in_c != "<" and in_c != "\"":
        return False
    if 1 <= len(in_array_tokens):
        if in_array_tokens[-1]._sub_type == PreprocessorType.INCLUDE:
            return True
    if 2 <= len(in_array_tokens):
        if in_array_tokens[-2]._sub_type == PreprocessorType.INCLUDE:
            return True
    return False

def ConsumeDict(in_trace_self, in_array_tokens, in_forward_string, in_dict, in_token_type):
    for key in in_dict.keys():
        if in_forward_string.startswith(key):
            value = in_dict[key]
            if in_trace_self._data != "":
                in_array_tokens.append(Token(key, in_token_type, value))
            else:
                in_trace_self._type = in_token_type
                in_trace_self._sub_type = value
                in_trace_self._data = key
            return len(key)
    return 0


class Token:
    def __init__(self, in_data = "", in_type = TokenType.NONE, in_sub_type = KeywordType.NONE):
        self._data = in_data
        self._type = in_type
        self._sub_type = in_sub_type

    def __str__(self):
        data = self._data.replace("\n", "\\n")
        return f"Token data ({data}) type ({self._type.name}) sub type ({self._sub_type.name})"

    def __repr__(self):
        data = self._data.replace("\n", "\\n")
        return f"Token(\'{data}\', \'{self._type.name}\, \'{self._sub_type.name}\')"

    def GetData(self):
        return self._data

    def SetData(self, in_data):
        self._data = in_data
        return

    def IsComment(self):
        return self._type == TokenType.COMMENT_END_LINE or self._type == TokenType.COMMENT_MULTI_LINE

    def IsWhiteSpace(self):
        return self._type == TokenType.WHITE_SPACE

    def DealChar(self, in_array_tokens, in_c, in_forward_string):
        trace_self = self

        # include path spec
        could_be_include_path_spec = CalculateCouldBeIncludePathSpec(in_array_tokens, in_c)
        if True == could_be_include_path_spec:
            if trace_self._data != "":
                in_array_tokens.append(Token(in_c, TokenType.INCLUDE_PATH_SPEC))
            else:
                trace_self._data += in_c
                trace_self._type = TokenType.INCLUDE_PATH_SPEC
            return 1

        # include path spec contents
        if trace_self._type == TokenType.INCLUDE_PATH_SPEC:
            trace_self._data += in_c
            if in_c == ">" or in_c == "\"":
                in_array_tokens.append(Token())
            return 1

        # String literal
        if trace_self._data[:1] == "\"":
            trace_self._type = TokenType.STRING_LITERAL
            trace_self._data += in_c
            if True == IsEscapedStringComplete(trace_self._data):
                in_array_tokens.append(Token())
            return 1

        # Character literal
        if trace_self._data[:1] == "\'":
            trace_self._type = TokenType.CHARACTER_LITERAL
            trace_self._data += in_c
            if True == IsEscapedCharacterComplete(trace_self._data):
                in_array_tokens.append(Token())
            return 1
        
        # Comment end of line
        if in_forward_string.startswith("//"):
            data = in_forward_string[:2]
            if trace_self._data != "":
                in_array_tokens.append(Token(data, TokenType.COMMENT, CommentType.END_OF_LINE))
            else:
                trace_self._type = TokenType.COMMENT
                trace_self._sub_type = CommentType.END_OF_LINE
                trace_self._data = data
            return 2

        # Comment end of line contents
        if trace_self._type == TokenType.COMMENT and trace_self._sub_type == CommentType.END_OF_LINE:
            if in_c == "\n":
                in_array_tokens.append(Token("\n", TokenType.WHITE_SPACE))
                return 1
            trace_self._data += in_c
            return 1

        # Comment multi line
        if in_forward_string.startswith("/*"):
            data = in_forward_string[:2]
            if trace_self._data != "":
                in_array_tokens.append(Token(data, TokenType.COMMENT, CommentType.MULTI_LINE))
            else:
                trace_self._type = TokenType.COMMENT
                trace_self._sub_type = CommentType.MULTI_LINE
                trace_self._data = data
            return 2

        # Comment multi line contents
        if trace_self._type == TokenType.COMMENT and trace_self._sub_type == CommentType.MULTI_LINE:
            trace_self._data += in_c
            if trace_self._data[-2:] == "*/":
                in_array_tokens.append(Token())
            return 1

        step = ConsumeDict(trace_self, in_array_tokens, in_forward_string, s_preprocessor_dict, TokenType.PREPROCESSOR)
        if 0 != step:
            return step
        step = ConsumeDict(trace_self, in_array_tokens, in_forward_string, s_operator_dict, TokenType.OPERATOR)
        if 0 != step:
            return step
        step = ConsumeDict(trace_self, in_array_tokens, in_forward_string, s_keyword_dict, TokenType.KEYWORD)
        if 0 != step:
            return step

        # Deal whitespace
        c_is_space = in_c.isspace()
        data_is_space = trace_self._data.isspace()
        if c_is_space == True:
            if trace_self._data == "" or True == data_is_space:
                trace_self._data += in_c
                trace_self._type = TokenType.WHITE_SPACE
                return 1
            in_array_tokens.append(Token(in_c, TokenType.WHITE_SPACE))
            return 1
        elif data_is_space == True:
            self._type = TokenType.WHITE_SPACE
            trace_self = Token()
            in_array_tokens.append(trace_self)
            # drop through to use following "in_c in s_special_symbols" ie, we have yet to add in_c, so no return

        trace_self._data += in_c
        trace_self._type = TokenType.TOKEN

        return 1

# startswith
def TokenizeData(in_data, in_debug = False):
    token_array = [Token()]

    index = 0
    while index < len(in_data):
        c = in_data[index]
        forward_string = in_data[index:]
        step = token_array[-1].DealChar(token_array, c, forward_string)
        if step <= 0:
            raise Exception("invalid state")
        index += step

    if 0 < len(token_array):
        if token_array[-1]._data == "":
            token_array.pop()

    if True == in_debug:
        reassembled_data = ""
        for token in token_array:
            print(str(token))
            reassembled_data += token._data

        if reassembled_data != in_data:
            print("==============( in_data )==========================")
            print(in_data)
            print("==============( reassembled data )=================")
            print(reassembled_data)
            print("===================================================")
            raise Exception("reassembled data from token array doesn't match the input data")

    return token_array
