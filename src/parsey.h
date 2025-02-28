#ifndef PARSEY_H_INCLUDED
#define PARSEY_H_INCLUDED

#include <memory>
#include <vector>
#include <string>
#include <format> 
#include <utility>

#include "toker.h"

enum class NodeType : int_fast8_t {
	Call=1,
	String,
	Number,
	Base,
	Stop,
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
};

using NodeRef = std::shared_ptr<Node>;

struct NodeCall: public Node {
	NodeCall() : Node(NodeType::Call), name("::invalid_fname::") {}
	NodeCall(std::string fName) : Node(NodeType::Call), name(fName) {}
	std::string name;
	std::vector<std::shared_ptr<Node>> args;
};


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


struct NodeIdent : public NodeStr {
	NodeIdent(std::string str) : NodeStr(str) { t = NodeType::Ident; }
};

struct NodeVariable: public NodeStr {
	bool isQuery = false;
	bool needsName = false;
	bool isKnownName = false;
	NodeVariable(std::string str) : NodeStr(str) { t = NodeType::Variable; }
	NodeVariable() : NodeStr("::unnamed_var::") { t = NodeType::Variable; needsName = true;  }
	std::string toString() { return std::format("::variable({})::", str); }
};

struct NodeErr : public NodeStr {
	NodeErr(std::string str) : NodeStr(str) { t = NodeType::Error; }
	std::string toString() override;
};


class Parsey {
	Toker toker;
	void skipToNextSemanticToken(Token& t);
	NodeRef parseNode(Token t);
	bool verbose;
	template <typename... Args> void log(std::format_string<Args...> fmt, Args&&... args) const;
public:

	Parsey(Toker toker, bool verbose=false);
	NodeRef program;
};


#endif
