
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


#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

std::string runDot(const std::string& dotInput) {
    int inPipe[2];  // parent writes to [1], child reads from [0]
    int outPipe[2]; // child writes to [1], parent reads from [0]

    if (pipe(inPipe) != 0 || pipe(outPipe) != 0) {
        perror("pipe");
        return "";
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return "";
    }

    if (pid == 0) {
        // --- Child process ---

        // Redirect stdin
        dup2(inPipe[0], STDIN_FILENO);
        close(inPipe[0]);
        close(inPipe[1]); // parent’s write end

        // Redirect stdout
        dup2(outPipe[1], STDOUT_FILENO);
        close(outPipe[1]);
        close(outPipe[0]); // parent’s read end

        execlp("dot", "dot", "-Tsvg", "-Gid=slsvg", nullptr);

        // If execlp fails
        perror("execlp");
        _exit(1);
    }

    // --- Parent process ---

    // Close unused ends
    close(inPipe[0]);  // parent doesn't read from inPipe
    close(outPipe[1]); // parent doesn't write to outPipe

    // Write DOT input to child
    write(inPipe[1], dotInput.c_str(), dotInput.size());
    close(inPipe[1]); // done writing

    // Read output from child
    std::stringstream output;
    char buffer[4096];
    ssize_t bytes;
    while ((bytes = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
        output.write(buffer, bytes);
    }
    close(outPipe[0]);

    // Wait for child to finish
    int status;
    waitpid(pid, &status, 0);

    return output.str();
}

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

    string bgCol;
    string mode = (node->perspect == NodePerspective::Val)?"":"(expr)";
    int prev=0;
    int cur = node->id;
    if(node->t == NodeType::Call) {
        const auto call = dynamic_pointer_cast<NodeCall>(node);
        if(call->name == "imp") {
            bgCol = (node->perspect == NodePerspective::Val)?"mintcream":"ghostwhite";
            ret += format("subgraph cluster_{} {{ label=\"Imp #{}{}\"; color=indigo; bgcolor={};\n", node->id,node->id, mode, bgCol);

            if(parentId==0) {
                ret+=format("{} [id=\"slnode{}\", label=\"Imp #{}\", color=indigo color=blue shape=invhouse style=filled, fillcolor=lightgreen];\n", node->id, node->id,node->id);
            } else {
                ret+=format("{} [id=\"slnode{}\", label=\"Imp #{}\", color=indigo shape=parallelogram style=filled, fillcolor=lightcyan];\n", node->id,node->id,node->id);
            }

            for(NodeRef& cn : call->args) {
                ret += format("{}\n", nodeToDot(cn, level+1, node->id));
                prev=cur;
                cur=cn->id;

                if(prev && call->args.size() > 1) {
                    ret+=format("{} -> {} [color=indigo];\n", prev, cur); // Control flow
                }
            }

            if(call->args.size() > 1) {
                ret += format("{} -> {} [color=green];\n", cur, node->id); // Data flow
            } else if(call->args.size() == 1) {
                ret += format("{} -> {} [dir=both, style=dashed, color=indigo];\n", cur, node->id); // Data flow
            }
            ret += "}\n";


        } else {
            bgCol = (node->perspect == NodePerspective::Val)?"lightgrey":"ghostwhite";

            ret += format("subgraph cluster_{} {{ label=\"#{}{}\"; color=yellow; bgcolor={};\n", node->id, node->id, mode, bgCol);
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

        println("Usage: ./{} program.slisp > out.html", argv[0]);
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

    string htmlStart = format("<html><head><title>explore {}</title></head><body>", argv[1]);

    string svgSrc = runDot(dotGraph);


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
svg.style.height="50%";
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
    std::string slSrc(size, '\0');
    std::ifstream in(argv[1]);
    in.read(&slSrc[0], size);


    println("{}{}<pre id=\"code\">{}</pre><div id=\"debug\" style=\"border: 1px solid black;\"></div><script>{}{}</script></body></html>",htmlStart,svgSrc, slSrc, jsonStr, script);

	return 0;
}