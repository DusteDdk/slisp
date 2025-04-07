#ifndef PARSEY_H_INCLUDED
#define PARSEY_H_INCLUDED

#include <memory>
#include <vector>
#include <string>
#include <format>
#include <utility>
#include <print>

#include "toker.h"

#include "bob.h"

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
