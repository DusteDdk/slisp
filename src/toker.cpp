#include "toker.h"

#include <print>

using namespace std;


bool Toker::getC() {
	if (replay) {
		replay = false;
		return true;
	}

	bool isEof = !!in.get(c);

	if(c=='\n') {
			line++;
			column=0;
	}

	if( c!= '\n' && c != '\r') {
		column++;
	}

	//std::println("eof: {} Line {} Column: {} Character: {}", isEof, line, column, c);
	return isEof;
}


// Identifiers are special tokens, they're "everything that's not whitespace or consumed into other tokens"
// So we emit them whenever we've accumulated something that fits that category, until whitespace or something else to consume.
// Meaning we have to "push" them out at weird places, we do that by holding back the current character, emit the identifier, and then
// next time, we're ready for a new token.
Token Toker::flush(Token t) {
	if (acc.length()) {
		replay = true;
		str = acc;
		acc = "";
		return Token::Identifier;
	}
	return t;
}

// Warning: Peek overwrites str, so if you need to keep the current str, you MUST do it before calling.
Token Toker::peek() {
	if (!peeked) {
		peekToken = nextToken();
		peeked = true;
	}
	return peekToken;
}



Token Toker::nextToken() {

	// We might need one extra call in case there was something in acc before eof
	if (eof) {
		return Token::Eof;
	}

	if (peeked) {
		peeked = false;
		return peekToken;
	}

	while (getC()) {

		if (isspace(c)) {
			if (acc.length()) {
				return flush(Token::Identifier);
			}
			continue;
		}
		switch (c) {
		case ':':
			return flush(Token::Name);
		case '@':
			return flush(Token::Loop);
		case '/': //A comment
			if (acc.length() && acc[0] == '/') {
				acc="";
				do {
					str += c;
				} while (getC() && c != '\n' && c != '\r');
				replay=true;
				continue;
			}
			break;

		case '{':
			return flush(Token::IBegin);
		case '(':
			return flush(Token::Begin);

		case '}':
			return flush(Token::IEnd);
		case ')':
			return flush(Token::End);
		case '"':
			// identifier"string" should be interpreted as identifer "string"
			if (acc.length()) {
				return flush(Token::Identifier);
			}
			str= "";
			while (getC()) {
				if (c == '"') {
					if (str.length() && str.back() == '\\') {
						str.pop_back();
					}
					else {
						return Token::String;
					}
				}
				str += c;
			}
			str = std::format("SyntaxError: Unterminated string: {}{}", str.substr(0,10), (str.length()>10)?"...":".");
			return Token::SyntaxError;
		}

		// Numbers must either start with a digit, or have a whitespace before the dot.
		// Because I want to reserve dot right against an identifier and other tokens too.
		bool hasDot = (c == '.');
		if (!acc.length() && (isdigit(c) || hasDot)) {
			str = "";
			do {
				str += c;
				if (!getC()) {
					return Token::Eof;
				}

				if (c == '.') {
					if (hasDot) {
						str = std::format("Error: Two dots in one number ? '{}'", str);
						return Token::SyntaxError;
					}
					hasDot = true;
				}
			} while (isdigit(c) || c == '.');

			replay = true; // Start with this current c for next token instead of getting a new.
			return Token::Number;
		}

		acc += c;
	}
	eof = true;
	return flush(Token::Eof);
}

