class OutputFile:
    def __init__(self, in_tab = "    ", in_new_line = "\n", in_max_line_length = 120, in_depth = 0):
        self._tab = in_tab
        self._new_line = in_new_line
        self._max_line_length = in_max_line_length
        self._depth = in_depth
        self._data = ""
        self._current_line = ""
        self._depth_stack = []
        return

    def IncrementDepth(self):
        self._depth += 1
        return

    def DecrementDepth(self):
        self._depth -= 1
        return

    def PushDepth(self):
        self._depth_stack.append(self._depth)
        self._depth = 0
        return

    def PopDepth(self):
        self._depth = self._depth_stack.pop()
        return

    def GetData(self):
        if "" != self._current_line:
            self._data += self._current_line
            self._current_line = ""
        return self._data

    def NewLine(self):
        if self._current_line.isspace():
            self._current_line = ""
        self._current_line += self._new_line
        self._data += self._current_line
        self._current_line = ""
        return

    # We add tokens, but we don't add white space to the start of a line
    def AddTokkenToCurrent(self, in_token_text): 
        if None == in_token_text or "" == in_token_text:
            return

        token_copy = in_token_text
        if self._current_line == "" or self._current_line.isspace():
            self._current_line = (self._tab * self._depth)
            token_copy = token_copy.strip()

        if self._max_line_length < len(token_copy) and (self._current_line == "" or self._current_line.isspace()):
            self._current_line = (self._tab * self._depth) + token_copy.strip()
        elif self._max_line_length < len(self._current_line) + len(token_copy):
            self._current_line += "\\" # add a line continue
            self.NewLine()
            self._current_line = (self._tab * (self._depth + 1)) + token_copy.strip()
        else:
            self._current_line += token_copy

    def AddEndLineComment(self, in_comment_text):
        self.AddTokkenToCurrent(in_comment_text)

    def AddMultiLineComment(self, in_multiline_comment_text):
        data = in_multiline_comment_text[2:]
        data = data[:-2]
        if "\n" in data:
            self.AddTokkenToCurrent("/*")
            self.IncrementDepth()
            split_line = data.split("\n") 
            for line in split_line:
                self.NewLine()
                self.AddTokkenToCurrent(line)
            self.DecrementDepth()
            self.NewLine()
            self.AddTokkenToCurrent("*/")
        else:
            text = "/* " + data.strip() + " */"
            self.AddTokkenToCurrent(text)

