from . import dsc_token_cpp
from . import dsc_ast_cpp
from . import common

def IsParamOut(in_ast_node, in_stack_ast_node, in_name):
    if in_name.startswith("out_"):
        return True
    return False

def IsParamOfMethod(in_ast_node, in_stack_ast_node, in_name):
    if in_name.startswith("in_"):
        return True
    if in_name.startswith("m_"):
        return False
    if in_name.startswith("g_"):
        return False
    if in_name.startswith("c_"):
        return False
    elif in_name.startswith("m") and in_name[1].isupper():
        return False
    elif in_name.startswith("s") and in_name[1].isupper():
        return False
    elif in_name.startswith("mp") and in_name[2].isupper():
        return False

    parent = None
    if 1 < len(in_stack_ast_node):
        parent = in_stack_ast_node[-1]
    grand_parent = None
    if 2 < len(in_stack_ast_node):
        grand_parent = in_stack_ast_node[-2]

    prev_node = None
    if parent:
        for child in parent._children:
            if id(child) == id(in_ast_node):
                break
            elif child._type != dsc_ast_cpp.AstType.WHITE_SPACE:
                prev_node = child

        if prev_node and prev_node._token:
            if prev_node._token._data in set({"::"}):
                return False

    if (
        parent and grand_parent and parent._type == dsc_ast_cpp.AstType.PARENTHESIS and
        (
            grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_CONSTRUCTOR or
            grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_DESTRUCTOR or
            grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DECLARATION or
            grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION
        )
        ):
        return True
    return False

# just use name, or look at stack for scope under class or struct
def IsClassMember(in_ast_node, in_stack_ast_node, in_name):
    if in_name.startswith("m_"):
        return True
    if in_name.startswith("g_"):
        return False
    if in_name.startswith("c_"):
        return False
    elif in_name.startswith("m") and in_name[1].isupper():
        return True
    elif in_name.startswith("mp") and in_name[2].isupper():
        return True
    
    statement = None
    if 1 < len(in_stack_ast_node):
        statement = in_stack_ast_node[-1]
    parent = None
    if 2 < len(in_stack_ast_node):
        parent = in_stack_ast_node[-2]
    grand_parent = None
    if 3 < len(in_stack_ast_node):
        grand_parent = in_stack_ast_node[-3]
    if (
        statement and parent and grand_parent and 
        statement._type == dsc_ast_cpp.AstType.STATEMENT and
        parent._type == dsc_ast_cpp.AstType.SCOPE
        ):
        # is class or struct the grandparent or child of grandparent
        class_or_struct_found = False
        if grand_parent._token._data == "class" or grand_parent._token._data == "struct":
            return True
        for child in grand_parent._children:
            if child._token._data == "class" or child._token._data == "struct":
                return True
            if child._token._data == "(":
                break
            if child._token._data == "<":
                break

    prev_node = None
    if statement:
        match_found = False
        for child in statement._children:
            if id(child) == id(in_ast_node):
                match_found = True
            elif match_found == True:
                if child._type == dsc_ast_cpp.AstType.PARENTHESIS:
                    prev_node = None
                break
            elif child._type != dsc_ast_cpp.AstType.WHITE_SPACE:
                prev_node = child

        if prev_node and prev_node._token:
            if prev_node._token._data in set({".","->"}):
                return True

    return False

def Cleanup(in_data): # remove m_, in_, out_, "m" then upper, "mp" then upper, "p" then upper, "b" then upper, "pb" then upper, replace "hWnd" with "hwnd"
    result = in_data
    if result.startswith("m_"):
        result = result[2:]
    elif result.startswith("m") and 1 < len(result) and result[1].isupper():
        result = result[1:]
    elif result.startswith("mp") and 2 < len(result) and result[2].isupper():
        result = result[2:]
    elif result.startswith("_"):
        result = result[1:]

    prev = ""
    while prev != result:
        prev = result
        if result.startswith("in_"):
            result = result[3:]
        if result.startswith("out_"):
            result = result[4:]
        if result.startswith("p") and 1 < len(result) and result[1].isupper():
            result = result[1:]
        if result.startswith("b") and 1 < len(result) and result[1].isupper():
            result = result[1:]
        if result.startswith("pb") and 2 < len(result) and result[2].isupper():
            result = result[2:]

    result = result.replace("hWnd", "Hwnd")

    return result

# rename members "m_pCommandList" => "_command_list"
# rename method param "pCommandList" => "in_command_list"
# rename local members "swapChainDesc" => "swap_chain_desc"
# for (auto pIter : m_listResource) => for (auto iter : _list_resource)
def AstTransformRenameVariables(in_ast_node, in_stack_ast_node, in_visit_data):

    # don't rename tokens under a preprocessor statement
    for child in in_stack_ast_node:
        if child._type == dsc_ast_cpp.AstType.PREPROCESSOR:
            return True

    if (
        in_ast_node._token and
        in_ast_node._token._type == dsc_token_cpp.TokenType.TOKEN
        ):
        original_name = in_ast_node._token._data

        if original_name in in_visit_data:
            in_ast_node._token._data = in_visit_data[original_name]
            return True

        # don't rename [classes, structs, methods, macros] which should start with a capitol letter
        if original_name[:1].isupper():
            return True
        if original_name in set({"_DEBUG", "_uuidof", "std", "size_t"}):
            return True

        new_name = original_name
        new_name = Cleanup(new_name) # remove m_, in_, out_, "m" then upper, "mp" then upper, "p" then upper, "b" then upper, "pb" then upper, replace "hWnd" with "hwnd"
        new_name = common.SnakeCase(new_name)

        # is node a param of a method
        is_param_of_method = IsParamOfMethod(in_ast_node, in_stack_ast_node, original_name)
        is_param_out = IsParamOut(in_ast_node, in_stack_ast_node, original_name)
        # is node a member of a class, struct
        is_class_member = IsClassMember(in_ast_node, in_stack_ast_node, original_name)

        if True == is_class_member:
            new_name = "_" + new_name
        elif True == is_param_out:
            new_name = "out_" + new_name
            in_visit_data[original_name] = new_name
        elif True == is_param_of_method:
            new_name = "in_" + new_name
            in_visit_data[original_name] = new_name
        else:
            in_visit_data[original_name] = new_name

        in_ast_node._token._data = new_name

    # clear rename history if we are at the end of a method
    if in_ast_node._type == dsc_ast_cpp.AstType.SCOPE_END:
        # parent needs to be a scope
        # grand parent needs to be some for of method definition

        parent = None
        if 1 < len(in_stack_ast_node):
            parent = in_stack_ast_node[-1]
        grand_parent = None
        if 2 < len(in_stack_ast_node):
            grand_parent = in_stack_ast_node[-2]
        if (
            parent and grand_parent and parent._type == dsc_ast_cpp.AstType.SCOPE and
            (
                grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_CONSTRUCTOR or
                grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_DESTRUCTOR or
                grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DECLARATION or
                grand_parent._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION
            )
            ):
            in_visit_data.clear()

    return True
