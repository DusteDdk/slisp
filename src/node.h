#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <string>
#include <memory>
#include <vector>

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

enum class NodePerspective : int_fast8_t {
	Val=1,
	Expr,
};

struct Node {
	NodeType t;
	NodePerspective perspect;
	Node() : t(NodeType::Base), perspect(NodePerspective::Val) {};
	Node(NodeType type) : t(type), perspect(NodePerspective::Val) {};
	virtual ~Node() = default;
	virtual std::string toString();
	TokenInfo origin;
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
	NodeCall() : Node(NodeType::Call), inert(false), name("::invalid_fname::"){}
	NodeCall(std::string fName) : Node(NodeType::Call), inert(false), name(fName) {}
	bool inert;
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

#endif