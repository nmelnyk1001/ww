Your name and NetID
• Your partner’s name and NetID, if you worked with a partner.
• A brief description of your testing strategy. How did you determine that your program is
correct? What sorts of files and scenarios did you check?
Name: Nathan Melnyk
NetID: nbm41
Testing Strategy:
- Ran against valgrind --leak-check=full to ensure no memory leaks were possible with all modes of input.
- Because all modes of ./ww run with the same code to parse the file(s), I mostly tested against ./ww [num] [file]
    - Checked input with width=(-x,0]
    - Checked input with no args
    - Checked input with too many args 
    - Checked input with invalid file path 
    - Checked input with invalid directory path
    - Checked input with stdin 
    - Checked input with multiple spaces and lines before any input
    - Checked input with multiple lines in between paragraphs
    - Checked input with words that are equal to the size of the width 
    - Checked input with words too big to fit the width
    - Checked input with tabs and other non space, non newline whitespace characters
    - Checked input with subdirectories 
    - Checked input with no files within subdirectory
    - Checked input with files already created within the directory