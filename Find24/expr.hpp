//
//  expr.h
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#ifndef expr_h
#define expr_h

#include "rational.hpp"
#include <list>

enum ETYPE { NONE, LITERAL, ADDSUB, MULDIV };

typedef uint32_t Rank;

// All Expr are immutable after construction
class Expr {
public:
    virtual int cmp(const Expr& other) const = 0;
    virtual std::string toString(bool embed) const = 0;
    virtual ETYPE getType() const = 0;
    virtual Rank getRank() const = 0;
    virtual ~Expr() { }
};

typedef std::list<const Expr*> ExprList;

int cmpExpr(const Expr* left, const Expr* right);
int compareExprList(const ExprList& left, const ExprList& right);
void addToList(ExprList& list, const Expr* expr);
void mergeList(ExprList& to, const ExprList& from);

class RankBuilder {
public:
    RankBuilder(ETYPE etype);
    bool addExprList(const ExprList& exprs);
    bool addEOLMarker();
    Rank getRank() const { return rank_; }
private:
    Rank rank_;
    int avail_;
};

Rank calcRank(ETYPE etype, const ExprList& l1, const ExprList& l2);

#endif /* expr_h */
