
//LanguageFUNCtionS

#ifndef LFUNCS_H_INCLUDED
#define LFUNCS_H_INCLUDED

#include <memory>
#include "parsey.h"
#include "fundamental.h"

class LFunc {
public:
	std::string fname;
	LFunc(std::string n) : fname(n) {}
	virtual ~LFunc() = default;
	virtual FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) = 0;
};

using LFuncRef = std::shared_ptr<LFunc>;

#endif