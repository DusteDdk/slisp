#ifndef PARSEY_H_INCLUDED
#define PARSEY_H_INCLUDED

#include <memory>
#include <vector>
#include <string>
#include <format>
#include <utility>
#include <print>

#include "toker.h"

enum class NodeType : int_fast8_t {
	Base=0,
	Call=1,
	List,
	String,
	Number,
	Ident,
	Error,
	Loop,
	Variable
};

struct Node {
	NodeType t;
	Node() : t(NodeType::Base) {};
	Node(NodeType type) : t(type) {};
	virtual ~Node() = default;
	virtual std::string toString();
	TokenInfo origin;
	//std::shared_ptr<Node> prevNode;
	//std::shared_ptr<Node> nextNode;
};

using NodeRef = std::shared_ptr<Node>;



struct NodeNum : public Node {
	NodeNum() : Node(NodeType::Number), num(0) {}
	NodeNum(long double num) : Node(NodeType::Number), num(num) {}
	long double num;
};

struct NodeStr : public Node {
	NodeStr() : Node(NodeType::String), str("") {}
	NodeStr(std::string str) : Node(NodeType::String), str(str) {}
	std::string str;
};


struct NodeIdent : public Node {
	NodeIdent(std::string _str) : Node(NodeType::Ident), str(_str) {}
	std::string str;
};

struct NodeCall: public Node {
	NodeCall() : Node(NodeType::Call), name("::invalid_fname::") {}
	NodeCall(std::string fName) : Node(NodeType::Call), name(fName) {}
	std::string name;
	std::unique_ptr<NodeIdent> ident;
	std::vector<std::shared_ptr<Node>> args;
};

struct NodeVariable: public Node {
	bool isQuery = false;
	bool isKnownName = false;
	bool nameFromHead = false;
	NodeVariable() : Node(NodeType::Variable) {}
	std::string name;
	std::string toString() { return std::format("::Create Variable{}>::", name); };
	NodeRef valProvider;
};

struct NodeErr : public NodeStr {
	NodeErr(std::string str) : NodeStr(str) { t = NodeType::Error; }
	std::string toString() override;
};


class Parsey {
	TokenProvider &top;
	//NodeRef prevNode;
	NodeRef setNodeInfo(TokenInfo& ti, NodeRef node);
	NodeRef mkVar(TokenInfo&ti, int level);
public:
	Parsey(TokenProvider& top);
	NodeRef parse(TokenInfo& t, int level=0);
};


#endif
