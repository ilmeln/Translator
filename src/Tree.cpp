#include "Tree.h"
#include <math.h>

Node* Tree::factor()
{
    Token t = getToken();
    if(1) {
        Node* n = new Node(t.value,Factor);
        return n;
    }
    else if(t.value=="(") {
        Node* e = new Node;
        e = exp();
        t = getToken();
        if(t.value==")") {
            return e;
        }
        else {
            std::cout << "Error ()" <<std::endl;
            return head;
        }
    }
    else {
        std::cout << "Error:factor\n";
        return head;
    }
}


Node* Tree::term()
{
    Node* ml = f();
    while(true) {
        Token t = getToken();
        if(t.value=="*"||t.value=="/") {
            Node* m = new Node(t.value,Term);
            m->left = ml;
            m->right = f();
            ml = m;
        }
        else {
            putback();
            return ml;
        }
    }
}


Node* Tree::exp()
{
    Node* lf = term();
    while(true) {
        Token t = getToken();
        if(t.tp==End)
            return lf;
        if(t.value=="+"||t.value=="-") {
            Node *m = new Node(t.value,Exp);
            m->left = lf;
            m->right = term();
            lf = m;
        }
        else if(t.value=="=") {
            return lf;
        }
        else {
            putback();
            return lf;
        }
    }
}


void Tree::makeTree()
{
    head = exp();
}




std::string Tree::show(Node *n)
{
    if(n->left==nullptr&&n->right==nullptr) {
        return n->val;
    }
    else {
        std::string s = "";
        s += getStringNodeType(n->tp);
        s += "->(" + show(n->left) + n->val + show(n->right) + ")";
        return s;
    }
}

std::string getStringNodeType(NodeType t)
{
    switch(t) {
    case Exp:
        return "Exp";
    case Term:
        return "Term";
    case Factor:
        return "Factor";
    case Def:
        return "Def";
    case Pow:
        return "Pow";
    }
}

void Tree::showTree()
{
    std::cout << show(head) << std::endl;
}


double Tree::calc(Node *n)
{
    if(n->val=="+")
        return calc(n->left)+calc(n->right);
    else if(n->val=="-")
        return calc(n->left)-calc(n->right);
    else if(n->val=="*")
        return calc(n->left)*calc(n->right);
    else if(n->val=="/")
        return calc(n->left)/calc(n->right);
    else if(n->val=="^")
        return pow(calc(n->left),calc(n->right));
    else
        return std::stod(n->val);
}





void Tree::calcTree()
{
    std::cout << "/n/n/n==" << calc(head) << std::endl;
}

Node* Tree::f()
{
    Node* m = factor();
    while(true) {
        Token t = getToken();
        if(t.value=="^") {
            Node *n = new Node(t.value,Pow);
            n->left = m;
            n->right = factor();
            m = n;
        }
        else {
            putback();
            return m;
        }
    }
}
