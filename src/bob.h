#ifndef BOB_THE_BUILDER_INCLUDED
#define BOB_THE_BUILDER_INCLUDED

#include <memory>

#include "node.h"

template<class T, class U>
std::shared_ptr<T> mkNode(U&& u) {
	auto node =std::make_shared<T>(std::forward<U>(u));
	return node;
}


#endif