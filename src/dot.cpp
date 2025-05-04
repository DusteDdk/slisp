
#include <iostream>
#include <string>
#include <memory>
#include "token.h"
#include "toker.h"
#include "parsey.h"

#include <fstream>
#include "node.h"

#include <format>
#include <print>
#include "toker.h"
#include <vector>
#include <filesystem>
using namespace std;

vector<string> json;

string nodeToDot(NodeRef node, int level, int parentId)
{

/*
	Token token=Token::NoOP;
	std::string str="";
	std::string file="";
	int line=0, column=0;

*/

    string ret="";

    json.push_back( format("  \"slnode{}\": {{ line: {}, column: {}, tok: \"{}\" }}", node->id, node->origin.line, node->origin.column, tokName(node->origin.token) ));

    int prev=0;
    int cur = node->id;
    if(node->t == NodeType::Call) {
        const auto call = dynamic_pointer_cast<NodeCall>(node);
        if(call->name == "imp") {
            ret += format("subgraph cluster_{} {{ label=\"Imp #{}\"; color=indigo; bgcolor=mintcream;\n", node->id,node->id);

            if(parentId==0) {
                ret+=format("{} [id=\"slnode{}\", label=\"Imp #{}\", color=indigo color=blue shape=invhouse style=filled, fillcolor=lightgreen];\n", node->id, node->id,node->id);
            } else {
                ret+=format("{} [id=\"slnode{}\", label=\"Imp #{}\", color=indigo shape=parallelogram style=filled, fillcolor=lightcyan];\n", node->id,node->id,node->id);
            }

            for(NodeRef& cn : call->args) {
                ret += format("{}\n", nodeToDot(cn, level+1, node->id));
                prev=cur;
                cur=cn->id;

                if(prev) {
                    ret+=format("{} -> {} [color=indigo];\n", prev, cur); // Control flow
                }
            }

            ret += format("{} -> {} [color=green];\n}}\n", cur, node->id); // Data flow



        } else {
            ret += format("subgraph cluster_{} {{ label=\"Expr #{}\"; color=yellow; bgcolor=lightgrey;\n", node->id, node->id);
            ret+=format("{} [id=\"slnode{}\", label=\"#{} {}\", shape=cds, style=filled, color=yellow, fillcolor=lightyellow];\n", node->id,node->id,node->id, call->name);
            cur=node->id;
            int argNum=0;
            for(NodeRef& cn : call->args) {
                ret += format("{}\n", nodeToDot(cn, level+1, node->id));
                prev=cur;
                cur=cn->id;

                ret+=format("{} -> {} [style=invis];\n", cur, prev); // Data flow

                string edgeLabel=format("{}, {}", node->id, argNum++);

                if(call->name == "?") {
                    if(call->args.size() == 1) {
                        edgeLabel = format("#{} then", node->id);
                    } else {
                        switch(argNum) {
                            case 1:
                                edgeLabel = format("#{} cond", node->id);
                            break;
                            case 2:
                                edgeLabel = format("#{} then", node->id);
                            break;
                            case 3:
                                edgeLabel = format("#{} else", node->id);
                            break;
                        }
                    }
                }

                ret+=format("{} -> {} [dir=both, style=dashed, color=black, label=\"{}\"];\n", node->id, cur, edgeLabel); // flow
            }
            ret += "}\n";
        }

    } else if(node->t == NodeType::Variable) {
        const auto var = dynamic_pointer_cast<NodeVariable>(node);

        ret+=format("{} [id=\"slnode{}\",label=\"{}\", shape=note, style=filled, fillcolor=papayawhip];\n",node->id,node->id, node->toString());
        ret += format("{}\n", nodeToDot(var->valProvider, level, node->id));
        ret+=format("{} -> {} [dir=both, style=dashed, color=black];\n", var->valProvider->id, node->id); //  flow
    } else {
        ret=format("{} [id=\"slnode{}\", label=\"{}\", shape=rect, style=filled, fillcolor=ivory];", node->id,node->id, node->toString());
    }


    return ret;
}


string toDot(NodeRef node)
{
    return format("digraph G {{bgcolor=lightblue2;\n{} }}", nodeToDot(node, 1, 0));
}



int main(int argc, char* argv[]) {

	if (argc == 1) {

        println("Usage: ./{} FILENAME", argv[0]);
        println("Example:");
        println("{} | dot -Tsvg > /tmp/img.svg ; feh /tmp/img.svg", argv[0]);
        return 1;
	}

    std::string fName(argv[1]);
    std::ifstream ifs(fName, std::ifstream::in);
    TokenProvider top(ifs, fName);
    Parsey p(top);

    if(! top.advanceSkipNoOp() ) {
        std::println("Error in input file.");
        return 1;
    }

    NodeRef firstNode = p.parse( top.curToken );
    //println( "{}", toDot( firstNode ) );

    string dotGraph = toDot( firstNode );

    string jsonStr="";
    bool first=true;
    jsonStr +="const srcMap = {\n";
    for( string& jl : json) {
        if(first) {
            first=false;
        } else {
            jsonStr +=",\n";
        }
        jsonStr += jl;
    }
    jsonStr += "\n};\n";

    println("<html><head><title>explore {}</title></head><body><center>", argv[1]);

    FILE* pipe = popen("dot -Tsvg  -Gid=\"slsvg\"", "w"); // Send output to Graphviz
    if (pipe) {
        fprintf(pipe, "%s\n", dotGraph.c_str() );
        fclose(pipe);
    }

    auto script = R"(
    window.onload = () => {
        for(const k in srcMap) {

            const node = document.getElementById(k);
            if(node) {
            node.onmouseenter = ()=>{
                //console.log(k);

                highlightPreLine('code', srcMap[k].line);
                document.getElementById('debug').innerHTML=JSON.stringify( srcMap[k], null, 4);

                const polygon = node.querySelector("polygon");

                // Save the original stroke
                const originalStroke = polygon.getAttribute("stroke");

                // Set a new stroke color
                polygon.setAttribute("stroke", "red");

                node.onmouseleave = ()=>{
                    polygon.setAttribute("stroke", originalStroke);
                };

            };
            } else {
           console.log("Not found",k);

            }
        }
        //




const graph = document.getElementById("slsvg");
const svg = graph.closest("svg");

let viewBox = { x: 0, y: 0, w: 800, h: 600 };  // Match initial viewBox
let isPanning = false;
let start = { x: 0, y: 0 };

svg.setAttribute("viewBox", `${viewBox.x} ${viewBox.y} ${viewBox.w} ${viewBox.h}`);
svg.style.width="100%";
svg.style.height="75%";
svg.style.border="1px solid black";

// Handle mouse drag
svg.addEventListener("mousedown", (e) => {
  isPanning = true;
  start = { x: e.clientX, y: e.clientY };
});

svg.addEventListener("mousemove", (e) => {
  if (!isPanning) return;
  const dx = (e.clientX - start.x) * viewBox.w / svg.clientWidth;
  const dy = (e.clientY - start.y) * viewBox.h / svg.clientHeight;
  viewBox.x -= dx;
  viewBox.y -= dy;
  svg.setAttribute("viewBox", `${viewBox.x} ${viewBox.y} ${viewBox.w} ${viewBox.h}`);
  start = { x: e.clientX, y: e.clientY };
});

svg.addEventListener("mouseup", () => isPanning = false);
svg.addEventListener("mouseleave", () => isPanning = false);

// Handle zoom
svg.addEventListener("wheel", (e) => {
  e.preventDefault();
  const zoom = e.deltaY > 0 ? 1.1 : 0.9;
  const mx = e.offsetX * viewBox.w / svg.clientWidth + viewBox.x;
  const my = e.offsetY * viewBox.h / svg.clientHeight + viewBox.y;

  viewBox.w *= zoom;
  viewBox.h *= zoom;
  viewBox.x = mx - (mx - viewBox.x) * zoom;
  viewBox.y = my - (my - viewBox.y) * zoom;

  svg.setAttribute("viewBox", `${viewBox.x} ${viewBox.y} ${viewBox.w} ${viewBox.h}`);
});

const lines = document.getElementById('code').textContent.split('\n');
function highlightPreLine(preId, lineNumber) {
  const pre = document.getElementById(preId);


  pre.innerHTML = ''; // Clear content to rebuild with spans

  lines.forEach((line, i) => {
    const span = document.createElement('span');
    span.style.display = 'block';
    span.style.whiteSpace = 'pre';

    if (i === lineNumber - 1) {
      span.style.background = 'yellow';
    }

    span.textContent = line+"\n";
    pre.appendChild(span);
  });
}



    };
    )";


    auto size = std::filesystem::file_size(argv[1]);
    std::string content(size, '\0');
    std::ifstream in(argv[1]);
    in.read(&content[0], size);

    println("</center><pre id=\"code\">{}</pre><div id=\"debug\" style=\"border: 1px solid black;\"></div><script>{}{}</script></body></html>",content, jsonStr, script);

    

    //    fprintf(pipe, "digraph G { A -> B; }");
/*
    println("\n\nJSON:");
    bool first=true;
    println("{{");
    for( string& jl : json) {
        if(first) {
            first=false;
        } else {
            println(",");
        }
        print("  {}", jl);
    }
    println("\n}}");*/
	return 0;
}