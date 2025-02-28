#include <format>
#include <print>
#include <utility>
#include "token.h"
#include "parsey.h"

std::string Node::toString() {
	return std::format("::Node(Type{})::", (int)t);
}

std::string NodeErr::toString() {
	return std::format("Error: {}", str);
}

template<class T, class U>
std::shared_ptr<T> mkNode(U&& u) {
	return std::make_shared<T>(std::forward<U>(u));
}

void Parsey::skipToNextSemanticToken(Token& t) {

	while (t == Token::Comment || t == Token::Skip) {
		log("   skip: {} / {} ", tokName(t), toker.str);
		t = toker.nextToken();

	}
}

template <typename... Args> void Parsey::log(std::format_string<Args...> fmt, Args&&... args) const {
	if (verbose) {
		std::println(fmt, std::forward<Args>(args)...);
	}
}

NodeRef Parsey::parseNode(Token t) {

	skipToNextSemanticToken(t);

	if (t == Token::Eof) {
		log("EOF");
		return mkNode<Node>(NodeType::Stop);
	}
	if (t == Token::UnExpEof) {
		log("UnexpectedEnd");
		return mkNode<NodeErr>("Unexpected EOF");
	}

	switch (t)
	{

	case Token::Name: // Token (named by $)
	{
		auto nsv = std::make_shared<NodeVariable>();
		auto next = toker.peek();
		if (next == Token::Name) { // It's the "known" variant if it's followed by another.
			toker.nextToken();
			nsv->isKnownName = true;
		}
		else if (next == Token::Identifier && toker.str == "?") {
			toker.nextToken();
			nsv->isQuery = true;
		}
		return nsv;
	}
		break;
	case Token::Identifier: // And directly named  names
	{
		std::string identStr = toker.str;
		// Check if this identifier is followed by one or two : in which case it's the name of a variable or known variable.
		auto next = toker.peek();
		if (next == Token::Name) {
			toker.nextToken(); // consume it

			auto nsv = std::make_shared<NodeVariable>(identStr);
			auto next = toker.peek();
			if (next == Token::Name) { // It's the "known" variant if it's followed by another.
				toker.nextToken();
				nsv->isKnownName = true;
			}
			else if (next == Token::Identifier && toker.str == "?") {
				toker.nextToken();
				nsv->isQuery = true;
			}
			return nsv;
		}
		return mkNode<NodeIdent>(identStr);
	}
	case Token::Number:
	{
		char* end{};
		long double num = std::strtold(toker.str.c_str(), &end);
		if (*end != 0 || errno != 0) {
			log("Num error {} ", toker.str);
			return mkNode<NodeErr>(std::format("ParserError: Failed to parse number '{}'", toker.str));
		}
		log("Num: {} (from string '{}')", num, toker.str);
		return mkNode<NodeNum>(num);
	}
	case Token::String:
		log("String '{}'", toker.str);
		return mkNode<NodeStr>(toker.str);
		break;
	case Token::Loop:
	{
		Token t = toker.peek();
		if (t != Token::End) {
			return mkNode<NodeErr>("SyntaxError: @ only allowed as last element in list");
		}
		return mkNode<Node>(NodeType::Loop);
	}
	case Token::Begin:
	{
		auto list = std::make_shared<NodeCall>();
		log("Call.");
		Token t = toker.nextToken();
		if (t != Token::Identifier) {
			log("  SyntaxError: Expected Identifier, got {}", tokName(t));
			return mkNode<NodeErr>(std::format("Syntax error: Expected Identifer, got {}", tokName(t)));
		}

		list->name = toker.str;

		log("CallBodyBegin({}):", list->name);
		while (true) {
			t = toker.nextToken();
			skipToNextSemanticToken(t);

			if (t == Token::End) {
				break;
			}

			if (t == Token::Eof || t == Token::UnExpEof) {
				log("SyntaxError: CallBody for {} body not closed.", list->name);
				return mkNode<NodeErr>("Syntax error, list not closed.");
			}

			NodeRef n = parseNode(t);
			list->args.push_back(n);
		}
		log("CallBodyEnd({}): {} arguments", list->name, list->args.size());
		return list;
	}
	case Token::End:
		log("SyntaxError: Stray )");
		return mkNode<NodeErr>("SyntaxError: Stray )");
		
	case Token::Eof:
		log("EOF (in switch/case, can this happen?)");
		return mkNode<Node>(NodeType::Stop);
	case Token::UnExpEof:
		log("Unexpected EOF (in switch/case, can this happen?)");
		return mkNode<NodeErr>("Syntax Error: Unexpected End of File.");

	case Token::Skip:
	case Token::SyntaxError:
	case Token::Comment:
	default:
	break;

	}

	log("Unhandled token {} {}",(int)t, tokName(t));
	return mkNode<NodeErr>(std::format("System Error: Unhandled {}", tokName(t)));
}

Parsey::Parsey(Toker toker, bool verbose) : toker(toker), verbose(verbose) {
	Token t = toker.nextToken();
	program = parseNode(t);

};
