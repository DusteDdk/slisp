#include "node.h"


std::string nodeTypeToStr(NodeType t) {
	std::string name;
	switch( t ) {
		case NodeType::Base:
			name ="Base";
		break;
		case NodeType::Call:
			name ="Call";
		break;
		case NodeType::List:
			name ="List";
		break;
		case NodeType::String:
			name ="String";
		break;
		case NodeType::Number:
			name ="Number";
		break;
		case NodeType::Ident:
			name ="Ident";
		break;
		case NodeType::Error:
			name ="Error";
		break;
		case NodeType::Loop:
			name ="Loop";
		break;
		case NodeType::Variable:
			name ="Variable";
		break;
		default:
			name="unknown";
		break;
	}

	return name;
}

std::string Node::toString() {
	return std::format("{}", nodeTypeToStr(t));
}

std::string NodeErr::toString() {
	return std::format("{}:{}<{}>", TokInfoStr(origin), nodeTypeToStr(t), str);
}

std::string NodeVariable::toString() {
	return std::format("{}<{}>", nodeTypeToStr(t), name);
}

int globId=1;

#include <print>
Node::Node() : Node(NodeType::Base) {};

Node::Node(NodeType type) : t(type), perspect(NodePerspective::Val) {
	id=globId++;
};


std::string NodeIdent::toString() {
	return std::format("{}<{}>", nodeTypeToStr(t), str);
}

std::string NodeCall::toString() {
	return std::format("{}<{}>", nodeTypeToStr(t), name);
}

std::string NodeStr::toString() {
	return std::format("{}<{}>", nodeTypeToStr(t), str);
}

std::string NodeNum::toString() {
	return std::format("{}<{}>", nodeTypeToStr(t), num);
}