#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED
#include <string>
#include <cstdint>

enum class Token : int_fast8_t {
	Loop=1,
	Skip,
	Identifier,
	Number,
	String,
	Begin,
	End,
	Eof,
	UnExpEof,
	SyntaxError,
	Comment,
	Name,
};

std::string tokName(Token t);

#endif
