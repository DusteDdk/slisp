
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

using namespace std;


string nodeToDot(NodeRef node, int level, int parentId)
{

    string padStr = "";
    padStr.insert(padStr.begin(), level*2, ' ');
    string ret="";

    int prev=0;
    int cur = node->id;
    if(node->t == NodeType::Call) {
        const auto call = dynamic_pointer_cast<NodeCall>(node);
        if(call->name == "imp") {
            ret += format("{}subgraph cluster_{} {{ label=\"Imp #{}\"; color=indigo; bgcolor=mintcream;\n", padStr, node->id,node->id);

            if(parentId==0) {
                ret+=format("nEntryPoint [label=\"Main\", color=blue shape=invhouse style=filled, fillcolor=lightgreen];\n");
                ret+=format("nEntryPoint -> {} [color=darkblue, style=dashed, dir=both];\n", node->id);
            }

            ret+=format("{}{} [label=\"Imp #{}\", color=indigo shape=parallelogram style=filled, fillcolor=lightcyan];\n", padStr,node->id,node->id);
            for(NodeRef& cn : call->args) {
                ret += format( "{}{}\n", padStr, nodeToDot(cn, level+1, node->id));
                prev=cur;
                cur=cn->id;

                if(prev) {
                    ret+=format("{}{} -> {} [color=indigo];\n", padStr,prev, cur); // Control flow
                }
            }

            ret += format("{} -> {} [color=green];\n", cur, node->id); // Data flow

            ret += format("{}}}\n",padStr);


        } else {
            ret += format("{}subgraph cluster_{} {{ label=\"Expr #{}\"; color=yellow; bgcolor=lightgrey;\n", padStr, node->id, node->id);
            ret+=format("{}{} [label=\"#{} {}\", shape=cds, style=filled, color=yellow, fillcolor=lightyellow];\n", padStr,node->id,node->id, call->name);
            cur=node->id;
            int argNum=0;
            for(NodeRef& cn : call->args) {
                ret += format( "{}{}\n", padStr, nodeToDot(cn, level+1, node->id));
                prev=cur;
                cur=cn->id;

                ret+=format("{}{} -> {} [style=invis];\n", padStr,cur, prev); // Data flow

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

                ret+=format("{}{} -> {} [dir=both, style=dashed, color=black, label=\"{}\"];\n", padStr, node->id, cur, edgeLabel); // Data flow

            }

            ret += format("{}}}\n",padStr);
           // ret += format("{} -> {} [color=green];\n", node->id, parentId); // Data flow
        }

       // ret+=format("{}}}; {} -> {};\n", padStr,node->id, node->id) ;
    } else if(node->t == NodeType::Variable) {
        const auto var = dynamic_pointer_cast<NodeVariable>(node);

        ret+=format("{}{} [label=\"{}\", shape=note, style=filled, fillcolor=papayawhip];\n", padStr,node->id, node->toString());
        ret += format("{}{}\n", padStr, nodeToDot(var->valProvider, level, node->id));
        ret += format("{} -> {} [color=darkgreen];\n", var->valProvider->id, node->id); // Data flow
        ret += format("{} -> {} [color=darkblue];\n", node->id, var->valProvider->id); // Control flow


    } else {
        ret=format("{}{} [label=\"{}\", shape=rect, style=filled, fillcolor=ivory];", padStr,node->id, node->toString());
    }


    return ret;
}


string toDot(NodeRef node)
{
    return format("digraph G {{ bgcolor=lightblue2;\n{} }}", nodeToDot(node, 1, 0));
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
    std::println( "{}", toDot( firstNode ) );
	return 0;
}