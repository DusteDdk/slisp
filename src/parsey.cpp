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



NodeRef Parsey::setNodeInfo(NodeRef n) {
	n->line = toker.line;
	n->column = toker.column;
	return n;
}

NodeRef Parsey::parseNode(Token t) {

	if (t == Token::Eof) {
		return setNodeInfo(mkNode<Node>(NodeType::Stop));
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
		return setNodeInfo(nsv);
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
			return setNodeInfo(nsv);
		}
		return setNodeInfo(mkNode<NodeIdent>(identStr));
	}
	case Token::Number:
	{
		char* end{};
		long double num = std::strtold(toker.str.c_str(), &end);
		if (*end != 0 || errno != 0) {
			return setNodeInfo(mkNode<NodeErr>(std::format("ParserError: Failed to parse number '{}'", toker.str)));
		}
		return setNodeInfo(mkNode<NodeNum>(num));
	}
	case Token::String:
		return setNodeInfo(mkNode<NodeStr>(toker.str));
	case Token::Loop:
	{
		Token t = toker.peek();
		if (t != Token::End) {
			return setNodeInfo(mkNode<NodeErr>("SyntaxError: @ only allowed as last element in list"));
		}
		return setNodeInfo(mkNode<Node>(NodeType::Loop));
	}
	case Token::Begin:
	{
		auto list = std::make_shared<NodeCall>();
		Token t = toker.nextToken();
		if (t != Token::Identifier) {
			return setNodeInfo(mkNode<NodeErr>(std::format("Syntax error: Expected Identifer, got {}", tokName(t))));
		}
		list->ident = std::make_unique<NodeIdent>(toker.str);
		list->name = list->ident->str;

		while (true) {
			t = toker.nextToken();

			if (t == Token::End) {
				break;
			}

			if( t == Token::SyntaxError) {
				return setNodeInfo(mkNode<NodeErr>(std::format("Error in call body: {}", toker.str)));
			}
			if (t == Token::Eof) {
				return setNodeInfo(mkNode<NodeErr>("Syntax error, list not closed."));
			}

			NodeRef n = parseNode(t);
			list->args.push_back(n);
		}
		return list;
	}

	case Token::IBegin:
	{
		auto list = std::make_shared<NodeCall>();

		list->name = "imp";

		while (true) {
			t = toker.nextToken();

			if (t == Token::IEnd) {
				break;
			}

			if( t == Token::SyntaxError) {
				return setNodeInfo(mkNode<NodeErr>(std::format("Error in ilist body: {}", toker.str)));
			}

			if (t == Token::Eof) {
				return setNodeInfo(mkNode<NodeErr>("Syntax error, ilist not closed."));
			}

			NodeRef n = parseNode(t);
			list->args.push_back(n);
		}
		return list;
	}

	case Token::LBegin:
	{
		auto list = std::make_shared<NodeCall>();

		list->name = "list";

		while (true) {
			t = toker.nextToken();

			if (t == Token::LEnd) {
				break;
			}

			if( t == Token::SyntaxError) {
				return setNodeInfo(mkNode<NodeErr>(std::format("Error in list body: {}", toker.str)));
			}

			if (t == Token::Eof) {
				return setNodeInfo(mkNode<NodeErr>("Syntax error, ilist not closed."));
			}

			NodeRef n = parseNode(t);
			list->args.push_back(n);
		}
		return list;
	}

	case Token::End:
		return setNodeInfo(mkNode<NodeErr>("SyntaxError: Stray ')'"));
	case Token::IEnd:
		return setNodeInfo(mkNode<NodeErr>("SyntaxError: Stray '}'"));
	case Token::LEnd:
		return setNodeInfo(mkNode<NodeErr>("SyntaxError: Stray ']'"));
		
	case Token::Eof:
		return setNodeInfo(mkNode<Node>(NodeType::Stop));

	case Token::SyntaxError:
		return setNodeInfo(mkNode<NodeErr>(toker.str));

	}

	return setNodeInfo(mkNode<NodeErr>(std::format("System Error: Unhandled {}", tokName(t))));
}

Parsey::Parsey(Toker& toker) : toker(toker) {};
