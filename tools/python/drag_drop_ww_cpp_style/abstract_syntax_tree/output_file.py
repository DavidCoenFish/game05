class OutputFile:
    def __init__(self, in_tab = "    ", in_new_line = "\n", in_max_line_length = 120, in_depth = 0):
        self._tab = in_tab
        self._new_line = in_new_line
        self._max_line_length = in_max_line_length
        self._depth = in_depth
        self._data = ""
        self._current_line = ""
        return

    def IncrementDepth(self):
        self._depth += 1
        return

    def DecrementDepth(self):
        self._depth -= 1
        return

    def GetData(self):
        if "" != self._current_line:
            self._data += self._current_line
            self._current_line = ""
        return self._data

    def NewLineIfNeeded(self):
        if False == self._current_line.isspace():
            self.NewLine()

    def NewLine(self):
        if self._current_line.isspace():
            self._current_line = ""
        self._current_line += self._new_line
        self._data += self._current_line
        self._current_line = ""
        return

    # We add tokens 
    def AddTokkenToCurrent(self, in_token_text): 
        if self._current_line == "":
            self._current_line = (self._tab * self._depth)

        if self._max_line_length < len(self._current_line) + len(in_token_text):
            self.IncrementDepth()
            self.NewLine()
            self._current_line = (self._tab * self._depth) + in_token_text
            self.DecrementDepth()
        else:
            self._current_line += in_token_text

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
            text = "/*" + in_multiline_comment_text + "*/"
            self.AddTokkenToCurrent(text)
