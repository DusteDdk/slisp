#include "toker.h"

#include <print>

using namespace std;


bool Toker::getC() {
	if (replay) {
		replay = false;
		return true;
	}

	if (generated.length()) {
		c = generated[0];
		generated = generated.substr(1);
		return true;
	}

	return !!in.get(c);
}

bool Toker::consumeWhitespace(bool breakOnNewLine) {
	while (isspace(c)) {
		if (!in.get(c)) {
			return false;
		}

		if (breakOnNewLine && (c == '\r' || c == '\n')) {
			return true;
		}
	}

	return true;
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
			if (!consumeWhitespace(false)) {
				return flush(Token::Eof);
			}
		}
		switch (c) {
		case ':':
			return flush(Token::Name);
		case '@':
			return flush(Token::Loop);
		case '/': //A comment
			if (acc.length() && acc[0] == '/') {
				acc = "";
				str = "";
				if ( !getC() || !consumeWhitespace(true)) {
					return Token::Comment;
				}
				do {
					str += c;
				} while (getC() && c != '\n' && c != '\r');
				return Token::Comment;
			}
			break;

		case '{':
			generated = std::format("imp {}", generated); // ? is it here?
			[[fallthrough]];
		case '(':
			return flush(Token::Begin);

		case '}': // } does nothing different at all
		case ')':
			return flush(Token::End);
		case '"':
			// identifier"string" should be interpreted as identifer "string"
			if (acc.length()) {
				return flush(Token::Identifier);
			}
			str= "";
			while (in.get(c)) {
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
			return Token::UnExpEof;
		}

		// Numbers must either start with a digit, or have a whitespace before the dot.
		// Because I want to reserve dot right against an identifier and other tokens too.
		bool hasDot = (c == '.');
		if (!acc.length() && (isdigit(c) || hasDot)) {
			str = "";
			do {
				str += c;
				if (!in.get(c)) {
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

