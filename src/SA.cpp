#include "SA.h"

#include <fstream>



std::string NodeTypeToString(NodeType t)
{
    switch(t) {
        case Factor:
            return "factor";
        case Term:
            return "term";
        case Exp:
            return "expression";
        case Stmt:
            return "statement";
        case logExp:
            return "log_exp";
        case addExp:
            return "additional_exp";
        case StmtExp:
            return "stmt_exp";
        case Statements:
            return "statements";
        case PRINT:
            return "print";
        default:
            return "Unknown";
    }
}




Node* SA::factor()
{
    Token t = getToken();
    switch(t.tp) {
        case Iden: {
            for(auto s : vars) {
                if(s.name==t.value) {
                    Node* n = new Node(Factor,t);
                    return n;
                }
            }

            error_f("Not inint variable " + t.value +'\n');
            return head;
        }
        case String:case Int:case Float: {
            Node* n = new Node(Factor,t);
            return n;
        }
        case lS: {
            Node* m = new Node;
            m = exp();
            t = getToken();
            if(t.tp!=rS) {
                error_f("Error: not found )\n");
            }
            Node* n = new Node(SKOBE,1);
            n->push(0,m);
            return n;
        }
        default: {
            error_f("Unexpected " + getTokenType(t) + "\n");
            return head;
        }
    }
}


Node* SA::term()
{
    Node* left = factor();
    while(true) {
        if(Error)
            return left;
        Token t = getToken();
        switch(t.tp) {
            case Prod: case Delen: {
                Node* r = new Node(Term,t,2);
                r->push(0,left);
                r->push(1,factor());
                left = r;
                break;
            }
            default: {
                putBack();
                return left;
            }
        }
    }
}


Node* SA::additive_exp()
{
    Node* left = term();
    while(true) {
        if(Error)
            return left;
        Token t = getToken();
        switch(t.tp) {
            case Plus:case Minus: {
                Node *r = new Node(addExp,t,2);
                r->push(0,left);
                r->push(1,term());
                left = r;
                break;
            }
            default: {
                putBack();
                return left;
            }
        }
    }
}


Node* SA::log_exp()
{
    Node* left = additive_exp();
    while(true) {
        if(Error)
            return left;
        Token t = getToken();
        switch(t.tp) {
            case Less:case Larger:case Ravno: {
                Node *r = new Node(logExp,t,2);
                r->push(0,left);
                r->push(1,additive_exp());
                left = r;
                break;
            }
            default:
                putBack();
                return left;
        }
    }
}


Node* SA::exp()
{
    Node* left = log_exp();
    while(true) {
        if(Error)
            return left;
        Token t = getToken();
        switch(t.tp) {
            case And:case Or: {
                Node *r = new Node(Exp,t,2);
                r->push(0,left);
                r->push(1,log_exp());
                left = r;
                break;
            }
            default: {
                putBack();
                return left;
            }
        }
    }

}

Node* SA::stmtExp()
{
    Token t = getToken();
    if(t.tp!=Iden) {
        error_f("Unexpected "+getTokenType(t) + "\n");
        return head;
    }
    std::string varName = t.value;
    Node* left = new Node(Factor,t);
    t = getToken();
    if(t.tp!=Eq) {
        error_f("Unexpected " + getTokenType(t.tp) + '\n');
        return head;
    }
    Node* m = new Node(StmtExp,t,2);
    m->push(0,left);
    TokenType tp;
    Node* right = exp();

    tp = check(right);
    m->push(1,right);
    vars.push_back(Var(tp,varName));
    return m;
}


Node* SA::stmt()
{
    if(Error)
        return head;
    Token t = getToken();
    switch(t.tp) {
        case Iden: {
            putBack();
            Node *n = stmtExp();
            t = getToken();
            if(t.tp!=zDot) {
                error_f("Unexpected " +getTokenType(t) +'\n');
                return head;
            }
            return n;
        }
        case lK: {
            t = getToken();
            if(t.tp==rK) {
                Node* n = new Node(Empty);
                return n;
            }
            putBack();
            Node* left = stmt();
            while(true) {
                if(Error)
                    return left;
                t = getToken();
                if(t.tp == rK) {
                    if(left->Type==StmtList)
                        left->Type = Statements;
                    return left;
                }
                if(index>=tokens.size()) {
                    error_f("Expected }\n");
                    return left;
                }
                putBack();
                Node* m = new Node(StmtList,2);
                m->push(0,left);
                m->push(1,stmt());
                left = m;
            }
        }
        case While: {
            Node *n =new Node(WHILE,2);
            t = getToken();
            if(t.tp!=lS) {
                error_f("Expected (\n");
                return n;
            }
            Node* left = exp();
            if(check(left)==Err) {
                return n;
            }
            n->push(0,left);
            t = getToken();
            if(t.tp!=rS) {
                error_f("Expected )\n");
                return n;
            }
            n->push(1,stmt());
            return n;
        }
        case If: {
            Node *n =new Node(IF,2);
            t = getToken();
            if(t.tp!=lS) {
                error_f("Expected (\n");
                return n;
            }
            Node* left = exp();
            if(check(left)==Err) {
                return n;
            }
            n->push(0,left);
            t = getToken();
            if(t.tp!=rS) {
                error_f("Expected )\n");
                return n;
            }
            n->push(1,stmt());
            t = getToken();
            if(t.tp!=Else) {
                putBack();
                return n;
            }
            Node* m = new Node(ELSE,1);
            m->push(0,stmt());
            Node* i = new Node(IFELSE,2);
            i->push(0,n);
            i->push(1,m);
            return i;
        }
        case Print: {
            Node* n = new Node(PRINT,1);
            t = getToken();
            if(t.tp!=lS) {
                error_f("Expected ( after print\n");
                return n;
            }
            Node* f = factor();
            if(f->Type!=Factor) {
                error_f("Expected factor\n");
                return n;
            }
            n->push(0,f);
            t = getToken();
            if(t.tp!=rS) {
                error_f("Expected ) after factor\n");
                return n;
            }
            t = getToken();
            if(t.tp!=zDot) {
                error_f("Expected ;\n");
            }
            return n;
        }
        default: {
            error_f("Unexpected " + getTokenType(t) +'\n');
            putBack();
            return head;
        }
    }
}



Node* SA::stmtList()
{
    Node *left = stmt();
    while(true) {
        if(Error||index>=tokens.size()-1) {
            left->Type = Statements;
            return left;
        }
        Node* m = new Node(StmtList,2);
        m->push(0,left);
        m->push(1,stmt());
        left = m;
    }
}



std::string SA::show(Node *n,int t)
{
    std::string s = "";
    if(t) {
        for(int i=0;i<t;++i)
            s += "-";
    }
    switch(n->Type) {
        case Exp:case Term:case logExp:case addExp:case Stmt:case StmtExp:{

            s += NodeTypeToString(n->Type)+ " "+ getTokenType(n->tok)+"\n"+ show(&(n->nodes[0]),t+1)  +show((&n->nodes[1]),t+1);
            return s;
        }
        case StmtList: {
            return show(&n->nodes[0],t) + show(&n->nodes[1],t);
        }
        case Statements: {
            s += "Statements\n" + show(&n->nodes[0],t+1) + show(&n->nodes[1],t+1);
            return s;
        }
        case WHILE: {
            s += "while ( "+NodeTypeToString(n->nodes[0].Type)+" ) "+NodeTypeToString(n->nodes[1].Type)+"\n" + show(&n->nodes[0],t+1) +  show(&n->nodes[1],t+1);
            return s;
        }
        case IF: {
            s += "if( " +NodeTypeToString(n->nodes[0].Type)+" ) "+NodeTypeToString(n->nodes[1].Type)+"\n"+show(&n->nodes[0],t+1)+show(&n->nodes[1],t+1);
            return s;
        }
        case ELSE: {
            s += "else " +NodeTypeToString(n->nodes[0].Type)+"\n" +show(&n->nodes[0],t+1);
            return s;
        }
        case IFELSE: {
            return show(&n->nodes[0],t) +show(&n->nodes[1],t);

        }
        case Factor: {
            s += getTokenType(n->tok) + "( " + n->tok.value + " )\n";
            return s;
        }
        case SKOBE: {
            return show(&n->nodes[0],t);

        }
        case PRINT: {
            s += "print "+show(&n->nodes[0],0);
            return s;
        }
        default: {
            return "";
        }
    }
}

void SA::inPython()
{
    std::ofstream fout("code.py",std::ios::trunc);
    fout << toPython(head);
    fout.close();
}

std::string SA::toPython(Node* n)
{
    std::string s = "";
    if(tab) {
        for(int i=0;i<tab;++i)
            s += " ";
    }
    switch(n->Type) {
        case Exp:case Term:case logExp:case addExp:case Stmt:case StmtExp:{
            s += toPython(&(n->nodes[0])) +" "+getTokenType(n->tok)+" "+toPython((&n->nodes[1]));
            return s;
        }
        case StmtList:case Statements: {
            return toPython(&n->nodes[0]) + '\n' + toPython(&n->nodes[1]);
        }
        case WHILE: {
            ++tab;
            s += "while(" + toPython(&n->nodes[0]) + "):\n" + toPython(&n->nodes[1]) + "\n";
            --tab;
            return s;
        }
        case IF: {
            ++tab;
            s += "if("+toPython(&n->nodes[0])+"):\n"+toPython(&n->nodes[1])+ "\n";
            --tab;
            return s;
        }
        case ELSE: {
            ++tab;
            s += "else:\n" +toPython(&n->nodes[0])+ "\n";
            --tab;
            return s;
        }
        case IFELSE: {
            return toPython(&n->nodes[0]) +toPython(&n->nodes[1]);
        }
        case Factor: {
            return n->tok.value;
        }
        case SKOBE: {
            s += "(" + toPython(&n->nodes[0]) + ")";
            return s;
        }
        case PRINT: {
            s += "print("+toPython(&n->nodes[0])+")\n";
            return s;
        }
        default: {
            return "";
        }
    }
}





TokenType SA::check(Node* n)
{
    if(Error)
        return Err;
    switch(n->Type) {
        case Factor: {
            if(n->tok.tp==Int||n->tok.tp==Float||n->tok.tp==String)
                return n->tok.tp;
            else if(n->tok.tp==Iden) {
                for(auto v : vars) {
                    if(v.name==n->tok.value)
                        return v.type;
                }
                error_f("Unknow iden type\n");
                return Err;
            }
        }
        case SKOBE: {
            return check(&n->nodes[0]);
        }
        case Term: {
            TokenType l = check(&n->nodes[0]);
            TokenType r = check(&n->nodes[1]);
            if(n->tok.tp==Prod) {
                if(l==String&&r==String) {
                    error_f("str * str\n");
                    return Err;
                }
                else if((l==String&&r==Int)||(l==Int&&r==String))
                    return String;
                else if((l==String&&r==Float)||(l==Float&&r==String)) {
                    error_f("str * float\n");
                    return Err;
                }
                else if((l==Float&&r==Int)||(l==Int&&r==Float))
                    return Float;
                else if(l==Int&&r==Int)
                    return Int;
                else if(l==Float&&r==Float)
                    return Float;
                else
                    return Err;
            }
            else if(n->tok.tp==Delen) {
                if(l==String||r==String) {
                    error_f("Can't division str\n");
                    return Err;
                }
                else
                    return Float;
            }
        }
        case addExp: {
            TokenType l = check(&n->nodes[0]);
            TokenType r = check(&n->nodes[1]);
            if(n->tok.tp == Plus) {
                if(l==String&&r==String)
                    return String;
                else if((l==String&&r!=String)||(l!=String&&r==String)) {
                    error_f("str + nummber\n");
                    return Err;
                }
                else if(l==Int&&r==Int)
                    return Int;
                else
                    return Float;
            }
            else if(n->tok.tp==Minus) {
                if(l==String||r==String) {
                    error_f("Unsupported operand \"-\" for str\n");
                    return Err;
                }
                else if(l==Int&&r==Int)
                    return Int;
                else
                    return Float;
            }
        }
        case logExp: {
            TokenType l = check(&n->nodes[0]);
            TokenType r = check(&n->nodes[1]);
            if(n->tok.tp==Less||n->tok.tp==Larger) {
                if((l==String&&r!=String)||(l!=String&&r==String)) {
                    error_f("Can't compare string and number\n");
                    return Err;
                }
            }
            return Int;
        }
        case Exp: {
            return Int;
        }
        default: {
            return Err;
        }
    }
}

void SA::error_f(std::string s)
{
    if(Error)
        return;
    Error = true;
    errorMessage = s;
}
