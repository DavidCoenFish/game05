import enum
import copy
from . import dsc_token_cpp

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

    STATEMENT_CLASS = 19
    STATEMENT_STRUCT = 20
    STATEMENT_CONSTRUCTOR = 21
    STATEMENT_DESTRUCTOR = 22
    STATEMENT_METHOD_DECLARATION = 23
    STATEMENT_METHOD_DEFINITION = 24
    STATEMENT_MEMBER = 25
    STATEMENT_FORWARD_DECLARATION = 26

    ACCESS_SPECIFIER = 27
    NEW_LINE = 28
    OUTPUT_DEPTH_INCREMENT = 29
    OUTPUT_DEPTH_DECREMENT = 30


def CreateNewNode(in_stack, in_child_array, in_type, in_token, in_access):
    node = AST(in_type, in_token, in_access)
    in_stack.append(node)
    in_child_array.append(node)

def CreateNewChild(in_child_array, in_type, in_token, in_access = AstAccess.NONE):
    node = AST(in_type, in_token, in_access)
    in_child_array.append(node)

def FindClassStructName(in_stack):
    class_found = False
    struct_found = False
    for item in reversed(in_stack):
        if item._type == AstType.SCOPE:
            continue
        if item._type == AstType.STATEMENT_CLASS:
            class_found = True
        elif item._type == AstType.STATEMENT_STRUCT:
            struct_found = True
        else:
            return ""
        
        class_struct_token_found = "class" == item._token._data or "struct" == "item._token._data"
        for child in item._children:
            if child._type == AstType.TOKEN and class_struct_token_found == True:
                return child._token._data
            if False == class_struct_token_found:
                class_struct_token_found = "class" == child._token._data or "struct" == "child._token._data"
    return ""

def RecalculateStatementType(in_ast_node, in_stack):
    parent_class_struct_name = FindClassStructName(in_stack)
    class_keyword_found = False
    struct_keyword_found = False
    parenthesist_found = False
    child_scope_found = False
    assignment_found = False
    parent_class_struct_name_found = False
    destructr_found = False

    if in_ast_node._token._data == "class":
        class_keyword_found = True
    if in_ast_node._token._data == "struct":
        struct_keyword_found = True
    if in_ast_node._token._data == parent_class_struct_name:
        parent_class_struct_name_found = True
    if in_ast_node._token._data == "~":
        destructr_found = True

    for child in in_ast_node._children:
        if child._token._data == "class" and False == struct_keyword_found:
            class_keyword_found = True
        if child._token._data == "struct" and False == class_keyword_found:
            struct_keyword_found = True
        if child._type == AstType.PARENTHESIS:
            parenthesist_found = True
        if child._type == AstType.SCOPE:
            child_scope_found = True
        if child._token._data == parent_class_struct_name:
            parent_class_struct_name_found = True
        if child._token._data == "~":
            destructr_found = True
        if child._token._data == "=":
            assignment_found = True

    if True == class_keyword_found and False == parenthesist_found and False == child_scope_found:
        return AstType.STATEMENT_FORWARD_DECLARATION
    if True == struct_keyword_found and False == parenthesist_found and False == child_scope_found:
        return AstType.STATEMENT_FORWARD_DECLARATION

    if False == parenthesist_found and True == child_scope_found and True == class_keyword_found:
        return AstType.STATEMENT_CLASS
    if False == parenthesist_found and True == child_scope_found and True == struct_keyword_found:
        return AstType.STATEMENT_STRUCT
    
    if True == parent_class_struct_name_found and False == destructr_found and True == parenthesist_found:
        return AstType.STATEMENT_CONSTRUCTOR
    if True == parent_class_struct_name_found and True == destructr_found and True == parenthesist_found:
        return AstType.STATEMENT_DESTRUCTOR

    if True == parenthesist_found and True == child_scope_found and  "" != parent_class_struct_name:
        return AstType.STATEMENT_METHOD_DEFINITION

    if True == parenthesist_found and False == child_scope_found and  "" != parent_class_struct_name:
        return AstType.STATEMENT_METHOD_DECLARATION

    if "" != parent_class_struct_name:
        return AstType.STATEMENT_MEMBER

    return AstType.STATEMENT

class AST:
    def __init__(self, in_type = AstType.NONE, in_token = None, in_access = AstAccess.NONE):
        self._type = in_type
        self._token = in_token
        self._access = in_access
        self._children = []

    def __str__(self):
        line = f"AST type ({self._type.name}) token ({self._token})"
        if self._access != AstAccess.NONE:
            line += f" access ({self._access.name})"
        return line

    def __repr__(self):
        return f"AST(\'{self._type.name}\', \'{self._token}\', \'{self._access}\', \'{self._children}\')"

    def AddToken(self, in_stack, in_token):
        if in_token._data == "#pragma":
            CreateNewNode(in_stack, self._children, AstType.PREPROCESSOR_PRAGMA, in_token, self._access)
            return
        if in_token._data == "#include":
            CreateNewNode(in_stack, self._children, AstType.PREPROCESSOR_INCLUDE, in_token, self._access)
            return
        if in_token._data == "public:":
            CreateNewChild(self._children, AstType.ACCESS_SPECIFIER, in_token, AstAccess.PUBLIC)
            self._access = AstAccess.PUBLIC
            return
        if in_token._data == "protected:":
            CreateNewChild(self._children, AstType.ACCESS_SPECIFIER, in_token, AstAccess.PROTECTED)
            self._access = AstAccess.PROTECTED
            return
        if in_token._data == "private:":
            CreateNewChild(self._children, AstType.ACCESS_SPECIFIER, in_token, AstAccess.PRIVATE)
            self._access = AstAccess.PRIVATE
            return
        if in_token._data == "{":
            CreateNewNode(in_stack, self._children, AstType.SCOPE, in_token, self._access)
            return
        if in_token._data == "}":
            CreateNewChild(self._children, AstType.SCOPE_END, in_token)
            in_stack.pop()
            #if in_stack[-1]._type == AstType.STATEMENT:
            #    in_stack.pop()
            return
        if in_token._data == "(":
            CreateNewNode(in_stack, self._children, AstType.PARENTHESIS, in_token, self._access)
            return
        if in_token._data == ")":
            CreateNewChild(self._children, AstType.PARENTHESIS_END, in_token)
            in_stack.pop()
            return

        if in_token._data == ";":
            #if self._type == AstType.STATEMENT:
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
        line = ("    " * in_depth) + str(self)
        print(line)
        for child in self._children:
            child.Dump(in_depth + 1)

    # The visitor should return False to stop visiting each ast node
    def Visit(self, in_visitor, in_stack = [], in_data = {}):
        keep_going = in_visitor(self, in_stack, in_data)
        if False == keep_going:
            return keep_going
        in_stack.append(self)
        for child in self._children:
            keep_going = child.Visit(in_visitor, in_stack, in_data)
            if False == keep_going:
                break
        in_stack.pop()
        return keep_going

def DoctorType(in_ast_node, in_stack = []):
    if in_ast_node._type == AstType.STATEMENT:
        in_ast_node._type = RecalculateStatementType(in_ast_node, in_stack)

    in_stack.append(in_ast_node)
    for child in in_ast_node._children:
        DoctorType(child, in_stack)
    in_stack.pop()

def MakeAst(in_token_array, in_debug = False):
    result = AST(AstType.DOCUMENT)
    stack = [result]
    for token in in_token_array:
        stack[-1].AddToken(stack, token)

    DoctorType(result)

    if True == in_debug:
        result.Dump()

    return result


