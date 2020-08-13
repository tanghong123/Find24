//
//  find24.h
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#ifndef find24_h
#define find24_h

#include <vector>
#include <set>
#include <map>

#include "rational.hpp"
#include "expr.hpp"

class ExprCmp {
public:
    bool operator () (const Expr* left, const Expr* right) const {
        return cmpExpr(left, right) < 0;
    }
};

// elems must be sorted in ascending order, could have duplicated elems
typedef std::vector<int> NumVec;
typedef std::set<Expr*, ExprCmp> ExprSet;
typedef std::map<Rational, ExprSet> ValExprMap;
typedef std::map<NumVec, ValExprMap> SolutionMap;

// Although the name comes from the game find-24, this class is a general
// solution that can find arithmatic expressions that would yield a specific
// target number (other than 24) using any number of positive integers
// (instead of 4 integers between 1 to 13).
// It will find all possible solutions, but will not show duplicates under
// commutative or associative laws.
class Find24 {
public:
    Find24(int target, std::vector<int> elems) :
    target_(target), elems_(elems)
    {
        std::sort(elems_.begin(), elems_.end());
    }
    
    void run(bool debug);
    
    ~Find24() {
        freeSolutionMap();
    }
    
private:
    int target_;
    NumVec elems_;
    SolutionMap solution_;
    
    struct Counters {
        int subsets;
        int combos;
        int valcombos;
        int exprcombos;
        int uniqexprs;
        Counters() : subsets(0), combos(0), valcombos(0), exprcombos(0), uniqexprs(0) { }
    } counters_;
    
    void addLiterals();
    class ValueBuilder;
    class SolutionBuilder;
    void buildSolutionMap();
    void freeSolutionMap();
};

#endif /* find24_h */
