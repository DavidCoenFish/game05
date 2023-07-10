from . import dsc_token_cpp
from . import dsc_ast_cpp

def CalculateIsCandidateForClassStructReorder(in_ast_node, in_stack_ast_node):
    if in_ast_node._type != dsc_ast_cpp.AstType.SCOPE:
        return False
    for item in reversed(in_stack_ast_node):
        if item._type == dsc_ast_cpp.AstType.STATEMENT_CLASS:
            return True
        elif item._type == dsc_ast_cpp.AstType.STATEMENT_STRUCT:
            return True
        break
    return False

def CollectChildren(in_children, in_output, in_focus_set, out_comment_list = [], in_left_over = []):
    comment_list = []
    for child in in_children:
        if child._type in in_focus_set:
            if in_output:
                in_output[child._access] = in_output[child._access] + comment_list + [child]
                comment_list.clear()
            out_comment_list.clear()
        elif child._type == dsc_ast_cpp.AstType.COMMENT:
            comment_list.append(child)
            out_comment_list.append(child)
        elif (
            child._type == dsc_ast_cpp.AstType.WHITE_SPACE or
            child._type == dsc_ast_cpp.AstType.ACCESS_SPECIFIER
            ):
            pass
        else:
            #if in_output:
            comment_list.clear()
            in_left_over.append(child)

def AddChildren(in_new_children, in_data_array, in_access_type, in_access_name, in_last_access):
    if 0 == len(in_data_array):
        return in_last_access
    
    if in_last_access != in_access_type:
        token = dsc_token_cpp.Token(in_access_name, dsc_token_cpp.TokenType.TOKEN)
        node = dsc_ast_cpp.AST(dsc_ast_cpp.AstType.ACCESS_SPECIFIER, token, in_access_type)
        in_new_children.append(node)
    for item in in_data_array:
        in_new_children.append(item)

    return in_access_type

def AstTransformSortClassStructMethodsMembers(in_ast_node, in_stack_ast_node, in_data):
    if False == CalculateIsCandidateForClassStructReorder(in_ast_node, in_stack_ast_node):
        return True

    members = {
        dsc_ast_cpp.AstAccess.PUBLIC : [],
        dsc_ast_cpp.AstAccess.PROTECTED : [],
        dsc_ast_cpp.AstAccess.PRIVATE : [],
        }
    methods = {
        dsc_ast_cpp.AstAccess.PUBLIC : [],
        dsc_ast_cpp.AstAccess.PROTECTED : [],
        dsc_ast_cpp.AstAccess.PRIVATE : [],
        }

    # Collect forward declarations, classes, structs
    CollectChildren(in_ast_node._children, methods, set({ dsc_ast_cpp.AstType.STATEMENT_FORWARD_DECLARATION, dsc_ast_cpp.AstType.STATEMENT_CLASS, dsc_ast_cpp.AstType.STATEMENT_STRUCT }))

    # Collect ctor
    CollectChildren(in_ast_node._children, methods, set({ dsc_ast_cpp.AstType.STATEMENT_CONSTRUCTOR }))

    # Collect dtor
    CollectChildren(in_ast_node._children, methods, set({ dsc_ast_cpp.AstType.STATEMENT_DESTRUCTOR }))

    # Collect methods
    CollectChildren(in_ast_node._children, methods, set({ dsc_ast_cpp.AstType.STATEMENT_METHOD_DECLARATION, dsc_ast_cpp.AstType.STATEMENT_METHOD_DEFINITION}))

    # Collect members
    CollectChildren(in_ast_node._children, members, set({ dsc_ast_cpp.AstType.STATEMENT_MEMBER, dsc_ast_cpp.AstType.STATEMENT}))

    # get comments at end of class not collected by the above
    comment_list = []
    left_over = []
    CollectChildren(in_ast_node._children, None, set({ 
            dsc_ast_cpp.AstType.STATEMENT_FORWARD_DECLARATION, 
            dsc_ast_cpp.AstType.STATEMENT_CLASS, 
            dsc_ast_cpp.AstType.STATEMENT_STRUCT,
            dsc_ast_cpp.AstType.STATEMENT_CONSTRUCTOR,
            dsc_ast_cpp.AstType.STATEMENT_DESTRUCTOR,
            dsc_ast_cpp.AstType.STATEMENT_METHOD_DECLARATION, 
            dsc_ast_cpp.AstType.STATEMENT_METHOD_DEFINITION,
            dsc_ast_cpp.AstType.STATEMENT_MEMBER, 
            dsc_ast_cpp.AstType.STATEMENT
        }),
        comment_list,
        left_over
        )

    new_children = []

    last_access = dsc_ast_cpp.AstAccess.NONE
    last_access = AddChildren(new_children, methods[dsc_ast_cpp.AstAccess.PUBLIC], dsc_ast_cpp.AstAccess.PUBLIC, "public:", last_access)
    last_access = AddChildren(new_children, methods[dsc_ast_cpp.AstAccess.PROTECTED], dsc_ast_cpp.AstAccess.PROTECTED, "protected:", last_access)
    last_access = AddChildren(new_children, methods[dsc_ast_cpp.AstAccess.PRIVATE], dsc_ast_cpp.AstAccess.PRIVATE, "private:", last_access)
    last_access = AddChildren(new_children, members[dsc_ast_cpp.AstAccess.PUBLIC], dsc_ast_cpp.AstAccess.PUBLIC, "public:", last_access)
    last_access = AddChildren(new_children, members[dsc_ast_cpp.AstAccess.PROTECTED], dsc_ast_cpp.AstAccess.PROTECTED, "protected:", last_access)
    last_access = AddChildren(new_children, members[dsc_ast_cpp.AstAccess.PRIVATE], dsc_ast_cpp.AstAccess.PRIVATE, "private:", last_access)
    for item in comment_list:
        new_children.append(item)
    for item in left_over:
        new_children.append(item)

    in_ast_node._children = new_children

    return True

