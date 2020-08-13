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

enum ETYPE { LITERAL, ADDSUB, MULDIV };

// All Expr are immutable after construction
class Expr {
public:
    virtual int cmp(const Expr& other) const = 0;
    virtual std::string toString(bool embed) const = 0;
    virtual ETYPE getType() const = 0;
    virtual ~Expr() { }
};

typedef std::list<const Expr*> ExprList;

int cmpExpr(const Expr* left, const Expr* right);
int compareExprList(const ExprList& left, const ExprList& right);
void addToList(ExprList& list, const Expr* expr);
void mergeList(ExprList& to, const ExprList& from);

#endif /* expr_h */
