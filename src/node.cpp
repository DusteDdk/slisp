#include "node.h"

std::string Node::toString() {
	return std::format("::Node(Type{})::", (int)t);
}

std::string NodeErr::toString() {
	return std::format("{} {}", TokInfoStr(origin), str);
}

