#ifndef TOKER_H_INCLUDED
#define TOKER_H_INCLUDED
#include <istream>
#include "token.h"

class Toker {
	std::istream& in;
	bool eof=false;
	bool replay = false;
	char c=0;
	bool getC();
	bool consumeWhitespace(bool breakOnNewLine);
	Token flush(Token t);
	bool peeked = false;
	std::string acc;
	Token peekToken;
	std::string generated;
public:
	Toker(std::istream& inStream) : in(inStream), acc(""), peekToken(Token::Skip), generated("") { c = 0; };

	std::string str;

	Token nextToken();
	Token peek();
};

#endif