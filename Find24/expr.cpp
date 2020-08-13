//
//  expr.cpp
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#include "expr.hpp"
#include <stdio.h>

int cmpExpr(const Expr* left, const Expr* right) {
    ETYPE ltype=left->getType();
    ETYPE rtype=right->getType();
    if (ltype != rtype) return ltype-rtype;
    return left->cmp(*right);
}

int compareExprList(const ExprList& left, const ExprList& right)
{
    auto it1=left.cbegin();
    auto it2=right.cbegin();
    while ( (it1 != left.cend()) && (it2 != right.cend()) ) {
        int ret=cmpExpr(*it1, *it2);
        if (ret != 0) return ret;
        ++it1;
        ++it2;
    }
    if (it1 != left.cend()) return 1;
    if (it2 != right.cend()) return -1;
    
    return 0;
}

void addToList(ExprList& list, const Expr* expr) {
    auto it=list.begin();
    while ( (it != list.end()) && (cmpExpr(*it, expr)<0) )
        ++it;
    list.insert(it, expr);
}

// Assuming both lists are already sorted
void mergeList(ExprList& to, const ExprList& from) {
    auto it_to=to.begin();
    for (auto& expr : from) {
        while ( (it_to != to.end()) && (cmpExpr(*it_to, expr)<0) )
            ++it_to;
        to.insert(it_to, expr);
    }
}
