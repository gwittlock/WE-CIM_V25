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
# CmdParser.v
# YACC verbose file generated from CmdParser.y.
# 
# Date: 12/01/04
# Time: 08:30:04
# 
# AYACC Version: 2.07
#############################################################################


##############################################################################
# Rules
##############################################################################

    0  $accept : command $end

    1  command : routes
    2          | routes parameters

    3  routes :
    4         | routes route

    5  parameters : parameter
    6             | parameters ',' parameter

    7  route : IDENTIFIER ':'

    8  parameter : IDENTIFIER '=' value

    9  value : number
   10        | text

   11  number : INT
   12         | REAL

   13  text : STRING
   14       | IDENTIFIER


##############################################################################
# States
##############################################################################

state 0
	$accept : . command $end
	routes : .  (3)

	.  reduce 3

	command  goto 1
	routes  goto 2


state 1
	$accept : command . $end  (0)

	$end  accept


state 2
	command : routes .  (1)
	command : routes . parameters
	routes : routes . route

	IDENTIFIER  shift 3
	.  reduce 1

	parameters  goto 4
	route  goto 5
	parameter  goto 6


state 3
	route : IDENTIFIER . ':'
	parameter : IDENTIFIER . '=' value

	':'  shift 7
	'='  shift 8


state 4
	command : routes parameters .  (2)
	parameters : parameters . ',' parameter

	','  shift 9
	.  reduce 2


state 5
	routes : routes route .  (4)

	.  reduce 4


state 6
	parameters : parameter .  (5)

	.  reduce 5


state 7
	route : IDENTIFIER ':' .  (7)

	.  reduce 7


state 8
	parameter : IDENTIFIER '=' . value

	IDENTIFIER  shift 10
	STRING  shift 11
	INT  shift 12
	REAL  shift 13

	text  goto 14
	value  goto 15
	number  goto 16


state 9
	parameters : parameters ',' . parameter

	IDENTIFIER  shift 17

	parameter  goto 18


state 10
	text : IDENTIFIER .  (14)

	.  reduce 14


state 11
	text : STRING .  (13)

	.  reduce 13


state 12
	number : INT .  (11)

	.  reduce 11


state 13
	number : REAL .  (12)

	.  reduce 12


state 14
	value : text .  (10)

	.  reduce 10


state 15
	parameter : IDENTIFIER '=' value .  (8)

	.  reduce 8


state 16
	value : number .  (9)

	.  reduce 9


state 17
	parameter : IDENTIFIER . '=' value

	'='  shift 8


state 18
	parameters : parameters ',' parameter .  (6)

	.  reduce 6


##############################################################################
# Summary
##############################################################################

9 token(s), 9 nonterminal(s)
15 grammar rule(s), 19 state(s)


##############################################################################
# End of File
##############################################################################
