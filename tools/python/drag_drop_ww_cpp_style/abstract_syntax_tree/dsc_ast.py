import enum

class AstAccess(enum.Enum):
    NONE = 0
    PUBLIC = 1
    PROTECTED = 2
    PRIVATE = 3

class AstType(enum.Enum):
    NONE = 0
    DOCUMENT = 1
    PRAGMA = 2
    INCLUDE = 3
    CLASS = 4
    STRUCT = 5
    COMMENT_LINE = 9
    COMMENT_MULTI = 10
    SCOPE = 11
    METHOD = 12
    MEMBER = 13
    UNKNOWN = 14
    TOKEN = 15
    ENUM = 16
    ASSIGNMENT = 17
    PARAMATERS = 18

class AST:
    def __init__(self, in_type = AstType.NONE, in_data = "", in_access = AstAccess.NONE):
        self._type = in_type
        self._data = in_data
        self._access = in_access
        self._children = []
    
    def AddToken(self, in_stack, in_token):
        if in_token._data == "#pragma":
            node = AST(AstType.PRAGMA)
            in_stack.append(node)
            self._children.append(node)
            return

        if in_token._data == "#include":
            node = AST(AstType.INCLUDE)
            in_stack.append(node)
            self._children.append(node)
            return

        if in_token._data == "//":
            node = AST(AstType.COMMENT_LINE)
            in_stack.append(node)
            self._children.append(node)
            return

        if in_token._data == "/*":
            node = AST(AstType.COMMENT_MULTI)
            in_stack.append(node)
            self._children.append(node)
            return

        if in_token._data == "class":
            node = AST(AstType.CLASS, "", AstAccess.PRIVATE)
            in_stack.append(node)
            self._children.append(node)
            return

        if in_token._data == "struct":
            node = AST(AstType.STRUCT, "", AstAccess.PUBLIC)
            in_stack.append(node)
            self._children.append(node)
            return

        if in_token._data == "enum":
            node = AST(AstType.ENUM)
            in_stack.append(node)
            self._children.append(node)
            return

        if in_token._data == "{":
            node = AST(AstType.SCOPE, "", self._access)
            self._children.append(node)
            in_stack.append(node)
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

        if self._type == AstType.PRAGMA:
            self._data = in_token._data
            in_stack.pop()
            return

        if self._type == AstType.INCLUDE:
            self._data = in_token._data
            in_stack.pop()
            return

        if self._type == AstType.COMMENT_LINE:
            self._data = in_token._data
            in_stack.pop()
            return

        if self._type == AstType.COMMENT_MULTI:
            self._data = in_token._data
            in_stack.pop()
            return

        if (
            self._type == AstType.CLASS or 
            self._type == AstType.STRUCT or
            self._type == AstType.ENUM 
            ):
            if in_token._data == ";":
                in_stack.pop()
                return
            if in_token._data == "{":
                node = AST(AstType.SCOPE)
                in_stack.append(node)
                self._children.append(node)
                return

            self._children.append(AST(AstType.TOKEN, in_token._data))
            return

        if self._type == AstType.SCOPE:
            if in_token._data == "}":
                in_stack.pop()
                return
            node = AST(AstType.UNKNOWN, in_token._data, self._access)
            in_stack.append(node)
            self._children.append(node)
            return

        if self._type == AstType.UNKNOWN:
            if in_token._data == ";":
                self._type = AstType.MEMBER
                in_stack.pop()
                return
            if in_token._data == "(":
                self._type = AstType.METHOD
                node = AST(AstType.PARAMATERS)
                in_stack.append(node)
                self._children.append(node)
                return

            if in_token._data == "=":
                self._type = AstType.ASSIGNMENT

            self._children.append(AST(AstType.TOKEN, in_token._data))
            return
        
        if self._type == AstType.METHOD:
            if in_token._data == ";":
                in_stack.pop()
                return
            self._children.append(AST(AstType.TOKEN, in_token._data))
            return

        if self._type == AstType.PARAMATERS:
            if in_token._data == ")":
                in_stack.pop()
                return
            self._children.append(AST(AstType.TOKEN, in_token._data))
            return

        if self._type == AstType.ASSIGNMENT:
            if in_token._data == ";":
                in_stack.pop()
                return
            self._children.append(AST(AstType.TOKEN, in_token._data))
            return


    def Dump(self, in_depth = 0):
        print(("    " * in_depth) + "{")
        line = ("    " * (in_depth + 1)) + f"type:\"{self._type}\""
        if self._access != AstAccess.NONE:
            line += f" access:\"{self._access}\""
        if self._data != "":
            line += f" data:\"{self._data}\""
        print(line)
        for child in self._children:
            child.Dump(in_depth + 1)
        print(("    " * in_depth) + "}")


    def MakeString(self, in_depth = 0):
        tab = "    "
        new_line = "\n"
        result = ""
        child_depth = in_depth
        if self._type == AstType.SCOPE:
            result += (tab * in_depth) + "{" + new_line
            child_depth += 1
        if self._type == AstType.PARAMATERS:
            result += "("
            if 0 != len(self._children):
                result += new_line
        if self._type == AstType.CLASS:
            result += (tab * in_depth) + "class "
            if "" != self._data:
                result += self._data
        if self._type == AstType.STRUCT:
            result += (tab * in_depth) + "struct "
            if "" != self._data:
                result += self._data
        if self._type == AstType.ENUM:
            result += (tab * in_depth) + "enum "
            if "" != self._data:
                result += self._data
        if self._type == AstType.PRAGMA:
            result += "#pragma"
            if "" != self._data:
                result += self._data + new_line
        if self._type == AstType.INCLUDE:
            result += "#include " + self._data + new_line

        if self._type == AstType.COMMENT_MULTI:
            result += "/*"
            if "" != self._data:
                result += self._data
            result += "*/" + new_line
        if self._type == AstType.COMMENT_LINE:
            result += "//"
            if "" != self._data:
                result += self._data
            result += new_line

        if (
            self._type == AstType.TOKEN or
            self._type == AstType.METHOD or 
            self._type == AstType.MEMBER
            ):
            result += self._data

        for child in self._children:
            if child._type == AstType.SCOPE:
                result += new_line
            result += child.MakeString(child_depth)
        
        if self._type == AstType.SCOPE:
            result += new_line
            result += (tab * in_depth) + "}"
            child_depth += 1
        if (
            self._type == AstType.CLASS or
            self._type == AstType.STRUCT or
            self._type == AstType.ENUM or
            self._type == AstType.MEMBER or
            self._type == AstType.METHOD
            ):
            result += ";" + new_line
        if self._type == AstType.PARAMATERS:
            if 0 == len(self._children):
                result += ")"
            else:
                result += new_line + (tab * in_depth) + ")"

        return result

def MakeAst(in_token_array):
    result = AST(AstType.DOCUMENT)
    stack = [result]
    for token in in_token_array:
        stack[-1].AddToken(stack, token)

    return result
