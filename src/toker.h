#ifndef TOKER_H_INCLUDED
#define TOKER_H_INCLUDED
#include <format>
#include <istream>
#include "token.h"

#define END_OF_TEXT (char)3

class Toker {
	int serial = 0;
	std::istream& in;
	bool replay = false;
	char c=0;
	std::string unic="";
	bool isUnic=false;
	bool getC();
	Token flush(Token t);
	std::string acc;
	int rline, rcolumn;

	public:
		Toker(std::istream& inStream);

	std::string str;

	int line, column;


	Token nextToken();
};

struct TokenInfo {
	Token token;
	std::string str;
	std::string file;
	int line, column;
};

std::string TokInfoStr(TokenInfo& t);

class TokenProvider {
	private:
	Toker toker;
	std::string fileName;
	TokenInfo readNextToken();
	public:
		TokenProvider(std::istream& inStream, std::string fname);
		bool advance();
		void reset(std::string fname);
		TokenInfo curToken;
		TokenInfo nxtToken;
};

#endif
