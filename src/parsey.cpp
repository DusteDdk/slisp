#include <format>
#include <print>
#include <utility>
#include "token.h"
#include "parsey.h"



NodeRef Parsey::setNodeInfo(TokenInfo& ti, NodeRef node) {
	node->origin = ti;
	return node;
}

NodeRef Parsey::mkVar(TokenInfo &ti, int level) {
	auto varNode = std::make_shared<NodeVariable>();
	varNode->isQuery=(ti.token == Token::NameQuery);
	varNode->isKnownName = (ti.token == Token::KnownName);
	varNode->nameFromHead= !(ti.str.length());
	varNode->name= ti.str.length() ? ti.str : ":headNamed:";

	if(!varNode->isQuery) {
		if(!top.advanceSkipNoOp()) {
			return setNodeInfo(ti, mkNode<NodeErr>(std::format("ParserError: Unexpected end of file while parsing variable {}", varNode->name)));
		}
		varNode->valProvider = parse(top.curToken, ++level);
		if(varNode->valProvider->t == NodeType::Error) {
			return varNode->valProvider;
		}


	}

	return setNodeInfo(ti, varNode);
}

NodeRef Parsey::parse(TokenInfo& ti, int level) {
	Token inToken = ti.token;

	if (inToken == Token::Eof) {
		return setNodeInfo(ti, mkNode<Node>(NodeType::Base));
	}

	switch (inToken)
	{

	case Token::NameQuery:	// Query if Variable exists
	case Token::KnownName: // Known Variable
	case Token::Name: // Variable
		return mkVar(ti, level);
	break;
	case Token::Identifier:
		return setNodeInfo(ti, mkNode<NodeIdent>(ti.str));
	case Token::Number:
	{
		// Todo: note that it seems like failed parsing here does not float to top in (+ 1 2) if 2 failed to parse.
		char* end;
		errno=0; // Oh my.. remember to clear errorno..
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
		if (top.nxtToken.token != Token::End && top.nxtToken.token != Token::IEnd && top.nxtToken.token != Token::LEnd) {
			return setNodeInfo(ti, mkNode<NodeErr>("SyntaxError: @ only allowed as last element in list"));
		}
		return setNodeInfo(ti, mkNode<Node>(NodeType::Loop));
	}
	case Token::Begin:
	{
		auto callNode = std::make_shared<NodeCall>();
		top.advanceSkipNoOp();
		if (top.curToken.token != Token::Identifier) {
			//TODO: Just descend instead to support dynamically selecting call by calling a function that determines the name?
			return setNodeInfo(ti, mkNode<NodeErr>(std::format("Syntax error: Expected Identifer, got {}", tokName(top.curToken.token))));
		}
		callNode->ident = std::make_unique<NodeIdent>(top.curToken.str);
		callNode->name = callNode->ident->str;

		setNodeInfo(ti, callNode);

		while (top.advanceSkipNoOp()) {
			if (top.curToken.token == Token::End) {
				break;
			}

			NodeRef n = parse(top.curToken, ++level);
			if(n->t == NodeType::Error) {
				return n;
			}
			callNode->args.push_back(n);
		}
		return callNode;
	}

	case Token::IBegin:
	{
		auto impNode = std::make_shared<NodeCall>();
		impNode->name = "imp";
		setNodeInfo(ti, impNode);

		while (top.advanceSkipNoOp()) {

			if (top.curToken.token == Token::IEnd) {
				break;
			}

			NodeRef n = parse(top.curToken, ++level);
			if(n->t == NodeType::Error) {
				return n;
			}

			impNode->args.push_back(n);
		}

		return impNode;
	}

	case Token::LBegin:
	{
		auto listNode = std::make_shared<NodeCall>();
		listNode->name = "list";
		setNodeInfo(ti, listNode);

		while (top.advanceSkipNoOp()) {

			if (top.curToken.token == Token::LEnd) {
				break;
			}

			NodeRef n = parse(top.curToken, ++level);
			if(n->t == NodeType::Error) {
				return n;
			}
			listNode->args.push_back(n);
		}

		return listNode;
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
		top.advanceSkipNoOp();
		return parse(top.curToken, level);

	case Token::Eof:
		return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: Unexpected end of file", TokInfoStr(top.curToken))));
		break;

	}

	return setNodeInfo(ti, mkNode<NodeErr>(std::format("{}: System Error: Uninterpreted token.", TokInfoStr(top.curToken))));
}

Parsey::Parsey(TokenProvider& top) : top(top) {};
