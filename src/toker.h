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
	Token flush(Token t);
	bool peeked = false;
	std::string acc;
	Token peekToken;
	std::string peekStrBackup;
	public:
	Toker(std::istream& inStream) : in(inStream), acc(""), peekToken(Token::Eof) { c = 0; line=1; column=0; };

	std::string str;

	int line, column;

	Token nextToken();
	Token peek();
};

#endif