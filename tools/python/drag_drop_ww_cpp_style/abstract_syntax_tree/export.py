def SaveFile(in_ast,in_file_path):
    print(f"SaveFile:{in_file_path}")

    data = in_ast.MakeString()

    text_file = open(in_file_path, "w")
    n = text_file.write(data)
    text_file.close()

    return
