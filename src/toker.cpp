#include <print>
#include "toker.h"
#include <format>

using namespace std;

Toker::Toker(std::istream& inStream) : in(inStream), acc("") {
	c = 0;
	line = 1;
	rline = 1;
	column = 0;
	rcolumn = 0;
}

bool Toker::getC() {

	if (replay) {
		replay=false;
		return true;
	}

	if (!in.eof()) {
		if (c == '\n') {
			rline++;
			rcolumn = 1;
		}
		if (c != '\n' && c != '\r') {
			if(c != END_OF_TEXT) {
				rcolumn++;
			}
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
			std::println("UTF8 error while decoding byte 1, line:{} column:{} Byte 0:{:x} Byte 1:{:x} ", line, column, (uint8_t)c, (uint8_t)n);
			return false;
		}
		unic += n;

		// More bytes ?
		if( (c & 0b11100000) == 0b11100000 ) {
			//Read next byte (byte 2)
			retVal = !!in.get(n);
			if(!retVal || (n & 0b11000000) != 0b10000000) {
				std::println("UTF8 error while decoding byte 2, line:{} column:{} Byte 1:{:x} Byte 2:{:x} ", line, column, (uint8_t)c, (uint8_t)n);
				return false;
			}
			unic += n;

			// More bytes ?
			if( (c & 0b11110000) == 0b11110000 ) {
				// Read next byte(byte3)
				retVal = !!in.get(n);
				if(!retVal || (n & 0b11000000) != 0b10000000) {
					std::println("UTF8 error while decoding byte 3, line:{} column:{} Byte 2:{:x} Byte 3:{:x} ", line, column, (uint8_t)c, (uint8_t)n);
					return false;
				}
				unic += n;
			}
		}
		c=0;
	}

	return retVal;
}


Token Toker::emitStrTok(Token t) {
	str = acc;
	acc = "";
	return t;
}

// Identifiers are special tokens, they're "everything that's not whitespace or consumed into other tokens"
// So we emit them whenever we've accumulated something that fits that category, until whitespace or something else to consume.
// Meaning we have to "push" them out at weird places, we do that by holding back the current character, emit the identifier, and then
// next time, we're ready for a new token.
Token Toker::flush(Token t) {
	if (acc.length()) {
		replay=true;
		return emitStrTok(Token::Identifier);
	}

	line = rline;
	column = rcolumn;

	str="";
	acc="";
	return t;
}

Token Toker::nextToken() {

	if (in.eof()) {
		return Token::Eof;
	}

	isMidToken = false;
	while (getC()) {

		if (isspace(c)) {
			if (acc.length()) {
				return emitStrTok(Token::Identifier);
			}
			continue;
		}
		switch (c) {

		case END_OF_TEXT:
			return flush(Token::NoOP);
		case ':':
		{
			if(!getC()) {
				str = "SyntaxError: Unexpected end of file after ':'";
				return Token::SyntaxError;
			}
			// These are set for identifiers, but many others won't set it, calculating it is luckily easy
			line = rline;
			column = ( rcolumn - 1 - acc.length() ); // Works for both when there is is something accumulated and when there is not.
			switch(c) {
				case ':':
					return emitStrTok(Token::KnownName); // Don't replay
				case '?':
					return emitStrTok(Token::NameQuery);
				default:
					replay=true;
					return emitStrTok(Token::Name); // replay
			}
		}
		case '@':
			return flush(Token::Loop);

		//Possibly a comment.
		case '/':
		case '*':
		{
			if(acc == "/") {
				if(c=='*') {
					int blockDepth=1; // Not being able to next block comments always annoyed me.
					acc="  ";
					isMidToken=true;
					while(blockDepth && getC() ) {
						acc[0]=acc[1];
						acc[1]=c;
						if(acc=="/*") {
							blockDepth++;
						} else if(acc=="*/") {
							blockDepth--;
						}
					}

					isMidToken=false;
					if(in.eof() && blockDepth) {
						str = std::format("SyntaxError: Unterminated block comment (depth: {}) started at line:{} colum:{}", blockDepth, line, column);
						return Token::SyntaxError;
					}
					acc="";
					continue; // Comment ended, continue outer loop without token emission
				} else if(c=='/') {
					isMidToken=true;
					while( getC() ) {
						if(c=='\n') {
							break;
						}
					}
					acc="";
					isMidToken=false;
					continue; // Comment ended, continue outer loop without token emission
				}
			}
			break;
		}
		case '[':
			return flush(Token::LBegin);
		case '{':
			return flush(Token::IBegin);
		case '(':
			return flush(Token::Begin);

		case ')':
			return flush(Token::End);
		case '}':
			return flush(Token::IEnd);
		case ']':
			return flush(Token::LEnd);

		case '"':
			// identifier"string" should be interpreted as identifer "string"
			if (acc.length()) {
				return flush(Token::Identifier);
			}
			isMidToken=true;
			str= "";
			line = rline;
			column = rcolumn;
			while (getC()) {
				if (c == '"') {
					if (str.length() && str.back() == '\\') {
						str.pop_back();
					}
					else {
						isMidToken=false;
						return Token::String;
					}
				}
				str += c;
			}
			str = std::format("SyntaxError: Unterminated string: {}{}", str.substr(0,10), (str.length()>10)?"...":".");
			isMidToken=false;
			return Token::SyntaxError;
		}

		// Capture start of identifier
		if (!acc.length()) {
			line = rline;
			column = rcolumn;
		}

		// Valid number decls: .1 -.1 1 -1
		bool hasDot = (c == '.');
		if (!acc.length() && (isdigit(c) || hasDot || ( c== '-'))) {

			do {
				acc += c;
				if (!getC()) {
					return Token::Eof;
				}

				if (c == '.') {
					if (hasDot) {
						str = std::format("SyntaxError: Multiple dots in number '{}'", str);

						return Token::SyntaxError;
					}
					hasDot = true;
				}
			} while (isdigit(c) || c == '.');
			if(acc == ".") {
				acc="";
				str = std::format("Error: Dot not followed by a digit is not yet a valid syntax,");
				return Token::SyntaxError;
			}

			replay=true;

			if( acc == "-" ) {
				// - is a valid identifier, so in case we entered the loop due to that condition being, but then exiting after reading the next token, we have correctly accumulated it
				continue; // Replay is set, this time we no longer match the "-" condition.
			}

			return emitStrTok(Token::Number);
		}

		// If nothing else matched, then we add the character to the accumulating string, it is destined as an identifier if not recognized as a comment or something else.
		acc += c;
	}

	return flush(Token::Eof);
}

std::string TokInfoStr(TokenInfo& t) {
	if(t.str.length()) {
		return std::format("{}<{}> @ {}:{}:{}",tokName(t.token), t.str.substr(0,4), t.file, t.line, t.column);
	}
	return std::format("{} @ {}:{}:{}",tokName(t.token), t.file, t.line, t.column);
}


TokenProvider::TokenProvider(std::istream &inStream, std::string fname): toker(inStream), fileName(fname), isMidToken( &toker.isMidToken) {
	reset(fileName);
}

void TokenProvider::reset(std::string fname) {
	fileName=fname;
	curToken.column=0;
	curToken.line=0;
	curToken.file=fileName;
	curToken.str="";
	curToken.token=Token::NoOP;
	nxtToken.column=0;
	nxtToken.line=0;
	nxtToken.file=fileName;
	nxtToken.str="";
	nxtToken.token=Token::NoOP;
}

TokenInfo TokenProvider::readNextToken() {
	TokenInfo ti;
	ti.token = toker.nextToken();
	ti.line = toker.line;
	ti.column = toker.column;
	ti.str = toker.str;
	ti.file = fileName;
	return ti;
}



bool TokenProvider::advance()
{
	if(curToken.token == Token::Eof) {
		return false;
	}

	curToken = nxtToken;
	nxtToken = readNextToken();

    return true;
}
