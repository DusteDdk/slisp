#ifndef DEBUG_SOURCE_H
#define DEBUG_SOURCE_H

// Use a raw string literal to store the text
// This might be source code or any other text you need
static const char test_source[] = R"(
//Weird little lisp with imperative tendencies
// Comments are C style because I like those, no you can't ; or /**/
// Builtin functions:
//	imp - imperative, evaluate each item in order
//	print - prints its args to stdout, forwards $
//  sprint - prints its args to a string
//	read - read from stdin until the string in the first item of the list is matched
//	+ - * / - What you'd expect, the first number in the list is the left operand, the rest of the numbers are the right operand: - 8 4 2 1 = 8 - 4 - 2 -1
// @ - Rewind receiving imp function
// ? - conditional if this then else - Returns whatever the target evaluated to
// < > - less than, greater than
// = - Equality check - Returns True(value that was compared against) or False(first value that caused the comparison to fail)
// Implicities:
//	Lists evaluate to the last item in them
//	Evaluating an item in an imp places the evaluation result in $
// Builtin variables:
// nl - Platforms newline sequence
// $ - The godhead (someone once said "programming is like talking with god")
// Syntax:
// { is syntatic sugar for (imp, } is ), they don't have to match..
// ( begins and ) ends a list definition
// ( fname [arg [arg [arg]]]) Calls function of fname with a number of arguments ([ and ] are not entered, they are to show optionality)
// " starts quoting as string, use " again to stop it
// It's a number if it starts with a digit, there can be a dot as decimal dot, more than one dot is a syntax error
// varName: - creates or sets variable "varName" in current scope, with value of following item - head passes through
// varName:: - sets existing variable "varName" in nearest scope, must exist, errors if not.
// varName:? - Returns true if varName exists, false otherwise
// (call...): - Sets variable named after return value of call, head skips over call. So you can (imp 1 (numToString):$ ) and expect the value to be set to 1.

{
	(print "I can do math: ")
	(print (/ (+ 1 2 3 4) 2) nl)
	(print "Who _ARE_ you? ")
	uName: (read)
	{
		backup: $
		uName: (sprint "Local_" uName "_scoped")
		(print "Shadowed variable: " uName nl)
		backup
	}
	(print "Hello '" $ "'" nl)
	(print "Escape test: \"" uName "\"" nl)
	41.5
	(+ .5 $)
	(print "Oh yeah? We can do that " $ nl)
	(print "Some comparison tests" nl)
	"a"
	(? (= "a") (print "a = a" nl) (print "a not a" nl))
	(? (= "a" "a") (print "a = a" nl) (print "a not a" nl))
	(? (= "a" "b") (print "a = b" nl) (print "a not b" nl))
	0
	(? (= 0) (print "0 = 0" nl) (print "0 not 0" nl))
	(? (= 1 2) (print "1 = 2" nl) (print "1 not 2" nl))
	(? (= 2 2) (print "2 = 2" nl) (print "2 not 2" nl))

	(print "Div test: " ( / 1 ( - 0 2 )  ) nl)

	
	4
	{loop:$
		var2:?
		var2:
		(? $
			(imp
				(print "var2 exists: " var2 nl)
				var2:: (+ var2 10 ))
			(imp
				(print "var2 does not exist, yet.." nl)
			0))

		loop: (- loop 1)
		(? (> 0 ) @)
	}

	2
	(print $ nl)
	(print ( < 6 ) nl)
	(print ( < 6 6 ) nl)
	7
	(print $ nl)
	(print ( < 6 ) nl)
	(print ( < 7 6 ) nl)

	(print $ nl)
	(print ( > 6 ) nl)
	(print ( > 6 3) nl)
	(print ( > 6 6 ) nl)
	2	
	(print $ nl)
	(print ( > 6 ) nl )
	(print ( > 6 3) nl)

	var: (+ 2 2)

	(print "A loop:" nl)
	5
	(imp
		(print "  Loop:  " $ nl)
		(- 1)
		(? (! (= 0)) (imp $ @))
		(print "Loop ends>" $ nl)
	)
	(print "The previously set variable is " var nl)
	(print "> " $ nl)
	(print "One more time, " uName " ? [y/N]")
	(read)
	(? (= $ "y")
		(imp
			(print "Okay, lets do anoother one!" nl nl)
		@) // Jump back to start of closest imp
		(print "Okay, was nice seeing you, later!" nl)
	)
	0 // Return 0 to os

	(print "Enter a number: ")
	numa: (read)
	(print nl "Enter another number: ")
	numb: (read)
	(print nl "You entered: " (+ numa numb))


}
)";

#endif
