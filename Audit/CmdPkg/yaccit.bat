REM // eg. yaccit CmdParser (formerly)
REM // This generates both CmdParser.cpp & CmdParser.h
byacc -d -l -o CmdParser.cpp CmdParser.y

rm tmp
sed -fCmdParser.sed CmdParser.cpp > tmp
rm CmdParser.cpp
mv tmp CmdParser.cpp

sed -fCmdParser.sed CmdParser.h > tmp
rm CmdParser.h
mv tmp CmdParser.h
