#include <print>
#include <string>
#include <format>
#include <iostream>
#include <stdio.h>

#include "sdlfuncs.h"
#include "interpreter.h"

FundamentalRef mkErr(std::string str, NodeRef n) {
	std::string msg = std::format("{}: Interpretation error: {}", TokInfoStr(n->origin), str);
	return std::make_shared<FundamentalError>(msg);
}

FundamentalRef wrapPotentialErr(FundamentalRef ft, NodeRef n) {
	if(ft->t == FType::Error) {
		auto e = std::dynamic_pointer_cast<FundamentalError>(ft);
		return mkErr(e->s, n);
	}
	return ft;
}


class LFsprint : public LFunc {
public:
	LFsprint(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {
		std::string r = "";
		// When there are no arguments, we operate on head
		if (args.size() == 0) {
			args.insert(args.begin(), head);
		}

		for (FundamentalRef a : args) {
			r += a->toString();
		}
		return std::make_shared<FundamentalString>(r);
	};
};

class LFprint : public LFunc {
private:
	LFuncRef sp;
public:
	LFprint(std::string n, LFuncRef spr) : LFunc(n), sp(spr) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {
		std::cout << sp->run(head, args)->toString();
		return head;
	};
};


class LFlist : public LFunc {
	public:
	LFlist(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {
		FundamentalRef r = std::make_shared<FundamentalList>(args.size());
		auto l = std::dynamic_pointer_cast<FundamentalList>(r);
		for(FundamentalRef a : args) {
			l->body.push_back(a);
		}
		return r;
	};
};


class LFread: public LFunc {
public:
	LFread(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {
		std::string r = "";
		for (FundamentalRef a : args) {
			r += a->toString();
		}
		std::string lin;
		if (std::getline(std::cin, lin)) {
			return std::make_shared<FundamentalString>(lin);
		}
		else {
			return std::make_shared<FundamentalError>("Could not read from STDIN");
		}


	};
};


FundamentalRef fStringTofNum(FundamentalRef a) {
auto fstr = std::dynamic_pointer_cast<FundamentalString>(a);
char* end{};
errno=0;
long double num = std::strtold(fstr->s.c_str(), &end);
if (*end != 0 || errno != 0) {
	return std::make_shared<FundamentalError>(std::format("Failed to parse string argument '{}' as number.", fstr->s));
}
return std::make_shared <FundamentalNumber>(num);

}

class LFmul : public LFunc {
public:
	LFmul(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one number!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			args.insert(args.begin(), head);
		}

		int pos = 0;
		long double d;
		for (FundamentalRef a : args) {
			pos++;
			if (a->t == FType::String) {
				a = fStringTofNum(a);
				if (a->t != FType::Number) {
					return a;
				}
			}
			if (a->t != FType::Number) {
				return std::make_shared<FundamentalError>(std::format("{} Expected number argument on pos {}", fname, pos));
			}

			auto n = std::dynamic_pointer_cast<FundamentalNumber>(a);
			if (pos == 1) {
				d = n->n;
			}
			else {
				d *= n->n;
			}

		}

		return std::make_shared<FundamentalNumber>(d);
	};
};



class LFdiv : public LFunc {
public:
	LFdiv(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one number!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			args.insert(args.begin(), head);
		}

		int pos = 0;
		long double d;
		for (FundamentalRef a : args) {
			pos++;
			if (a->t == FType::String) {
				a = fStringTofNum(a);
				if (a->t != FType::Number) {
					return a;
				}
			}

			if (a->t != FType::Number) {
				return std::make_shared<FundamentalError>(std::format("{} Expected number argument on pos {}", fname, pos));
			}

			auto n = std::dynamic_pointer_cast<FundamentalNumber>(a);
			if (pos == 1) {
				d = n->n;
			}
			else if (n->n == 0) {
				return std::make_shared<FundamentalError>(std::format("Divide by 0 on pos {}", pos));
			} else {
				d /= n->n;
			}

		}

		return std::make_shared<FundamentalNumber>(d);
	};
};

class LFplus : public LFunc {
public:
	LFplus(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one number!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			args.insert(args.begin(), head);
		}

		int pos = 0;
		long double d;
		for (FundamentalRef a : args) {
			pos++;
			if (a->t == FType::String) {
				a = fStringTofNum(a);
				if (a->t != FType::Number) {
					return a;
				}
			}

			if (a->t != FType::Number) {
				return std::make_shared<FundamentalError>(std::format("{} Expected number argument on pos {}", fname, pos));
			}

			auto n = std::dynamic_pointer_cast<FundamentalNumber>(a);
			if (pos == 1) {
				d = n->n;
			}
			else {
				d += n->n;
			}

		}

		return std::make_shared<FundamentalNumber>(d);
	};
};

class LFminus : public LFunc {
public:
	LFminus(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one number!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			args.insert(args.begin(), head);
		}

		int pos = 0;
		long double d;
		for (FundamentalRef a : args) {
			pos++;
			if (a->t == FType::String) {
				a = fStringTofNum(a);
				if (a->t != FType::Number) {
					return a;
				}
			}

			if (a->t != FType::Number) {
				return std::make_shared<FundamentalError>(std::format("{} Expected number argument on pos {}", fname, pos));
			}

			auto n = std::dynamic_pointer_cast<FundamentalNumber>(a);
			if (pos == 1) {
				d = n->n;
			}
			else {
				d -= n->n;
			}

		}

		return std::make_shared<FundamentalNumber>(d);
	};
};


class LFeq : public LFunc {
public:
	LFeq(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one argument!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			//std::println(" = head: {}", head->toString());
			args.insert(args.begin(), head);
		}

		int pos = 0;
		FundamentalRef v;
		for (FundamentalRef a : args) {
			pos++;
			if (pos == 1) {
				v = a;
			}
			else if(v->t != a->t) {
				return std::make_shared<FundamentalError>(std::format("{} Expects all arguments to be of same type!", fname));
			}
			else {
				int diff = v->diff(a);
				//std::println("= (argnum {}) diff: {} on {} vs {}", args.size(), diff, a->toString(), v->toString());
				if (diff)
				{
					//std::println("  = false ");
					return std::make_shared<FundamentalFalse>(a);
				}
			}

		}
		//std::println("  = true ");
		return std::make_shared<FundamentalTrue>(v);
	};
};

class LFor : public LFunc {
public:
	LFor(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one argument!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			args.insert(args.begin(), head);
		}

		FundamentalRef last;
		for (FundamentalRef a : args) {
			if (a->t != FType::False) {
				return std::make_shared<FundamentalTrue>(a);
			}
			last = a;
		}
		return std::make_shared<FundamentalFalse>(last);
	}

};


class LFgt : public LFunc {
public:
	LFgt(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one argument!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			args.insert(args.begin(), head);
		}

		int pos = 0;
		FundamentalRef v;
		for (FundamentalRef a : args) {
			pos++;
			if (pos == 1) {
				v = a;
			}
			else if (v->t != a->t) {
				return std::make_shared<FundamentalError>(std::format("{} Expects all arguments to be of same type!", fname));
			}
			else {
				int diff = v->diff(a);

				if (diff < 1.0)
				{
					return std::make_shared<FundamentalFalse>(a);
				}
			}

		}
		return std::make_shared<FundamentalTrue>(v);
	};
};

class LFlt : public LFunc {
public:
	LFlt(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		if (!args.size()) {
			return std::make_shared<FundamentalError>(std::format("{} Expected at least one argument!", fname));
		}
		// When there's only one argument, we operate on head
		if (args.size() == 1) {
			args.insert(args.begin(), head);
		}

		int pos = 0;
		FundamentalRef v;
		for (FundamentalRef a : args) {
			pos++;
			if (pos == 1) {
				v = a;
			}
			else if (v->t != a->t) {
				return std::make_shared<FundamentalError>(std::format("{} Expects all arguments to be of same type!", fname));
			}
			else {
				int diff = v->diff(a);
				if (diff > -1.0)
				{
					return std::make_shared<FundamentalFalse>(a);
				}
			}

		}
		return std::make_shared<FundamentalTrue>(v);
	};
};

class LFnegate : public LFunc {
public:
	LFnegate(std::string n) : LFunc(n) {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {

		// When there's no arguments
		if (args.size() == 0) {
			args.insert(args.begin(), head);
		}

		if (args.size() != 1) {
			return std::make_shared<FundamentalError>(std::format("{} Can negate head or one argument, but got {} arguments", fname, args.size()));
		}

		if (args[0]->t == FType::False) {
			return std::make_shared<FundamentalTrue>(args[0]);
		}
		return std::make_shared<FundamentalFalse>(args[0]);
	};
};


// Todo, when slisp gets a way to declare functions, we should register our own functions using that, rather than the.. three? different ways we have here
FundamentalRef Interpreter::doCall(std::shared_ptr<NodeCall> call, FundamentalRef h) {

	if (call->name == "imp") {
		FundamentalRef res = h;
		for(size_t i = 0; i < call->args.size(); i++) {
			NodeRef arg = call->args[i];
			if (arg->t == NodeType::Loop) {
				res = std::make_shared<FundamentalLoop>(res);
			} else {
				res = descend(arg, res);
				if (res->t == FType::Loop) {
					auto fl = std::dynamic_pointer_cast<FundamentalLoop>(res);
					res = fl->lh; // Set head to headvalue of @ (so subexpressions transport state uptree, is this a good idea ?)
					i = -1; // Don't skip first element
					continue;
				}
			}
			if (res->t == FType::Error) {
				return res;
			}
		}
		return res;
	}


	// Rather than using an identifier name, look up the variable contained in the identifier
	if(call->name == "deref") {
		if(!call->args.size()) {
			return mkErr("deref Expects exactly one argument", call);
		}

		if(call->args[0]->t != NodeType::Ident) {
			return mkErr("deref Expects argument to be Identifier", call);
		}
		auto varNamer =std::dynamic_pointer_cast<NodeIdent>(call->args[0]);
		auto varName = wrapPotentialErr( scopey.read(varNamer->str), varNamer );
		return wrapPotentialErr( scopey.read(varName->toString()), varNamer );

	}

	/**
	(? (then)) - compare to head -
	(? (cond) (then))
	(? (cond) (then) (else))
	*/
	if (call->name == "?") {
		if (call->args.size() == 0) {
			return mkErr("? Expects at least one argument", call);
		}

		if (call->args.size() == 1) {
			if (h->t != FType::False) {
				return descend(call->args[0], h, true);
			}
			return h;
		}
		if (call->args.size() == 2) {
			FundamentalRef cond = descend(call->args[0], h);
			if (cond->t != FType::False) {
				return descend(call->args[1], h, true);
			}
			return h;
		}
		if (call->args.size() == 3) {
			FundamentalRef cond = descend(call->args[0], h);
			if (cond->t != FType::False) {
				return descend(call->args[1], h, true);
			}
			return descend(call->args[2], h, true);
		}

		return mkErr(std::format("? {} is too many args!", call->args.size()), call);
	}


	if(scopey.has(call->name)) {
		return wrapPotentialErr( scopey.read(call->name), call );
	}

	LFuncRef fun = calls[call->name];
	if (fun) {
		std::vector<FundamentalRef> argVals;

		for (NodeRef arg : call->args)
		{
			auto val = descend(arg,h);

			// Todo, if the value is a variableDecl, it's a named variable, don't add it to list..
			// or consider whether this should be the parsers job ? it could treat variable decl nodes in call bodies special,
			// YES! That's much nicer!

			if (val->t == FType::Error) {
				return val;
			}
			argVals.push_back(val);
		}
		return fun->run(call->ident.get(), h, argVals);
	}

	return mkErr(std::format("ERROR: Function not found '{}'", call->name), call);
}

FundamentalRef Interpreter::descend(NodeRef n, FundamentalRef h, bool loopTokenValid) {
	switch (n->t)
	{
	case NodeType::Call: {
		auto call = std::dynamic_pointer_cast<NodeCall>(n);
		scopey.enterScope(call->name);
		auto callRet = doCall(call, h);
		scopey.exitScope();
		return callRet;
	}
	case NodeType::String: {
		auto str = std::dynamic_pointer_cast<NodeStr>(n);
		return std::make_shared<FundamentalString>(str->str);
	}
	case NodeType::Loop:
		if (loopTokenValid) {
			return std::make_shared<FundamentalLoop>(h);
		} else {
			return mkErr("Unexpected @", n);
		}
	case NodeType::Ident:
	{
		auto ident = std::dynamic_pointer_cast<NodeIdent>(n);
		if (ident->str == "$") {
			return h;
		}
		if (ident->str == "nl") {
			return std::make_shared<FundamentalString>("\n");
		}

		return wrapPotentialErr( scopey.read(ident->str), ident);
	}
	case NodeType::Variable: // Consider if we can emit info on the variable name too here somehow? (consider how much effort its worth and how it blends into the idea of the fundamental objects, or wheter nodes will become that eventually)
	{
		auto varNode = std::dynamic_pointer_cast<NodeVariable>(n);

		if(varNode->nameFromHead) {
			switch (h->t) {
				case FType::Number:
				case FType::String:
					varNode->name = h->toString();
					break;
				default:
					return mkErr(std::format("Useless head type for variable name: {}", h->toString()), varNode);
			}
		}

		if (varNode->isQuery) {
			if (scopey.has(varNode->name)) {
				auto varVal = scopey.read(varNode->name);
				return std::make_shared<FundamentalTrue>(varVal);
			}
			return std::make_shared<FundamentalFalse>(std::make_shared<FundamentalString>(varNode->name));
		}

		FundamentalRef varValue = descend(varNode->valProvider, h);
		if(varValue->t == FType::Error) {
			return wrapPotentialErr(varValue, varNode->valProvider);
		}

		auto vd = std::make_shared<FundamentalVariableDefinition>(varNode->name);
		vd->isKnownName = varNode->isKnownName;
		vd->v = varValue;
		auto ret = wrapPotentialErr( scopey.write(vd), varNode);
		return ret;

	}
	case NodeType::Number:
	{
		auto num = std::dynamic_pointer_cast<NodeNum>(n);
		return std::make_shared<FundamentalNumber>(num->num);
	}
	case NodeType::Error:
	{
		auto err = std::dynamic_pointer_cast<NodeErr>(n);
		return std::make_shared<FundamentalError>(err->toString());
	}
	case NodeType::Base:
	default:
		break;

	}

	return mkErr(std::format("Uninterpreted type {}", n->toString()), n);
}

void Interpreter::addFunc(std::string name, LFuncRef fun) {
	if(calls.contains(name)) {
		std::println("Warning: Overwriting existing funcation {}", name);
	}
	calls[name] = fun;
}

Interpreter::Interpreter() {

	addFunc("sprint", std::make_shared<LFsprint>("sprint"));
	addFunc("print", std::make_shared<LFprint>("print", calls["sprint"]));
	addFunc("read", std::make_shared<LFread>("read"));
	addFunc("+", std::make_shared<LFplus>("+"));
	addFunc("-", std::make_shared<LFminus>("-"));
	addFunc("*", std::make_shared<LFmul>("*"));
	addFunc("/", std::make_shared<LFdiv>("/"));
	addFunc("=", std::make_shared<LFeq>("="));
	addFunc("!", std::make_shared<LFnegate>("!"));
	addFunc(">", std::make_shared<LFgt>(">"));
	addFunc("<", std::make_shared<LFlt>("<"));
	addFunc("|", std::make_shared<LFor>("|"));
	addFunc("list", std::make_shared<LFlist>("list"));

	registerSDLFunc(*this);
}

void Interpreter::run(NodeRef program)
{
	FundamentalRef head = std::make_shared<FundamentalEmpty>();
	FundamentalRef r = descend(program, head);
	std::println("");
}
