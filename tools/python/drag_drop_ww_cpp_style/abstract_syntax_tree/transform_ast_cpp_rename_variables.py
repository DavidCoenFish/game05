from . import dsc_token_cpp
from . import dsc_ast_cpp

def RenameMemeber(in_name, in_visit_data):
    #first letter is capitol, is a class name or function name?
    if in_name[:1].isupper():
        return in_name
    if in_name.isdigit():
        return in_name

    new_name = in_name
    if "m_" == new_name[:2]:
        new_name = new_name[2:]
    else:
        if in_name in in_visit_data:
            return in_visit_data[in_name]
    if 2 <= len(new_name):
        if "p" == new_name[:1] and new_name[1].isupper():
            new_name = new_name[1:]
        elif "b" == new_name[:1] and new_name[1].isupper():
            new_name = new_name[1:]
    
    temp = new_name.replace("hWnd", "hwnd")
    new_name = ""
    for c in temp:
        if c.isupper() and new_name != "":
            new_name += "_"
        new_name += c.lower()

    new_name = "_" + new_name
    in_visit_data[in_name] = new_name

    return new_name

def RenameParameter(in_name, in_visit_data):
    if in_name.isdigit():
        return in_name

    #if in_name in in_visit_data:
    #    return in_visit_data[in_name]
    if "m_p" == in_name[:3] or "m_" == in_name[:2]:
        return RenameMemeber(in_name, in_visit_data)

    new_name = in_name
    new_name = new_name.replace("hWnd", "hwnd")
    if 2 <= len(new_name):
        if "p" == new_name[:1] and new_name[1].isupper():
            new_name = new_name[1:]
        elif "b" == new_name[:1] and new_name[1].isupper():
            new_name = new_name[1:]
    temp = ""
    for c in new_name:
        if c.isupper() and temp != "":
            temp += "_"
        temp += c.lower()
    new_name = "in_" + temp
    in_visit_data[in_name] = new_name

    return new_name

# rename members "m_pCommandList" => "_command_list"
# rename method param "pCommandList" => "in_command_list"
# rename local members "swapChainDesc" => "swap_chain_desc"
# for (auto pIter : m_listResource) => for (auto iter : _list_resource)
def AstTransformRenameVariables(in_ast_node, in_stack_ast_node, in_visit_data):
    # Rename member, the last token
    if in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_MEMBER:
        for child in reversed(in_ast_node._children):
            if child._type == dsc_ast_cpp.AstType.STATEMENT_END:
                continue
            if child._type == dsc_ast_cpp.AstType.TOKEN:
                child._token._data = RenameMemeber(child._token._data, in_visit_data)
                break

    # rename tokens in statement which are not keywords or operators
    if (
        in_ast_node._sub_type == dsc_ast_cpp.SubType.NONE and
        in_ast_node._type == dsc_ast_cpp.AstType.STATEMENT
        ):
        for child in in_ast_node._children:
            if (
                child._type == dsc_ast_cpp.AstType.TOKEN and
                child._token._type == dsc_token_cpp.TokenType.TOKEN

                ):
                child._token._data = RenameMemeber(child._token._data, in_visit_data)
                break

    # rename formal paramater list
    if in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION:
        found_colin = False
        for child in in_ast_node._children:
            if child._token:
                if child._token._data == ":":
                    found_colin = True
                elif found_colin == True and child._type == dsc_ast_cpp.AstType.TOKEN and child._token._type == dsc_token_cpp.TokenType.TOKEN:
                    child._token._data = RenameMemeber(child._token._data, in_visit_data)

    # Rename method param
    if (
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_CONSTRUCTOR or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_DESTRUCTOR or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DECLARATION or
        in_ast_node._sub_type == dsc_ast_cpp.SubType.STATEMENT_METHOD_DEFINITION
        ):
        for child in in_ast_node._children:
            if child._type == dsc_ast_cpp.AstType.PARENTHESIS:
                trigger = True
                for sub_child in reversed(child._children):
                    if (
                        sub_child._type == dsc_ast_cpp.AstType.PARENTHESIS_END or
                        sub_child._type == dsc_ast_cpp.AstType.WHITE_SPACE
                        ):
                        continue
                    if (
                        sub_child._token._data == "," or
                        sub_child._token._data == "="
                        ):
                        trigger = True
                    elif sub_child._type == dsc_ast_cpp.AstType.TOKEN and True == trigger:
                        sub_child._token._data = RenameParameter(sub_child._token._data, in_visit_data)
                        trigger = False

    return True
