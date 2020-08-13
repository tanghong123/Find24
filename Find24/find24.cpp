//
//  find24.cpp
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#include "find24.hpp"
#include "literal.hpp"
#include "addsub.hpp"
#include "muldiv.hpp"

#include <iostream>
#include <memory>

void Find24::run(bool debug) {
    buildSolutionMap();
    auto it=solution_.find(elems_);
    if (it == solution_.end()) {
        std::cerr << "Oops, something is wrong!" << std::endl;
        return;
    }
    auto it2=it->second.find(Rational(target_));
    if (it2 == it->second.end()) {
        std::cerr << "Oops, no solution found!" << std::endl;
        return;
    }
    
    for (auto& expr : it2->second) {
        std::cout << expr->toString(false) << "=" << expr->eval().toString() << std::endl;
    }
    
    std::cout << "Found " << it2->second.size() << " solutions" << std::endl;
    
    if (debug) {
        std::cout << "counters: " <<
        "subsets=" << counters_.subsets <<
        ", combos=" << counters_.combos <<
        ", valcombos=" << counters_.valcombos <<
        ", exprcombos=" << counters_.exprcombos <<
        ", uniqexprs=" << counters_.uniqexprs << std::endl;
    }
}

void Find24::addLiterals()
{
    for (auto& elem : elems_) {
        NumVec key;
        key.push_back(elem);
        if (solution_.find(key) == solution_.end()) {
            // avoid duplicated literals
            ExprSet exprs;
            exprs.insert(new Literal(elem));
            ValExprMap value;
            value.insert(make_pair(Rational(elem), exprs));
            solution_.insert(make_pair(key, value));
            ++counters_.subsets;
        }
    }
}

template<typename Op>
void selectK(int n, int k, Op op)
{
    int selection[k];
    int index=0;
    selection[0]=-1;
    while (index>=0) {
        ++selection[index];
        int elemNeeded=k-index-1;
        if (selection[index] + elemNeeded >= n) {
            --index;
            continue;
        }
        
        for (int i=index+1; i<k; ++i) {
            selection[i]=selection[i-1]+1;
        }
        index=k-1;
        
        do {
            op(selection, k);
            ++selection[index];
        } while (selection[index] < n);
        --index;
    }
}

static void splitVec(const NumVec& from, int sel[], int k, NumVec& s1, NumVec& s2)
{
    int index=0;
    for (int i=0; i<from.size(); ++i) {
        if (index>=k) {
            s2.push_back(from[i]);
        } else if (i<sel[index]) {
            s2.push_back(from[i]);
        } else { // i == sel[index]
            s1.push_back(from[i]);
            ++index;
        }
    }
}

class Find24::ValueBuilder {
public:
    ValueBuilder(const NumVec& key, ValExprMap& value, SolutionMap& solution,
                 Counters& counters)
    : key_(key), value_(value), solution_(solution), counters_(counters) { }
    void operator() (int* sel, int k) {
        NumVec s1, s2;
        splitVec(key_, sel, k, s1, s2);
        const ValExprMap& s1_vals=(solution_.find(s1))->second;
        const ValExprMap& s2_vals=(solution_.find(s2))->second;
        // neither s1_vals nor s2_vals should be empty
        for (auto& i : s1_vals) {
            for (auto& j : s2_vals) {
                ++counters_.valcombos;
                doPlus(i, j);
                doMinus(i, j);
                doMinus(j, i);
                doMultiple(i, j);
                doDivision(i, j);
                doDivision(j, i);
            }
        }
        ++counters_.combos;
    }
    
private:
    const NumVec& key_;
    ValExprMap& value_;
    const SolutionMap& solution_;
    Counters& counters_;
    
    void doPlus(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
        Rational result=left.first + right.first;
        ExprSet& exprs=value_[result];
        for (auto& lexpr : left.second) {
            for (auto& rexpr : right.second) {
                ++counters_.exprcombos;
                std::unique_ptr<Expr> expr(new AddSub(lexpr, rexpr, false));
                if (exprs.find(expr.get()) == exprs.end()) {
                    ++counters_.uniqexprs;
                    exprs.insert(expr.release());
                }
            }
        }
    }
    
    void doMinus(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
        Rational result=left.first - right.first;
        if (result < 0) return;
        
        ExprSet& exprs=value_[result];
        for (auto& lexpr : left.second) {
            for (auto& rexpr : right.second) {
                ++counters_.exprcombos;
                std::unique_ptr<Expr> expr(new AddSub(lexpr, rexpr, true));
                if (exprs.find(expr.get()) == exprs.end()) {
                    ++counters_.uniqexprs;
                    exprs.insert(expr.release());
                }
            }
        }
    }
    
    void doMultiple(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
        Rational result=left.first * right.first;
        ExprSet& exprs=value_[result];
        for (auto& lexpr : left.second) {
            for (auto& rexpr : right.second) {
                ++counters_.exprcombos;
                std::unique_ptr<Expr> expr(new MulDiv(lexpr, rexpr, false));
                if (exprs.find(expr.get()) == exprs.end()) {
                    ++counters_.uniqexprs;
                    exprs.insert(expr.release());
                }
            }
        }
    }
    
    void doDivision(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
        if (right.first == 0) return;
        Rational result=left.first / right.first;
        ExprSet& exprs=value_[result];
        for (auto& lexpr : left.second) {
            for (auto& rexpr : right.second) {
                ++counters_.exprcombos;
                std::unique_ptr<Expr> expr(new MulDiv(lexpr, rexpr, true));
                if (exprs.find(expr.get()) == exprs.end()) {
                    ++counters_.uniqexprs;
                    exprs.insert(expr.release());
                }
            }
        }
    }
};

class Find24::SolutionBuilder {
public:
    SolutionBuilder(Find24* parent) : p_(parent) { }
    void operator() (int* sel, int k) {
        NumVec key;
        for (int i=0; i<k; ++i) {
            key.push_back(p_->elems_[sel[i]]);
        }
        if (p_->solution_.find(key) == p_->solution_.end()) {
            ValExprMap value;
            ValueBuilder vb(key, value, p_->solution_, p_->counters_);
            for (int i=1; i<=key.size()/2; ++i) {
                selectK((int)key.size(), i, vb);
            }
            p_->solution_.insert(make_pair(key, value));
            ++p_->counters_.subsets;
        }
    }
    
private:
    Find24* p_;
};

void Find24::buildSolutionMap() {
    addLiterals();
    SolutionBuilder sb(this);
    for (int i=2; i<=elems_.size(); ++i) {
        selectK((int)elems_.size(), i, sb);
    }
}
