#include <print>
#include <string>
#include <format>
#include <map>
#include <iostream>
#include "interpreter.h"
#include "lfunctions.h"
#include <stdio.h>
#include <SDL2/SDL.h>

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

class LFwindow : public LFunc {
	private:
		SDL_Window* win;
		SDL_Renderer* ren;
		int width, height;
		std::string wname;
		uint8_t red=0,green=0,blue=0;
	public:
	LFwindow(std::string n) : LFunc(n), win(0),ren(0), width(640), height(480), wname("Unnamed slisp window")  {}
	FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) override {
			if(!win) {

				if(args.size()>0) {
					if(args[0]->t == FType::String) {
						wname = std::dynamic_pointer_cast<FundamentalString>(args[0])->s;
					}
				}
				if(args.size()>1) {
					if(args[1]->t == FType::Number) {
						width = std::dynamic_pointer_cast<FundamentalNumber>(args[1])->n;
					}
				}
				if(args.size()>2) {
					if(args[2]->t == FType::Number) {
						height = std::dynamic_pointer_cast<FundamentalNumber>(args[2])->n;
					}
				}

				SDL_Init(SDL_INIT_VIDEO);

				win = SDL_CreateWindow(
					wname.c_str(),
					SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					width, height,
					SDL_WINDOW_SHOWN
					);

				if (win == NULL) {
					return std::make_shared<FundamentalError>("Could not create window");
				}


				ren = SDL_CreateRenderer (win, -1,
                                             SDL_RENDERER_ACCELERATED
                                            /* | SDL_RENDERER_PRESENTVSYNC*/);


			}


			return std::make_shared<FundamentalTrue>(std::make_shared<FundamentalString>("window"));
	};

	FundamentalRef run(NodeIdent* ident, FundamentalRef head, std::vector<FundamentalRef> args) override {
		if(ident->str == "window") {
			return run(head, args);
		}

			if(ident->str == "color") {
				if(args.size() == 3) {

				  red=(uint8_t)std::dynamic_pointer_cast<FundamentalNumber>(args[0])->n;
				  green=(uint8_t)std::dynamic_pointer_cast<FundamentalNumber>(args[1])->n;
				  blue=(uint8_t)std::dynamic_pointer_cast<FundamentalNumber>(args[2])->n;

				  SDL_SetRenderDrawColor(ren, red, green, blue, 255);
				}
			}

			if(ident->str == "window.fill") {
				SDL_RenderFillRect(ren, NULL);
			}

			if(ident->str == "window.dot") {
				if(args.size() == 3) {
					SDL_Rect r;

					r.x=(int)std::dynamic_pointer_cast<FundamentalNumber>(args[1])->n;
					r.y=(int)std::dynamic_pointer_cast<FundamentalNumber>(args[2])->n;
					r.w=(int)(std::dynamic_pointer_cast<FundamentalNumber>(args[0])->n/2);
					r.h=r.w;
				  SDL_RenderFillRect(ren, &r);

				}
			}

			if(ident->str == "window.flip") {
				SDL_RenderPresent(ren);

				SDL_Event event;
    			SDL_PollEvent(&event);
				if(event.type == SDL_QUIT) {
					std::println("SDL: QUIT");
					return std::make_shared<FundamentalFalse>(std::make_shared<FundamentalString>("QUIT"));
				}

			}



		return std::make_shared<FundamentalTrue>(std::make_shared<FundamentalString>("window"));
	}
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
	case NodeType::Variable:
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
	calls["|"] = std::make_shared<LFor>("|");
	calls["list"] = std::make_shared<LFlist>("list");
	calls["window"] = std::make_shared<LFwindow>("window");
	calls["window.color"] = calls["window"];
	calls["window.flip"] = calls["window"];
	calls["window.dot"] = calls["window"];
	calls["window.fill"] = calls["window"];

}

void Interpreter::run(NodeRef program)
{
	FundamentalRef head = std::make_shared<FundamentalEmpty>();
	FundamentalRef r = descend(program, head);
	std::println("");
}
