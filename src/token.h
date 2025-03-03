#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED
#include <string>
#include <cstdint>

enum class Token : int_fast8_t {
	Loop=1,
	Identifier,
	Number,
	String,
	IBegin,
	IEnd,
	LBegin,
	LEnd,
	Begin,
	End,
	Eof,
	SyntaxError,
	Name,
};

std::string tokName(Token t);

#endif
