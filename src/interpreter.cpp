#include <print>
#include <string>
#include <format>
#include <map>
#include <iostream>
#include "interpreter.h"
#include "lfunctions.h"
#include <stdio.h>

static std::map<std::string, LFuncRef> calls{};

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
				return std::make_shared<FundamentalError>(std::format("{} Divide by 0 on pos {}", fname, pos));
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



FundamentalRef Interpreter::doCall(std::shared_ptr<NodeCall> call, FundamentalRef h) {

	// The interpreter implements a few of the calls: imp, =
	if (call->name == "imp") {
		FundamentalRef res = std::make_shared<FundamentalEmpty>();
		FundamentalRef prev = std::make_shared<FundamentalEmpty>(); // For head "skipping"
		res = h;
		prev = h;
		//for (auto arg : call->args) {
		for(size_t i = 0; i < call->args.size(); i++) {
			NodeRef arg = call->args[i];
			if (arg->t == NodeType::Loop) {
				res = std::make_shared<FundamentalLoop>(res);
			}
			else if (arg->t == NodeType::Variable) {
				auto varNode = std::dynamic_pointer_cast<NodeVariable>(arg);

				if (varNode->isQuery) {
					if (scopey.has(varNode->str)) {
						auto varVal = scopey.read(varNode->str);
						res = std::make_shared<FundamentalTrue>(varVal);
					}
					else {
						res = std::make_shared<FundamentalFalse>(std::make_shared<FundamentalString>( varNode->str));
					}
					prev = res;
					continue; // Skip rest of variable stuff
				}

				if (varNode->needsName) {
					varNode->needsName = false;
					switch (res->t) {
					case FType::Number:
					case FType::String:
						varNode->str = res->toString();
						break;
					default:
						return std::make_shared<FundamentalError>(std::format("Unexpected type for variable name: {}", res->toString()));
					}
				}

				auto vd = std::make_shared<FundamentalVariableDefinition>(varNode->str);
				vd->isKnownName = varNode->isKnownName;

				if (i + 1 == call->args.size()) {
					return std::make_shared<FundamentalError>(std::format("SyntaxError declaring variable {} with no value", vd->toString() ));
				}
				i++;
				res = descend(call->args[i], prev);
				vd->v = res;
				res = scopey.write(vd);

			}
			else {
				prev = res;
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
			return std::make_shared<FundamentalError>("deref Expects exactly one argument");
		}
		

		if(call->args[0]->t != NodeType::Ident) {
			return std::make_shared<FundamentalError>("deref Expects argument to be Identifier");
		}
		auto varNamer =std::dynamic_pointer_cast<NodeIdent>(call->args[0]);
		auto varName = scopey.read(varNamer->str);
		return scopey.read(varName->toString());

	}

	/**
	(? (then)) - compare to head -
	(? (cond) (then))
	(? (cond) (then) (else))
	*/
	if (call->name == "?") {
		if (call->args.size() == 0) {
			return std::make_shared<FundamentalError>("? Expects at least one argument");
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
		
		return std::make_shared<FundamentalError>(std::format("? {} is too many args!", call->args.size()));
	}


	if(scopey.has(call->name)) {
		return scopey.read(call->name);
	}

	LFuncRef fun = calls[call->name];
	if (fun) {
		std::vector<FundamentalRef> argVals;

		for (NodeRef arg : call->args)
		{
			auto val = descend(arg,h);
			if (val->t == FType::Error) {
				return val;
			}
			argVals.push_back(val);
		}
		return fun->run(h, argVals);
	}
	else {
		return std::make_shared<FundamentalError>(std::format("ERROR: Function not found '{}'", call->name));
	}
	std::println("No return?");
	return std::make_shared<FundamentalEmpty>();
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
			return std::make_shared<FundamentalError>("Unexpected @");
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

		return scopey.read(ident->str);
	}
	
	case NodeType::Variable:
		return std::make_shared<FundamentalError>(std::format("Unexpected attempt to declare variable outside imp."));
		
	case NodeType::Number:
	{
		auto num = std::dynamic_pointer_cast<NodeNum>(n);
		return std::make_shared<FundamentalNumber>(num->num);
	}
	case NodeType::Base:
	case NodeType::Stop:
	case NodeType::Error:
	default:


	}

	return std::make_shared<FundamentalError>(std::format("Uninterpreted type {}", n->toString()));
}

Interpreter::Interpreter() {
	calls["sprint"] = std::make_shared<LFsprint>("sprint");
	calls["print"] = std::make_shared<LFprint>("print", calls["sprint"]);
	calls["read"] = std::make_shared<LFread>("read");
	calls["+"] = std::make_shared<LFplus>("+");
	calls["-"] = std::make_shared<LFminus>("-");
	calls["*"] = std::make_shared<LFmul>("*");
	calls["/"] = std::make_shared<LFdiv>("/");
	calls["="] = std::make_shared<LFeq>("=");
	calls["!"] = std::make_shared<LFnegate>("!");
	calls[">"] = std::make_shared<LFgt>(">");
	calls["<"] = std::make_shared<LFlt>("<");
}

void Interpreter::run(NodeRef program)
{
	FundamentalRef head = std::make_shared<FundamentalEmpty>();
	FundamentalRef r = descend(program, head);
	std::println("\nInterpreter::run: Program Exit.\n\n{}", r->toString());
}
