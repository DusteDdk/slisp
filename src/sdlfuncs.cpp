#include <SDL2/SDL.h>

#include "sdlfuncs.h"
#include "lfunctions.h"

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


void registerSDLFunc(Interpreter& inter) {
    LFuncRef sdlWin = std::make_shared<LFwindow>("window");
   	inter.addFunc("window", sdlWin);
	inter.addFunc("window.color", sdlWin);
	inter.addFunc("window.flip", sdlWin);
	inter.addFunc("window.dot", sdlWin);
	inter.addFunc("window.fill", sdlWin);
}
