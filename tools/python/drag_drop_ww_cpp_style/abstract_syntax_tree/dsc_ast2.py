import enum

class AstAccess(enum.Enum):
    NONE = 0
    PUBLIC = 1
    PROTECTED = 2
    PRIVATE = 3

class AstType(enum.Enum):
    NONE = 0
    DOCUMENT = 1
    COMMENT = 2
    PREPROCESSOR_PRAGMA = 3
    PREPROCESSOR_INCLUDE = 4
    #PREPROCESSOR_PCH = 4
    PREPROCESSOR_CONDITION = 5
    PREPROCESSOR_DEFINITION = 6
    #CLASS_FORWARD = 7
    #CLASS_DECLARATION = 8
    SCOPE = 9
    SCOPE_END = 10
    STATEMENT = 11
    STATEMENT_END = 12
    WHITE_SPACE = 13
    TOKEN = 14
    PARENTHESIS = 15
    PARENTHESIS_END = 16
    #METHOD_IMPLEMENTATION = 17

def CreateNewNode(in_stack, in_child_array, in_type, in_token, in_access):
    node = AST(in_type, in_token, in_access)
    in_stack.append(node)
    in_child_array.append(node)

def CreateNewChild(in_child_array, in_type, in_token, in_access = AstAccess.NONE):
    node = AST(in_type, in_token, in_access)
    in_child_array.append(node)

class AST:
    def __init__(self, in_type = AstType.NONE, in_token = None, in_access = AstAccess.NONE):
        self._type = in_type
        self._token = in_token
        self._access = in_access
        self._children = []

    def AddToken(self, in_stack, in_token):
        if in_token._data == "#pragma":
            CreateNewNode(in_stack, self._children, AstType.PREPROCESSOR_PRAGMA, in_token, self._access)
            return
        if in_token._data == "#include":
            CreateNewNode(in_stack, self._children, AstType.PREPROCESSOR_INCLUDE, in_token, self._access)
            return
        if in_token._data == "public:":
            self._access = AstAccess.PUBLIC
            return
        if in_token._data == "protected:":
            self._access = AstAccess.PROTECTED
            return
        if in_token._data == "private:":
            self._access = AstAccess.PRIVATE
            return
        if in_token._data == "{":
            CreateNewNode(in_stack, self._children, AstType.SCOPE, in_token, self._access)
            return
        if in_token._data == "}":
            CreateNewChild(self._children, AstType.SCOPE_END, in_token)
            in_stack.pop()
            if in_stack[-1]._type == AstType.STATEMENT:
                in_stack.pop()
            return
        if in_token._data == "(":
            CreateNewNode(in_stack, self._children, AstType.PARENTHESIS, in_token, self._access)
            return
        if in_token._data == ")":
            CreateNewChild(self._children, AstType.PARENTHESIS_END, in_token)
            in_stack.pop()
            return

        if in_token._data == ";":
            if self._type == AstType.STATEMENT:
                CreateNewChild(self._children, AstType.STATEMENT_END, in_token)
                in_stack.pop()
            return

        if in_token.IsComment():
            CreateNewNode(in_stack, self._children, AstType.COMMENT, in_token, self._access)
            in_stack.pop()
            return

        if (
            self._type == AstType.PREPROCESSOR_PRAGMA or
            self._type == AstType.PREPROCESSOR_INCLUDE
            ):
            if "\n" in in_token._data:
                in_stack.pop()
                return
            type = AstType.TOKEN
            if True == in_token.IsWhiteSpace():
                type = AstType.WHITE_SPACE
            CreateNewChild(self._children, type, in_token)
            return
        
        if self._type == AstType.PARENTHESIS:
            type = AstType.TOKEN
            if True == in_token.IsWhiteSpace():
                type = AstType.WHITE_SPACE
            CreateNewChild(self._children, type, in_token)
            return

        if in_token.IsWhiteSpace():
            if self._type == AstType.STATEMENT:
                CreateNewChild(self._children, AstType.WHITE_SPACE, in_token)
            return

        #start a statement if we get this far, but
        if self._type == AstType.STATEMENT:
            CreateNewChild(self._children, AstType.TOKEN, in_token)
        else:
            CreateNewNode(in_stack, self._children, AstType.STATEMENT, in_token, self._access)

        return

    def Dump(self, in_depth = 0):
        line = ("    " * in_depth) + f"type:\"{self._type.name}\""
        if self._access != AstAccess.NONE:
            line += f" access:\"{self._access.name}\""
        if self._token and self._token._data != "":
            data = self._token._data.replace("\n","\\n")
            line += f" data:\"{data}\""
        print(line)
        for child in self._children:
            child.Dump(in_depth + 1)

    # The visitor should return False to stop visiting each ast node
    def Visit(self, in_visitor, in_stack = []):
        keep_going = in_visitor(self, in_stack)
        if False == keep_going:
            return keep_going
        in_stack.append(self)
        for child in self._children:
            keep_going = child.Visit(in_visitor, in_stack)
            if False == keep_going:
                break
        in_stack.pop()
        return keep_going

    def MakeString(self, in_depth = 0):
        tab = "    "
        new_line = "\n"
        result = ""
        child_depth = in_depth
        return result

def MakeAst(in_token_array):
    result = AST(AstType.DOCUMENT)
    stack = [result]
    for token in in_token_array:
        stack[-1].AddToken(stack, token)

    return result


