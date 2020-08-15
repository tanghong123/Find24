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
#include "selectk.hpp"

#include <iostream>
#include <memory>

void Find24::run(bool debug) {
    buildSolutionMap();
    
    if (debug) {
        std::cout << "counters: " << std::endl <<
        "subsets=" << counters_.subsets << std::endl <<
        "combos=" << counters_.combos << std::endl <<
        "newvalues=" << counters_.newvalues << std::endl <<
        "valcombos=" << counters_.valcombos << std::endl <<
        "exprcombos=" << counters_.exprcombos << std::endl <<
        "uniqexprs=" << counters_.uniqexprs << std::endl <<
        "csubsets=" << counters_.csubsets << std::endl <<
        "ccombos=" << counters_.ccombos << std::endl <<
        "cvalcombos=" << counters_.cvalcombos << std::endl <<
        std::endl;
    }
}

std::vector<std::string> Find24::getExprs() const {
    std::vector<std::string> ret;
    auto it=solution_.find(elems_);
    if (it == solution_.end()) {
        std::cerr << "Oops, something is wrong!" << std::endl;
        return ret;
    }
    
    auto it2=it->second.find(Rational(target_));
    if (it2 == it->second.end()) {
        return ret;
    }
    
    for (auto& expr : it2->second) {
        ret.push_back(expr->toString(false));
    }
    
    return ret;
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

void Find24::addRootConstraint() {
    ValSet value;
    value.insert(Rational(target_));
    constraint_.insert(make_pair(elems_, value));
}


static void splitVec(const NumVec& from, int sel[], int k, NumVec& s1,
                     NumVec& s2)
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
    ValueBuilder(const NumVec& key, ValExprMap& value,
                 const SolutionMap& solution,
                 const ValSet* constraint, Counters& counters)
    : key_(key), value_(value), solution_(solution), constraint_(constraint), counters_(counters) { }
    
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
    const ValSet* constraint_;
    Counters& counters_;
    
    void doPlus(const ValExprMap::value_type& left,
                const ValExprMap::value_type& right)
    {
        Rational result=left.first + right.first;
        if ( (constraint_) && (constraint_->find(result)==constraint_->end()) )
            return;
        auto it = value_.find(result);
        if (it == value_.end()) {
            ++counters_.newvalues;
            it=value_.insert(make_pair(result, ExprSet())).first;
        }
        ExprSet& exprs=it->second;
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
    
    void doMinus(const ValExprMap::value_type& left,
                 const ValExprMap::value_type& right)
    {
        Rational result=left.first - right.first;
        if (result < Rational(0)) return;
        
        if ( (constraint_) && (constraint_->find(result)==constraint_->end()) )
            return;
        
        auto it = value_.find(result);
        if (it == value_.end()) {
            ++counters_.newvalues;
            it=value_.insert(make_pair(result, ExprSet())).first;
        }
        ExprSet& exprs=it->second;
        bool checkZero = result==Rational(0);
        for (auto& lexpr : left.second) {
            for (auto& rexpr : right.second) {
                ++counters_.exprcombos;
                if (checkZero && cmpExpr(lexpr, rexpr)>0) continue;
                std::unique_ptr<Expr> expr(new AddSub(lexpr, rexpr, true));
                if (exprs.find(expr.get()) == exprs.end()) {
                    ++counters_.uniqexprs;
                    exprs.insert(expr.release());
                }
            }
        }
    }
    
    void doMultiple(const ValExprMap::value_type& left,
                    const ValExprMap::value_type& right)
    {
        Rational result=left.first * right.first;
        if ( (constraint_) && (constraint_->find(result)==constraint_->end()) )
            return;
        
        auto it = value_.find(result);
        if (it == value_.end()) {
            ++counters_.newvalues;
            it=value_.insert(make_pair(result, ExprSet())).first;
        }
        ExprSet& exprs=it->second;
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
    
    void doDivision(const ValExprMap::value_type& left,
                    const ValExprMap::value_type& right)
    {
        if (right.first == Rational(0)) return;
        Rational result=left.first / right.first;
        if ( (constraint_) && (constraint_->find(result)==constraint_->end()) )
            return;
        
        auto it = value_.find(result);
        if (it == value_.end()) {
            ++counters_.newvalues;
            it=value_.insert(make_pair(result, ExprSet())).first;
        }
        ExprSet& exprs=it->second;
        bool checkOne=result==Rational(1);
        for (auto& lexpr : left.second) {
            for (auto& rexpr : right.second) {
                ++counters_.exprcombos;
                if (checkOne && cmpExpr(lexpr, rexpr)>0) continue;
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
    SolutionBuilder(Find24& parent, bool check_constraint) :
    p_(parent), check_constraint_(check_constraint) { }
    
    void operator() (int* sel, int k) {
        NumVec key;
        for (int i=0; i<k; ++i) {
            key.push_back(p_.elems_[sel[i]]);
        }
        if (p_.solution_.find(key) == p_.solution_.end()) {
            ValExprMap value;
            ValSet* constraint=0;
            if (check_constraint_) {
                auto it=p_.constraint_.find(key);
                assert(it != p_.constraint_.end());
                constraint=&(it->second);
            }
            ValueBuilder vb(key, value, p_.solution_, constraint, p_.counters_);
            for (int i=1; i<=key.size()/2; ++i) {
                selectK((int)key.size(), i, vb);
            }
            p_.solution_.insert(make_pair(key, value));
            ++p_.counters_.subsets;
        }
    }
    
private:
    Find24& p_;
    bool check_constraint_;
};

class Find24::CVBuilder {
public:
    CVBuilder(const NumVec& ckey, const NumVec& eelems, ValSet& value,
              const ConstraintMap& constraint, const SolutionMap& solution,
              Counters& counters) :
    ckey_(ckey), eelems_(eelems), value_(value), constraint_(constraint),
    solution_(solution), counters_(counters) { }
    
    void operator() (int* sel, int k) {
        // we are trying to find the constraint for one of the operands (ckey)
        // of the following equations: (sum = ckey op other) and
        // (sum = other op ckey), with the knowledge of the constraints for sum
        // and the possible values of others.
        NumVec sum, other;
        int ck_index=0;
        for (int i=0; i<k; ++i) {
            other.push_back(eelems_[sel[i]]);
            while ( (ck_index < ckey_.size()) &&
                   (ckey_[ck_index]<=eelems_[sel[i]]) ) {
                sum.push_back(ckey_[ck_index]);
                ++ck_index;
            }
            sum.push_back(eelems_[sel[i]]);
        }
        
        while (ck_index < ckey_.size()) {
            sum.push_back(ckey_[ck_index]);
            ++ck_index;
        }
        
        const ValSet& sum_constraint=(constraint_.find(sum))->second;
        const ValExprMap& other_values=(solution_.find(other))->second;
        
        // neither sum_constraint nor right_values should be empty
        for (auto& i : sum_constraint) {
            for (auto& j : other_values) {
                ++counters_.cvalcombos;
                doMinus(i, j.first); // i = x + j
                doPlus(i, j.first); // i = x - j
                doMinus(j.first, i); // i = j - x
                doDivision(i, j.first); // i = x*j
                doMultiple(i, j.first); // i = x/j
                doDivision(j.first, i); // i = j/x
            }
        }
        ++counters_.ccombos;
    }
    
private:
    const NumVec& ckey_;
    const NumVec& eelems_;
    ValSet& value_;
    const ConstraintMap& constraint_;
    const SolutionMap& solution_;
    Counters& counters_;
    
    void doPlus(const Rational& left, const Rational& right)
    {
        Rational result=left + right;
        value_.insert(result);
    }
    
    void doMinus(const Rational& left, const Rational& right)
    {
        Rational result=left - right;
        if (result < 0) return;
        value_.insert(result);
    }
    
    void doMultiple(const Rational& left, const Rational& right)
    {
        Rational result=left * right;
        value_.insert(result);
    }
    
    void doDivision(const Rational& left, const Rational& right)
    {
        if (right == 0) return;
        Rational result=left / right;
        value_.insert(result);
    }
};

class Find24::ConstraintBuilder {
public:
    ConstraintBuilder(Find24& p) : p_(p) {}
    void operator () (int* sel, int k) {
        NumVec ckey; // key to the constraint map
        NumVec eelems; // expanding elems
        splitVec(p_.elems_, sel, k, eelems, ckey);
        if (p_.constraint_.find(ckey) == p_.constraint_.end()) {
            ValSet value;
            CVBuilder cvb(ckey, eelems, value, p_.constraint_, p_.solution_,
                          p_.counters_);
            for (int i=1; i<=(int)eelems.size(); ++i) {
                selectK((int)eelems.size(), i, cvb);
            }
            p_.constraint_.insert(make_pair(ckey, value));
            ++p_.counters_.csubsets;
        }
    }
    
private:
    Find24& p_;
};

void Find24::buildSolutionMap() {
    addLiterals();
    SolutionBuilder sb(*this, false);
    for (int i=2; i<=elems_.size()/2; ++i) {
        selectK((int)elems_.size(), i, sb);
    }
    
    addRootConstraint();
    ConstraintBuilder cb(*this);
    for (int i=1; i<=elems_.size()/2; ++i) {
        selectK((int)elems_.size(), i, cb);
    }
    
    SolutionBuilder sb2(*this, true);
    for (int i=(int)elems_.size()/2+1; i<=elems_.size(); ++i) {
        selectK((int)elems_.size(), i, sb2);
    }
}

void Find24::freeSolutionMap() {
    for (auto& x : solution_) {
        for (auto& y : x.second) {
            for (auto& z : y.second) {
                delete z;
            }
        }
    }
}
