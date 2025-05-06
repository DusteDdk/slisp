#ifndef TOKER_H_INCLUDED
#define TOKER_H_INCLUDED

#include <list>
#include <format>
#include <istream>
#include "token.h"

#define END_OF_TEXT (char)3

enum class BlockReason : int_fast8_t {
	Unblocked=0,
	Waiting=1,
	String,
	Number,
	Comment,
	Identifier
};

std::string BlockReasonStr (BlockReason r);

struct TokenInfo {
	Token token=Token::NoOP;
	std::string str="";
	std::string file="";
	int line=0, column=0;
	int eline=0, ecolumn=0;
};
class Toker {
	std::istream& in;
	char c=0;
	std::string unic="";
	bool isUnic=false;
	bool getC();
	int cline, ccolumn;
	int pline, pcolumn;
	TokenInfo eof;
	std::string acc;
	std::string fileName;
	int getCCalls=0;

	bool dontConsumeChar=false;
	TokenInfo token(Token t);
	TokenInfo token(Token t, std::string txt);
	void advanceTi();

	public:
	Toker(std::istream& inStream, std::string& fname);
	void reset(std::string fname);

	BlockReason blockReason;
	TokenInfo nextToken();
};


std::string TokInfoStr(TokenInfo& t);

class TokenProvider {
private:
    Toker toker;
public:
    TokenProvider(std::istream& inStream, std::string& fname);
    bool advance();
    bool advanceSkipNoOp();
    BlockReason getBlockReason();
    void reset(std::string& fname);
    TokenInfo curToken;
    TokenInfo nxtToken;
};

#endif
