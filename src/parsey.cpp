#include <format>
#include <print>
#include <utility>
#include "token.h"
#include "parsey.h"

std::string Node::toString() {
	return std::format("::Node(Type{})::", (int)t);
}

std::string NodeErr::toString() {
	return std::format("{} {}", TokInfoStr(origin), str);
}

template<class T, class U>
std::shared_ptr<T> mkNode(U&& u) {
	auto node =std::make_shared<T>(std::forward<U>(u));
	return node;
}

NodeRef setNodeInfo(TokenInfo& ti, NodeRef node) {
	node->origin = ti;
	return node;
}

NodeRef Parsey::parse(TokenInfo& ti) {
	Token inToken = ti.token;

	if (inToken == Token::Eof) {
		return setNodeInfo(ti, mkNode<Node>(NodeType::Base));
	}

	switch (inToken)
	{

	case Token::Name: // Token (named by $)
	{
		auto nsv = std::make_shared<NodeVariable>();
		auto next = top.nxtToken;
		if (next.token == Token::Name) { // It's the "known" variant if it's followed by another ':'
			top.advance();
			nsv->isKnownName = true;
		}
		else if (next.token == Token::Identifier && next.str == "?") {
			top.advance();
			nsv->isQuery = true;
		}
		return setNodeInfo(ti, nsv);
	}
		break;
	case Token::Identifier: // And directly named  names
	{

		// Check if this identifier is followed by one or two : in which case it's the name of a variable or known variable.
		auto next = top.nxtToken;
		if (next.token == Token::Name) {

			auto nsv = std::make_shared<NodeVariable>(ti.str);
			top.advance();
			auto next = top.nxtToken;
			if (next.token == Token::Name) { // It's the "known" variant if it's followed by another ':'
				top.advance();
				nsv->isKnownName = true;
			}
			else if (next.token == Token::Identifier && next.str == "?") {
				top.advance();
				nsv->isQuery = true;
			}
			return setNodeInfo(ti, nsv);
		}
		return setNodeInfo(ti, mkNode<NodeIdent>(ti.str));
	}
	case Token::Number:
	{
		char* end{};
		long double num = std::strtold(ti.str.c_str(), &end);
		if (*end != 0 || errno != 0) {
			return setNodeInfo(ti, mkNode<NodeErr>(std::format("ParserError: Failed to parse number '{}'", ti.str)));
		}
		return setNodeInfo(ti, mkNode<NodeNum>(num));
	}
	case Token::String:
		return setNodeInfo(ti, mkNode<NodeStr>(ti.str));
	case Token::Loop:
	{
		if (top.nxtToken.token != Token::End) {
			return setNodeInfo(ti, mkNode<NodeErr>("SyntaxError: @ only allowed as last element in list"));
		}
		return setNodeInfo(ti, mkNode<Node>(NodeType::Loop));
	}
	case Token::Begin:
	{
		auto list = std::make_shared<NodeCall>();
		top.advance();
		if (top.curToken.token != Token::Identifier) {
			return setNodeInfo(ti, mkNode<NodeErr>(std::format("Syntax error: Expected Identifer, got {}", tokName(top.curToken.token))));
		}
		list->ident = std::make_unique<NodeIdent>(top.curToken.str);
		list->name = list->ident->str;

		while (top.advance()) {

			if (top.curToken.token == Token::End) {
				break;
			}

			if( top.curToken.token == Token::SyntaxError) {
				return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: Error in call body: {}", TokInfoStr(ti), top.curToken.str)));
			}
			if (top.curToken.token == Token::Eof) {
				return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: Syntax error, call not closed.", TokInfoStr(ti))));
			}

			NodeRef n = parse(top.curToken);
			list->args.push_back(n);
		}
		return list;
	}

	case Token::IBegin:
	{
		auto list = std::make_shared<NodeCall>();

		list->name = "imp";

		while (top.advance()) {

			if (top.curToken.token == Token::IEnd) {
				break;
			}

			if( top.curToken.token == Token::SyntaxError) {
				return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: Error in imp body: {}", TokInfoStr(ti), top.curToken.str)));
			}
			if (top.curToken.token == Token::Eof) {
				return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: Syntax error, imp not closed.", TokInfoStr(ti))));
			}

			NodeRef n = parse(top.curToken);
			list->args.push_back(n);
		}
		return list;
	}

	case Token::LBegin:
	{
		auto list = std::make_shared<NodeCall>();

		list->name = "list";

		while (top.advance()) {

			if (top.curToken.token == Token::LEnd) {
				break;
			}

			if( top.curToken.token == Token::SyntaxError) {
				return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: Error in list body: {}", TokInfoStr(ti), top.curToken.str)));
			}
			if (top.curToken.token == Token::Eof) {
				return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: Syntax error, list not closed.", TokInfoStr(ti))));
			}

			NodeRef n = parse(top.curToken);
			list->args.push_back(n);
		}
		return list;
	}

	case Token::End:
		return setNodeInfo(ti, mkNode<NodeErr>("SyntaxError: Stray ')'"));
	case Token::IEnd:
		return setNodeInfo(ti, mkNode<NodeErr>("SyntaxError: Stray '}'"));
	case Token::LEnd:
		return setNodeInfo(ti, mkNode<NodeErr>("SyntaxError: Stray ']'"));

	case Token::SyntaxError:
		return setNodeInfo(ti, mkNode<NodeErr>(top.curToken.str));

	case Token::NoOP: // Used for pushing the previous token through without ending the stream or adding a meaningful token.
		top.advance();
		return parse(top.curToken);

	case Token::Eof:
		return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: System Error: Unexpected EOF token.", TokInfoStr(top.curToken))));
		break;

	}

	return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: System Error: Uninterpreted token.", TokInfoStr(top.curToken))));
}

Parsey::Parsey(TokenProvider& top) : top(top) {};
