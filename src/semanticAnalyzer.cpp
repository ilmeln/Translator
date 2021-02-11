#include "semanticAnalyzer.h"

Node* semanticAnalyzer::factor()
{
    Token t = getToken();
    switch(t.tp) {
        case Int:case Float:case Iden: {
            Node* n = new Node(Factor,t);
            return n;
        }
        case lS: {
            Node* m = new Node;
            m = exp();
            t = getToken();
            if(t.tp!=rS)
                std::cout << "Error: not found )\n";
            return m;
        }
        default: {
            std::cout << "Error factor\n";
            return head;
        }
    }
}


Node* semanticAnalyzer::term()
{
    Node* l = factor();
    while(true) {
        Token t = getToken();
        switch(t.tp) {
            case Prod:case Delen: {
                Node *m = new Node(Term,t);
                m->left = l;
                m->right = factor();
                l = m;
                break;
            }
            default: {
                putback();
                return l;
            }
        }
    }
}


Node* semanticAnalyzer::exp()
{
    Node* n = term();
    while(true) {
        Token t = getToken();
        switch(t.tp) {
            case Plus:case Minus: {
                Node* m = new Node(Exp,t);
                m->left = n;
                m->right = term();
                n = m;
                break;
            }
            case Eq: {
                Node* m = new Node(Exp,t);
                m->left = n;
                m->right = exp();
                return m;
            }
            default: {
                putback();
                return n;
            }
        }
    }
}
Node* semanticAnalyzer::stmt()
{
    Node* l = exp();
    while(true) {
        Token t = getToken();
        switch(t.tp) {
            case zDot: {
                Node *m = new Node(StmtList,t);
                m->left = l;
                m->right = exp();
                l = m;
                break;
            }
            case End: {
                return l;
            }
            default: {
                std::cout << "Error stmt\n";
                return l;
            }
        }
    }
}

std::string semanticAnalyzer::show(Node* n)
{
    switch(n->tp) {
        case Exp:case Term: {
            std::string s = "";
            if(n->tp==Exp)
                s+="Exp->";
            else
                s+="Term->(";
            s =s+ "(" + show(n->left)+")"  +getTokenType(n->tok)+"("+show(n->right)+ "))";
            return s;
        }
        case Factor:
            return n->tok.value;
    }
}

void semanticAnalyzer::showTree()
{
    std::cout << show(head) <<std::endl;
}

double semanticAnalyzer::calc(Node* n)
{
    switch(n->tok.tp) {
    case Int: case Float:
        return std::stod(n->tok.value);
    case Plus:
        return calc(n->left) + calc(n->right);
    case Minus:
        return calc(n->left) - calc(n->right);
    case Prod:
        return calc(n->left) * calc(n->right);
    case Delen:
        return calc(n->left) / calc(n->right);
    default:
        std::cout << "Error calc\n";
        return 0;
    }
}
