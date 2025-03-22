#include <print>
#include "toker.h"
#include <format>

using namespace std;



Toker::Toker(std::istream &inStream) : in(inStream)
{
    reset("<?>");
}


void Toker::reset(std::string fname)
{
	blockReason=BlockReason::Unblocked;
	c = 0;
	pline = 1;
	cline = 1;
	pcolumn = 0;
	ccolumn = 0;
	fileName=fname;
	waitingTokens.clear();
}

void Toker::queueToken(Token t)
{
	auto ti = TokenInfo{
		.token = t,
		.str = acc,
		.file=fileName,
		.line = pline,
		.column = pcolumn,
	};

	//std::println("queued token: {}", TokInfoStr(ti));

	waitingTokens.push_back(ti);
	pline=cline;
	pcolumn=ccolumn;
	acc="";
}

TokenInfo Toker::dequeueToken()
{
	if(!waitingTokens.size()) {
		std::println("System Error: tried to dequeue token from empty token queue!");
	}

	TokenInfo ti = waitingTokens.front();
	waitingTokens.pop_front();

	return ti;
}

TokenInfo Toker::token(Token t, std::string txt) {
	acc = txt;
	queueToken(t);
	return dequeueToken();
}

TokenInfo Toker::token(Token t)
{
    queueToken(t);
	return dequeueToken();
}

bool Toker::getC() {

	if( in.eof() ) {
		return false;
	}

	if (c == '\n') {
		cline++;
		ccolumn = 1;
	}
	if (c != '\n' && c != '\r') {
		if(c != END_OF_TEXT) {
			ccolumn++;
		}
	}

	bool retVal = !!in.get(c);

	isUnic=false;
	unic = "";
	unic += c;

	if( retVal && (c & 0b11000000) == 0b11000000 ) {
		char n;
		isUnic=true;

		// Read next byte (byte 2)
		retVal = !!in.get(n);
		if(!retVal || (n & 0b11000000) != 0b10000000) {
			std::println("UTF8 error while decoding byte 1, line:{} column:{} Byte 0:{:x} Byte 1:{:x} ", cline, ccolumn, (uint8_t)c, (uint8_t)n);
			return false;
		}
		unic += n;

		// More bytes ?
		if( (c & 0b11100000) == 0b11100000 ) {
			//Read next byte (byte 2)
			retVal = !!in.get(n);
			if(!retVal || (n & 0b11000000) != 0b10000000) {
				std::println("UTF8 error while decoding byte 2, line:{} column:{} Byte 1:{:x} Byte 2:{:x} ", cline, ccolumn, (uint8_t)c, (uint8_t)n);
				return false;
			}
			unic += n;

			// More bytes ?
			if( (c & 0b11110000) == 0b11110000 ) {
				// Read next byte(byte3)
				retVal = !!in.get(n);
				if(!retVal || (n & 0b11000000) != 0b10000000) {
					std::println("UTF8 error while decoding byte 3, line:{} column:{} Byte 2:{:x} Byte 3:{:x} ", cline, ccolumn, (uint8_t)c, (uint8_t)n);
					return false;
				}
				unic += n;
			}
		}
		c=0;
	}

	return retVal;
}




bool shouldEmitIdent(char c) {
	if(isspace(c)) {
		return true;
	}

	switch(c) {
		case END_OF_TEXT:
		case '@':
		case '[':
		case '{':
		case '(':
		case ']':
		case '}':
		case ')':
		case '"':
		return true;
		default:
	}
	return false;
}

TokenInfo Toker::nextToken() {

	blockReason=BlockReason::Unblocked;

	if(waitingTokens.size()) {
		std::println(">>> dequeue in nextToken()");
		return dequeueToken();
	}

	if (in.eof()) {
		return eof;
	}

	pline=cline;
	pcolumn=ccolumn;


	blockReason=BlockReason::Waiting;
	while ( dontConsumeChar || getC()) {


		if(waitingTokens.size()) {
			std::println(">>> dequeue in while()");
			return dequeueToken();
		} else if( acc.length() && shouldEmitIdent(c) ) {
			dontConsumeChar=true;
			return token(Token::Identifier);
		}

		dontConsumeChar=false;

		if(isspace(c)) {
			continue;
		}

		// And finally look for stuff of interest
		switch (c) {

		case END_OF_TEXT:
			return token(Token::NoOP);
		case ':':
		{
			if(!getC()) {
				return token(Token::SyntaxError, "SyntaxError: Unexpected end of file after ':'");
			}
			// These are set for identifiers, but many others won't set it, calculating it is luckily easy
			pline = cline;
			pcolumn = ( ccolumn - 1 - acc.length() ); // Works for both when there is is something accumulated and when there is not.

			switch(c) {
				case ':':
					return token(Token::KnownName);
				case '?':
					return token(Token::NameQuery);
				default:
				{
					dontConsumeChar=true;
					return token(Token::Name);
				}
			}
		}
		case '@':
			return token(Token::Loop);

		//Possibly a comment.
		case '/':
		case '*':
		{
			if(acc == "/") {
				if(c=='*') {
					blockReason=BlockReason::Comment;
					int blockDepth=1; // Not being able to next block comments always annoyed me.
					acc="  ";
					while(blockDepth && getC() ) {
						acc[0]=acc[1];
						acc[1]=c;
						if(acc=="/*") {
							blockDepth++;
							acc="  ";
						} else if(acc=="*/") {
							blockDepth--;
							acc="  ";
						}
					}

					if(in.eof() && blockDepth) {
						return token(Token::SyntaxError, std::format("SyntaxError: Unterminated block comment (depth: {}) started at line:{} colum:{}", blockDepth, pline, pcolumn));
					}
					acc="";
					continue; // Comment ended, continue outer loop without token emission
				} else if(c=='/') {
					blockReason=BlockReason::Comment;

					while( getC() ) {
						if(c=='\n') {
							break;
						}
					}
					acc="";
					continue; // Comment ended, continue outer loop without token emission
				}
			}
			break;
		}
		case '[':
			return token(Token::LBegin);
		case '{':
			return token(Token::IBegin);
		case '(':
			return token(Token::Begin);

		case ')':
			return token(Token::End);
		case '}':
			return token(Token::IEnd);
		case ']':
			return token(Token::LEnd);

		case '"':
			// identifier"string" should be interpreted as identifer "string"
			// Like, it should be valid to have no separators between an identifier and a string.
			if (acc.length()) {
				queueToken(Token::Identifier);
			}
			blockReason=BlockReason::String;
			while (getC()) {
				if (c == '"') {
					if (acc.length() && acc.back() == '\\') {
						acc.pop_back();
					}
					else {
						return token(Token::String);
					}
				}
				acc += c;
			}

			return token(Token::SyntaxError, "SyntaxError: Unterminated string.");
		}

		// Capture start of identifier
		if (!acc.length()) {
			pline = cline;
			pcolumn = ccolumn;
		}

		// Valid number decls: .1 -.1 1 -1
		bool hasDot = (c == '.');
		bool isNegative = (c == '-');
		bool hasDigit=isdigit(c);
		if ( !acc.length() && ( hasDot || isNegative || hasDigit) ) {

			acc = c;
			blockReason=BlockReason::Number;
			while( getC() ) {
				if (c == '.') {
					if (hasDot) {
						return token(Token::SyntaxError, std::format("SyntaxError: Multiple dots in number '{}'", acc));
					}
					hasDot = true;
				} else if( isdigit(c) ) {
					hasDigit=true; // a number must have some digits.
				} else {
					break;
				}
				acc += c;
			}

			dontConsumeChar=true;
			if(hasDigit) {
				return token(Token::Number); //maybe queue instead?
			}
			continue;
		}

		// If we reach this, it's an identifier (or becomes a variable)
		blockReason=BlockReason::Identifier;
		acc += c;
	}

	eof.token=Token::Eof;
	eof.line=cline;
	eof.column=ccolumn;
	eof.str="";
	eof.file=fileName;

	return nextToken();
}


std::string BlockReasonStr (BlockReason r) {
	switch(r) {
		case BlockReason::Unblocked:
			return "Unblocked";
		case BlockReason::Waiting:
			return "Waiting";
		case BlockReason::String:
			return "String";
		case BlockReason::Number:
			return "Number";
		case BlockReason::Comment:
			return "comment";
		case BlockReason::Identifier:
			return "Identifier";
		default:
			return "Unknown";
	}
}

std::string TokInfoStr(TokenInfo& t) {
	if(t.str.length()) {
		return std::format("{}<{}> @ {}:{}:{}",tokName(t.token), t.str.substr(0,4), t.file, t.line, t.column);
	}
	return std::format("{} @ {}:{}:{}",tokName(t.token), t.file, t.line, t.column);
}


TokenProvider::TokenProvider(std::istream &inStream, std::string fname): toker(inStream) {
	reset(fname);
}

void TokenProvider::reset(std::string fname) {
	toker.reset(fname);
	curToken.column=0;
	curToken.line=0;
	curToken.str="";
	curToken.token=Token::NoOP;
	nxtToken.column=0;
	nxtToken.line=0;
	nxtToken.str="";
	nxtToken.token=Token::NoOP;
}




bool TokenProvider::advance()
{
	if(curToken.token == Token::Eof) {
		return false;
	}

	curToken = nxtToken;
	nxtToken = toker.nextToken();

    return true;
}

bool TokenProvider::advanceSkipNoOp()
{
	bool more = false;
	do {
		more = advance();
	}
	while(curToken.token == Token::NoOP);
	return more;
}

BlockReason TokenProvider::getBlockReason()
{
    return toker.blockReason;
}
