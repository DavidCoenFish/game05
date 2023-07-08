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
        return self._data

    def NewLineIfNeeded(self):
        if False == self._current_line.isspace():
            self.NewLine()

    def NewLine(self):
        if self._current_line.isspace():
            self._current_line = ""
        self._current_line += self._new_line
        self._data += self._current_line
        self._current_line = "" + (self._tab * self._depth)
        return

    # We add tokens 
    def AddTokkenToCurrent(self, in_token_text): 
        if self._max_line_length < len(self._current_line) + len(in_token_text):
            self.IncrementDepth()
            self.NewLine()
            self._current_line += in_token_text
            self.DecrementDepth()
        else:
            self._current_line += in_token_text

    def AddEndLineComment(self, in_comment_text):
        text = "// " + in_comment_text
        self.AddTokkenToCurrent(text)
        self.NewLine()

    def AddMultiLineComment(self, in_multiline_comment_text):
        if "\n" in in_multiline_comment_text:
            self.NewLine()
            self.AddTokkenToCurrent("/*")
            self.IncrementDepth()
            split_line = in_multiline_comment_text.split("\n") 
            for line in split_line:
                self.NewLine()
                self.AddTokkenToCurrent(line)
            self.DecrementDepth()
            self.NewLine()
            self.AddTokkenToCurrent("*/")
            self.NewLine()

        else:
            text = "/*" + in_multiline_comment_text + "*/"
            self.AddTokkenToCurrent(text)

