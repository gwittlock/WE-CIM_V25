REM // eg. lexit CmdLexer (formerly)
REM // This generates CmdLexer.cpp
flex -L -oCmdLexer.cpp CmdLexer.l
rm tmp
sed -fCmdLexer.sed CmdLexer.cpp > tmp
rm CmdLexer.cpp
mv tmp CmdLexer.cpp
