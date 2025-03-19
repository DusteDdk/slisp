#include <format>

#include "token.h"

std::string tokName(Token t) {
	switch (t) {
	case Token::Loop:
		return "Token::Loop";
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
	case Token::IBegin:
		return "Token::IBegin";
	case Token::IEnd:
		return "Token::IEnd";
	case Token::LBegin:
		return "Token::LBegin";
	case Token::LEnd:
		return "Token::LEnd";
	case Token::Eof:
		return "Token::Eof";
	case Token::SyntaxError:
		return "Token::SyntaxError";
	case Token::Name:
		return "Token::Name";
	case Token::NoOP:
		return "Token::NoOP";
	}

	return  std::format("Unknown::Token({:X})", (int)t);
}
