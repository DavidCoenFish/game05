import enum
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
    WHITE_SPACE = 3
    PREPROCESSOR = 4
    TOKEN = 5
    STATEMENT = 6
    STATEMENT_END = 7
    SCOPE = 8
    SCOPE_END = 9
    PARENTHESIS = 10
    PARENTHESIS_END = 11
    ARRAY_INDEX = 12
    ARRAY_INDEX_END = 13

class SubType(enum.Enum):
    NONE = 0

    #PREPROCESSOR_PRAGMA = 3
    PREPROCESSOR_INCLUDE = 4
    #PREPROCESSOR_CONDITION = 5
    #PREPROCESSOR_DEFINITION = 6

    STATEMENT_CLASS = 19
    STATEMENT_STRUCT = 20
    STATEMENT_CONSTRUCTOR = 21
    STATEMENT_DESTRUCTOR = 22
    STATEMENT_METHOD_DECLARATION = 23
    STATEMENT_METHOD_DEFINITION = 24
    STATEMENT_MEMBER = 25
    STATEMENT_FORWARD_DECLARATION = 26
    STATEMENT_ACCESS = 27
    # STATEMENT_IF
    # STATEMENT_ELSE

def SaveTextFile(in_file_path, in_data):
    text_file = open(in_file_path, "w")
    n = text_file.write(in_data)
    text_file.close()

def CreateNewNode(in_stack, in_child_array, in_type, in_token, in_access = AstAccess.NONE, in_sub_type = SubType.NONE):
    node = AST(in_type, in_token, in_access, in_sub_type)
    in_stack.append(node)
    in_child_array.append(node)

def CreateNewChild(in_child_array, in_type, in_token, in_access = AstAccess.NONE, in_sub_type = SubType.NONE):
    node = AST(in_type, in_token, in_access, in_sub_type)
    in_child_array.append(node)

def FindClassStructName(in_stack):
    class_struct_token_found = False
    for item in reversed(in_stack):
        if item._type == AstType.STATEMENT and item._token:
            if item._token._sub_type == dsc_token_cpp.KeywordType.CLASS:
                class_struct_token_found = True
            elif item._token._sub_type == dsc_token_cpp.KeywordType.STRUCT:
                class_struct_token_found = True
        else:
            continue

        for child in item._children:
            if child._type == AstType.TOKEN and class_struct_token_found == True:
                return child._token._data
            if False == class_struct_token_found and child._token:
                if child._token._sub_type == dsc_token_cpp.KeywordType.CLASS:
                    class_struct_token_found = True
                elif child._token._sub_type == dsc_token_cpp.KeywordType.STRUCT:
                    class_struct_token_found = True

    return ""

def RecalculateStatementType(in_ast_node, in_stack):
    parent_class_struct_name = FindClassStructName(in_stack)
    class_keyword_found = False
    struct_keyword_found = False
    other_keyword_found = False
    parenthesist_found = False
    child_scope_found = False
    assignment_found = False
    access_found = False
    parent_class_struct_name_found = False
    destructr_found = False

    if in_ast_node._token:
        if in_ast_node._token._sub_type == dsc_token_cpp.KeywordType.CLASS:
            class_keyword_found = True
        elif in_ast_node._token._sub_type == dsc_token_cpp.KeywordType.STRUCT:
            struct_keyword_found = True
        elif in_ast_node._token._sub_type in set({ dsc_token_cpp.KeywordType.FOR, dsc_token_cpp.KeywordType.WHILE, dsc_token_cpp.KeywordType.IF }):
            other_keyword_found = True
        elif (
            in_ast_node._token._sub_type == dsc_token_cpp.KeywordType.PUBLIC or
            in_ast_node._token._sub_type == dsc_token_cpp.KeywordType.PROTECTED or
            in_ast_node._token._sub_type == dsc_token_cpp.KeywordType.PRIVATE
            ):
            access_found = True
        elif in_ast_node._token._data == parent_class_struct_name:
            parent_class_struct_name_found = True
        elif in_ast_node._token._data == "~":
            destructr_found = True

    for child in in_ast_node._children:
        if child._token._sub_type == dsc_token_cpp.KeywordType.CLASS and False == struct_keyword_found:
            class_keyword_found = True
        if child._token._sub_type == dsc_token_cpp.KeywordType.STRUCT and False == class_keyword_found:
            struct_keyword_found = True
        if in_ast_node._token._sub_type in set({ dsc_token_cpp.KeywordType.FOR, dsc_token_cpp.KeywordType.WHILE, dsc_token_cpp.KeywordType.IF }):
            other_keyword_found = True
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

    if True == access_found:
        return SubType.STATEMENT_ACCESS
    if True == class_keyword_found and False == parenthesist_found and False == child_scope_found:
        return SubType.STATEMENT_FORWARD_DECLARATION
    if True == struct_keyword_found and False == parenthesist_found and False == child_scope_found:
        return SubType.STATEMENT_FORWARD_DECLARATION

    if False == parenthesist_found and True == child_scope_found and True == class_keyword_found:
        return SubType.STATEMENT_CLASS
    if False == parenthesist_found and True == child_scope_found and True == struct_keyword_found:
        return SubType.STATEMENT_STRUCT
    
    if True == parent_class_struct_name_found and False == destructr_found and True == parenthesist_found:
        return SubType.STATEMENT_CONSTRUCTOR
    if True == parent_class_struct_name_found and True == destructr_found and True == parenthesist_found:
        return SubType.STATEMENT_DESTRUCTOR

    if True == parenthesist_found and True == child_scope_found and False == other_keyword_found: # and  "" != parent_class_struct_name:
        return SubType.STATEMENT_METHOD_DEFINITION

    if True == parenthesist_found and False == child_scope_found and False == other_keyword_found: # and  "" != parent_class_struct_name:
        return SubType.STATEMENT_METHOD_DECLARATION

    if "" != parent_class_struct_name:
        return SubType.STATEMENT_MEMBER

    return SubType.NONE

def CalculateColonLable(in_parent):
    class_or_struct_found = False
    parenthesis_found = False
    questionmark_found = False
    if in_parent._type == AstType.PARENTHESIS:
        # case 5, for (auto iter : m_arrayPage)
        return False

    if in_parent._token and ( in_parent._token._data == "class" or in_parent._token._data == "struct"):
        class_or_struct_found = True
    for child in in_parent._children:
        if child._token and ( child._token._data == "class" or child._token._data == "struct"):
            class_or_struct_found = True
        elif child._token and child._token._data == "(":
            parenthesis_found = True
        elif child._token and child._token._data == "?":
            questionmark_found = True
    if True == class_or_struct_found or True == parenthesis_found or True == questionmark_found:
        # case 3, "class Foo : public Bar
        # case 4, "ctor(int in_a) : m_a(in_a), m_b() {}
        return False

    # case 1, "public:" access specifier
    # case 2, "default:" switch lable, goto lable
    return True

# return True 
def CalculateScopeStatementEnd(in_stack, in_forward_tokens):
    if len(in_stack) <= 0:
        return False
    parent = in_stack[-1]
    if parent._type != AstType.STATEMENT:
        return False
    
    # look ahead, if there is a ";" after the scope end "}", we don't close the scope now
    for item in in_forward_tokens[1:]:
        if (
            item._type == dsc_token_cpp.TokenType.COMMENT or
            item._type == dsc_token_cpp.TokenType.WHITE_SPACE
            ):
            continue
        if item._data == ";":
            return False
        break

    # If we find the [class, struct, namespace] keyword, the scope end is not the statemenet end
    if parent._token._sub_type == dsc_token_cpp.KeywordType.CLASS:
        return False
    elif parent._token._sub_type == dsc_token_cpp.KeywordType.STRUCT:
        return False
    for child in parent._children:
        if child._token._sub_type == dsc_token_cpp.KeywordType.CLASS:
            return False
        elif child._token._sub_type == dsc_token_cpp.KeywordType.STRUCT:
            return False
        elif child._token._sub_type == dsc_token_cpp.OperatorType.ASSIGNMENT:
            return False

    return True

class AST:
    def __init__(self, in_type = AstType.NONE, in_token = None, in_access = AstAccess.NONE, in_sub_type = SubType.NONE):
        self._type = in_type
        self._token = in_token
        self._access = in_access
        self._sub_type = in_sub_type
        self._children = []
        self._export_pre_token_format = []
        self._export_post_token_format = []
        self._export_post_children_format = []

    def __str__(self):
        line = f"AST type ({self._type.name}) token ({self._token})"
        if self._access != AstAccess.NONE:
            line += f" access ({self._access.name})"
        if self._sub_type != SubType.NONE:
            line += f" sub_type ({self._sub_type.name})"
        return line

    def __repr__(self):
        return f"AST(\'{self._type.name}\', \'{self._token}\', \'{self._access}\', \'{self._sub_type}\', \'{self._children}\')"

    def CreateTokenOrStatement(self, in_stack, in_token_or_statement, in_token):
        if in_token_or_statement == AstType.STATEMENT:
            CreateNewNode(in_stack, self._children, in_token_or_statement, in_token, self._access)
        else:
            CreateNewChild(self._children, in_token_or_statement, in_token, self._access)
        return

    def AddTokenToChildren(self, in_token, in_stack, in_forward_tokens):
        token_or_statement = AstType.TOKEN
        parent = None
        grand_parent = None
        if 0 < len(in_stack):
            parent = in_stack[-1]
        if 1 < len(in_stack):
            grand_parent = in_stack[-2]
        if parent and parent._type in set({
            AstType.DOCUMENT,
            AstType.SCOPE
            }):
            token_or_statement = AstType.STATEMENT

            # assignment to array
            if parent._type == AstType.SCOPE and grand_parent:
                for child in grand_parent._children:
                    if "=" == child._token._data:
                        token_or_statement = AstType.TOKEN



        if in_token._type == dsc_token_cpp.TokenType.COMMENT:
            CreateNewChild(self._children, AstType.COMMENT, in_token, self._access)
        elif in_token._type == dsc_token_cpp.TokenType.WHITE_SPACE:
            if token_or_statement != AstType.STATEMENT:
                CreateNewChild(self._children, AstType.WHITE_SPACE, in_token, self._access)
        elif in_token._type == dsc_token_cpp.TokenType.INCLUDE_PATH_SPEC:
            CreateNewChild(self._children, AstType.TOKEN, in_token, self._access)
        elif in_token._type == dsc_token_cpp.TokenType.STRING_LITERAL:
            CreateNewChild(self._children, AstType.TOKEN, in_token, self._access)
        elif in_token._type == dsc_token_cpp.TokenType.CHARACTER_LITERAL:
            CreateNewChild(self._children, AstType.TOKEN, in_token, self._access)
        elif in_token._type == dsc_token_cpp.TokenType.PREPROCESSOR:
            sub_type = SubType.NONE
            if in_token._sub_type == dsc_token_cpp.PreprocessorType.INCLUDE:
                sub_type = SubType.PREPROCESSOR_INCLUDE
            CreateNewNode(in_stack, self._children, AstType.PREPROCESSOR, in_token, self._access, sub_type)
        elif in_token._type == dsc_token_cpp.TokenType.OPERATOR:
            if in_token._sub_type == dsc_token_cpp.OperatorType.SCOPE_START:
                CreateNewNode(in_stack, self._children, AstType.SCOPE, in_token, self._access)
            elif in_token._sub_type == dsc_token_cpp.OperatorType.SCOPE_END:
                CreateNewChild(self._children, AstType.SCOPE_END, in_token, self._access)
                in_stack.pop()
                #in_stack.pop()
                if True == CalculateScopeStatementEnd(in_stack, in_forward_tokens):
                    in_stack.pop()

            elif in_token._sub_type == dsc_token_cpp.OperatorType.PARENTHISIS_START:
                CreateNewNode(in_stack, self._children, AstType.PARENTHESIS, in_token, self._access)
            elif in_token._sub_type == dsc_token_cpp.OperatorType.PARENTHISIS_END:
                CreateNewChild(self._children, AstType.PARENTHESIS_END, in_token, self._access)
                in_stack.pop()

            elif in_token._sub_type == dsc_token_cpp.OperatorType.ARRAY_INDEX_START:
                CreateNewNode(in_stack, self._children, AstType.ARRAY_INDEX, in_token, self._access)
            elif in_token._sub_type == dsc_token_cpp.OperatorType.ARRAY_INDEX_END:
                CreateNewChild(self._children, AstType.ARRAY_INDEX_END, in_token, self._access)
                in_stack.pop()

            elif in_token._sub_type == dsc_token_cpp.OperatorType.STATEMENT_END:
                CreateNewChild(self._children, AstType.STATEMENT_END, in_token, self._access)
                if (
                    parent and parent._type == AstType.PARENTHESIS and
                    grand_parent and grand_parent._token and 
                    grand_parent._token._sub_type == dsc_token_cpp.KeywordType.FOR
                    ):
                    pass
                else:
                    in_stack.pop()

            elif in_token._sub_type == dsc_token_cpp.OperatorType.COLON:
                # case 1, "public:" access specifier
                # case 2, "default:" switch lable, goto lable
                if True == CalculateColonLable(in_stack[-1]):
                    CreateNewChild(self._children, AstType.STATEMENT_END, in_token, self._access)
                    in_stack.pop()

                # case 3, "class Foo : public Bar
                # case 4, "ctor(int in_a) : m_a(in_a), m_b() {}
                # case 5, "for (auto iter : m_arrayPage)
                else:
                    CreateNewChild(self._children, AstType.TOKEN, in_token, self._access)

            elif in_token._sub_type == dsc_token_cpp.OperatorType.TILDA:
                if token_or_statement == AstType.STATEMENT:
                    CreateNewNode(in_stack, self._children, AstType.STATEMENT, in_token, self._access)
                else:
                    CreateNewChild(self._children, AstType.TOKEN, in_token, self._access)

            else:
                CreateNewChild(self._children, AstType.TOKEN, in_token, self._access)

        elif in_token._type == dsc_token_cpp.TokenType.KEYWORD:
            if token_or_statement == AstType.STATEMENT:
                if in_token._sub_type == dsc_token_cpp.KeywordType.CLASS:
                    self._access = AstAccess.PRIVATE
                elif in_token._sub_type == dsc_token_cpp.KeywordType.STRUCT:
                    self._access = AstAccess.PUBLIC
                elif in_token._sub_type == dsc_token_cpp.KeywordType.PUBLIC:
                    self._access = AstAccess.PUBLIC
                    CreateNewNode(in_stack, self._children, token_or_statement, in_token, self._access)
                    return
                elif in_token._sub_type == dsc_token_cpp.KeywordType.PROTECTED:
                    self._access = AstAccess.PROTECTED
                    CreateNewNode(in_stack, self._children, token_or_statement, in_token, self._access)
                    return
                elif in_token._sub_type == dsc_token_cpp.KeywordType.PRIVATE:
                    self._access = AstAccess.PRIVATE
                    CreateNewNode(in_stack, self._children, token_or_statement, in_token, self._access)
                    return
            self.CreateTokenOrStatement(in_stack, token_or_statement, in_token)
        elif in_token._type == dsc_token_cpp.TokenType.NUMERIC:
            self.CreateTokenOrStatement(in_stack, AstType.TOKEN, in_token)
        elif in_token._type == dsc_token_cpp.TokenType.TOKEN:
            self.CreateTokenOrStatement(in_stack, token_or_statement, in_token)
        else:
            raise Exception("invalid state")

    def AddToken(self, in_stack, in_token, in_forward_tokens):
        parent = None
        if 0 < len(in_stack):
            parent = in_stack[-1]

        # Finishing the line for a preprocessor
        if parent._type == AstType.PREPROCESSOR:
            if (
                in_token._type == dsc_token_cpp.TokenType.WHITE_SPACE and
                "\n" in in_token._data
                ):
                in_stack.pop()
                return 1

        self.AddTokenToChildren(in_token, in_stack, in_forward_tokens)
        return 1

    def Dump(self, in_depth = 0):
        data = ("    " * in_depth) + str(self) + "\n"
        #print(data)
        for child in self._children:
            data += child.Dump(in_depth + 1)
        return data

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
        in_ast_node._sub_type = RecalculateStatementType(in_ast_node, in_stack)

    in_stack.append(in_ast_node)
    for child in in_ast_node._children:
        DoctorType(child, in_stack)
    in_stack.pop()

def MakeAst(in_token_array, in_debug = False):
    result = AST(AstType.DOCUMENT)
    stack = [result]

    index = 0
    while index < len(in_token_array):
        token = in_token_array[index]
        forward_tokens = in_token_array[index:]

        if len(stack) <= 0:
            debug_file_data = result.Dump()
            SaveTextFile("build\\debug_ast_exception.txt", debug_file_data)
            raise Exception("MakeAst::stack reach zero size")

        step = stack[-1].AddToken(stack, token, forward_tokens)

        if step <= 0:
            raise Exception("invalid state")

        index += step

    DoctorType(result)

    if True == in_debug:
        debug_file_data = result.Dump()
        SaveTextFile("build\\debug_ast.txt", debug_file_data)

    return result


