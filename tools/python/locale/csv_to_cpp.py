#C:\Python310\python.exe C:\development\game05\tools\python\locale\csv_to_cpp.py "C:\development\game05\tools\python\locale\locale - game_05.csv"

import sys
import os

def MakeLine(in_tab_depth, in_data):
    result = ""
    for i in range(in_tab_depth):
        result += "    "
    result += in_data
    result += "\n"
    return result

def Clean(in_sub_value):
    if in_sub_value.isascii():
        return in_sub_value
    temp = ""
    output = ""
    for char in in_sub_value:
        if char.isascii():
            temp += char
        else:
            if temp != "":
                output += f"\"{temp}\""
                temp = ""
            for byte in char.encode("utf-8"):
                output += f"\"\\x{hex(byte)[2:]}\""
    if temp != "":
        output += f"\"{temp}\""
    output = output[1:]
    output = output[:-1]
    return output

def DealFile(in_file_path, in_debug):
    file = open(in_file_path, "rb")
    data = file.read()
    file.close()
    #utf8_data = byte_data.decode("utf-8")

    id_array = []
    data_map = dict()
    first_line = True
    for line in data.decode("utf-8").split("\r\n"):
        token_array = line.split(",")
        if True == first_line:
            for token in token_array[1:]:
                id_array.append(token)
                data_map[token] = dict()
            first_line = False
        else:
            key = token_array[0]
            if "" == key:
                continue
            for i in range(len(token_array) - 1):
                id = id_array[i]
                token = token_array[i + 1]
                data_map[id][key] = token

    #print(data_map)
    output = ""
    for key, value in data_map.items():
        output += MakeLine(1, f"// {key}")
        output += MakeLine(1, "{")
        output += MakeLine(2, f"in_locale_system.Append(\"{key}\",")
        output += MakeLine(3, "std::vector<LocaleSystem::Data>({")
        for sub_key, sub_value in value.items():
            clean_value = Clean(sub_value)
            output += MakeLine(4, f"{{\"{sub_key}\",\"{clean_value}\"}},")
        output += MakeLine(3, "}));")
        output += MakeLine(1, "}")
        output += MakeLine(0, "")

    path_root,_ = os.path.splitext(in_file_path)
    new_path = path_root + ".cpp"

    file = open(new_path, "w")
    file.write(output)
    file.close()

def Main(*in_args):
    debug = False #True
    if 0 == len(in_args):
        print("usage csv_to_cpp.py: drag and drop files to process")
        return
    for iter in in_args[1:]:
        DealFile(iter, debug)
    return

if __name__ == "__main__":
    Main(*sys.argv)