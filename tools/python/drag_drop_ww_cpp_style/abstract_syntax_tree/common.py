# special rule, runs of all capitol letter, m_pDXGIFactory => _dxgi_factory
# fooBarMoo => foo_bar_moo
# FooBarMoo => foo_bar_moo
# FooBMoo => foo_b_moo
# FooBAMoo => foo_ba_moo
# FooBarMOO => foo_bar_moo
# FOOBARMoo => foobar_moo
# FooBarM => foo_bar_m

def SnakeCase(in_data):
    result = ""
    # for c in in_data:
    #     if c.isupper() and result != "":
    #         result += "_"
    #     result += c.lower()
    length = len(in_data)
    index = 0
    was_upper = False
    while index < length: 
        c = in_data[index]
        next_c = c #"" #"".isupper == False
        if index + 1 < length:
            next_c = in_data[index + 1]
        index += 1

        c_upper = c.isupper()
        next_c_upper = next_c.isupper()
        if next_c.isdigit():
            next_c_upper = c_upper

        if True == c_upper and False == next_c_upper:
            was_upper = False

        if True == c_upper and False == was_upper:
            if result != "":
                result += "_"

        if True == c_upper:
            was_upper = True
        else:
            was_upper = False

        result += c.lower()

    prev = ""
    while prev != result:
        prev = result
        result = result.replace("__", "_")

    return result
