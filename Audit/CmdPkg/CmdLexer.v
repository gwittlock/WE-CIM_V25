#############################################################################
#                     U N R E G I S T E R E D   C O P Y
# 
# You are on day 5 of your 30 day trial period.
# 
# This file was produced by an UNREGISTERED COPY of Parser Generator. It is
# for evaluation purposes only. If you continue to use Parser Generator 30
# days after installation then you are required to purchase a license. For
# more information see the online help or go to the Bumble-Bee Software
# homepage at:
# 
# http://www.bumblebeesoftware.com
# 
# This notice must remain present in the file. It cannot be removed.
#############################################################################

#############################################################################
# CmdLexer.v
# Lex verbose file generated from CmdLexer.l.
# 
# Date: 12/01/04
# Time: 08:30:04
# 
# ALex Version: 2.07
#############################################################################


#############################################################################
# Expressions
#############################################################################

    1  [ \t]

    2  (\/{2,2}.*)

    3  [,=:]

    4  [-0-9]+

    5  ([-0-9]*\.[0-9]+[eE][-+]?[0-9]+)

    6  ([-0-9]+\.[eE][-+]?[0-9]+)

    7  ([-0-9]*\.[0-9]+)

    8  ([-0-9]+\.)

    9  \"[^"]*["]

   10  \'[^']*[']

   11  [a-z$#@A-Z_][a-zA-Z0-9_]*


#############################################################################
# States
#############################################################################

state 1
	INITIAL

	0x09               goto 3
	0x20               goto 3
	0x22               goto 4
	0x23 - 0x24 (2)    goto 5
	0x27               goto 6
	0x2c               goto 7
	0x2d               goto 8
	0x2e               goto 9
	0x2f               goto 10
	0x30 - 0x39 (10)   goto 8
	0x3a               goto 7
	0x3d               goto 7
	0x40 - 0x5a (27)   goto 5
	0x5f               goto 5
	0x61 - 0x7a (26)   goto 5


state 2
	^INITIAL

	0x09               goto 3
	0x20               goto 3
	0x22               goto 4
	0x23 - 0x24 (2)    goto 5
	0x27               goto 6
	0x2c               goto 7
	0x2d               goto 8
	0x2e               goto 9
	0x2f               goto 10
	0x30 - 0x39 (10)   goto 8
	0x3a               goto 7
	0x3d               goto 7
	0x40 - 0x5a (27)   goto 5
	0x5f               goto 5
	0x61 - 0x7a (26)   goto 5


state 3
	match 1


state 4
	0x00 - 0x21 (34)   goto 4
	0x22               goto 11
	0x23 - 0xff (221)  goto 4


state 5
	0x30 - 0x39 (10)   goto 5
	0x41 - 0x5a (26)   goto 5
	0x5f               goto 5
	0x61 - 0x7a (26)   goto 5

	match 11


state 6
	0x00 - 0x26 (39)   goto 6
	0x27               goto 12
	0x28 - 0xff (216)  goto 6


state 7
	match 3


state 8
	0x2d               goto 8
	0x2e               goto 13
	0x30 - 0x39 (10)   goto 8

	match 4


state 9
	0x30 - 0x39 (10)   goto 14


state 10
	0x2f               goto 15


state 11
	match 9


state 12
	match 10


state 13
	0x30 - 0x39 (10)   goto 14
	0x45               goto 16
	0x65               goto 16

	match 8


state 14
	0x30 - 0x39 (10)   goto 14
	0x45               goto 17
	0x65               goto 17

	match 7


state 15
	0x00 - 0x09 (10)   goto 15
	0x0b - 0xff (245)  goto 15

	match 2


state 16
	0x2b               goto 18
	0x2d               goto 18
	0x30 - 0x39 (10)   goto 19


state 17
	0x2b               goto 20
	0x2d               goto 20
	0x30 - 0x39 (10)   goto 21


state 18
	0x30 - 0x39 (10)   goto 19


state 19
	0x30 - 0x39 (10)   goto 19

	match 6


state 20
	0x30 - 0x39 (10)   goto 21


state 21
	0x30 - 0x39 (10)   goto 21

	match 5


#############################################################################
# Summary
#############################################################################

1 start state(s)
11 expression(s), 21 state(s)


#############################################################################
# End of File
#############################################################################
