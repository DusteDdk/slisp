#ifndef TOKER_H_INCLUDED
#define TOKER_H_INCLUDED
#include <istream>
#include "token.h"

class Toker {
	int serial = 0;
	std::istream& in;
	bool replay = false;
	char c=0;
	std::string unic="";
	bool isUnic=false;
	bool getC();
	Token flush(Token t);
	bool peeked = false;
	std::string acc;
	Token peekToken;
	std::string peekStrBackup;
	int rline, rcolumn;

	public:
		Toker(std::istream& inStream);

	std::string str;

	int line, column;


	Token nextToken();
	Token peek();
};

#endif
