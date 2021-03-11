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
    Rank lrank=left->getRank();
    Rank rrank=right->getRank();
    if (lrank != rrank) return (lrank>rrank)?1:-1;
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

static const int BITS_FOR_ETYPE=2;
static const int MAX_ELEMS=sizeof(Rank)*8/BITS_FOR_ETYPE-1;

RankBuilder::RankBuilder(ExprType etype) :
rank_(((Rank)etype) << (BITS_FOR_ETYPE*MAX_ELEMS)),
avail_(MAX_ELEMS) { }

bool RankBuilder::addExprList(const ExprList& exprs)
{
    for (auto& expr : exprs) {
        if (avail_>0) {
            --avail_;
            rank_|=((Rank)expr->getType())
            << (avail_*BITS_FOR_ETYPE);
        } else {
            return false;
        }
    }
    return true;
}

bool RankBuilder::addEOLMarker() {
    if (avail_>0) {
        --avail_; // assuming NONE==0
        return true;
    }
    return false;
}

Rank calcRank(ExprType etype, const ExprList& l1, const ExprList& l2) {
    RankBuilder rb(etype);
    if (!rb.addExprList(l1)) goto _done;
    if (!rb.addEOLMarker()) goto _done;
    rb.addExprList(l2);
_done:
    return rb.getRank();
}
