#include "token.h"

std::string tokName(Token t) {
	switch (t) {
	case Token::Loop:
		return "Token::Loop";
	case Token::Skip:
		return "Token::Skip";
	case Token::Identifier:
		return "Token::Identifier";
	case Token::Number:
		return "Token::Number";
	case Token::String:
		return "Token::String";
	case Token::Begin:
		return "Token::Begin";
	case Token::End:
		return "Token::End";
	case Token::Eof:
		return "Token::Eof";
	case Token::UnExpEof:
		return "Token::UnExpEof";
	case Token::Comment:
		return "Token::Comment";
	case Token::SyntaxError:
		return "Token::SyntaxError";
	case Token::Name:
		return "Token::Name";
	}
	return  "Unknown::Token";
}
