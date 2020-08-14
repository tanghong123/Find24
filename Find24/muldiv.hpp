//
//  muldiv.hpp
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#ifndef muldiv_hpp
#define muldiv_hpp

#include "expr.hpp"
#include "rational.hpp"
#include <string>

class MulDiv : public Expr {
public:
    MulDiv(const Expr* left, const Expr* right, bool isDiv) {
        if (left->getType() == MULDIV) {
            const MulDiv* expr=dynamic_cast<const MulDiv*>(left);
            mul_list_=expr->mul_list_;
            div_list_=expr->div_list_;
        } else {
            mul_list_.push_back(left);
        }
        
        if (right->getType() == MULDIV) {
            const MulDiv* expr=dynamic_cast<const MulDiv*>(right);
            if (isDiv) {
                mergeList(mul_list_, expr->div_list_);
                mergeList(div_list_, expr->mul_list_);
            } else {
                mergeList(mul_list_, expr->mul_list_);
                mergeList(div_list_, expr->div_list_);
            }
        } else {
            if (isDiv) {
                addToList(div_list_, right);
            } else {
                addToList(mul_list_, right);
            }
        }
        
        rank_ = calcRank(MULDIV, mul_list_, div_list_);
    }
    
    int cmp(const Expr& other) const {
        const MulDiv* expr=dynamic_cast<const MulDiv*>(&other);
        int ret=compareExprList(mul_list_, expr->mul_list_);
        if (ret!=0) return ret;
        return compareExprList(div_list_, expr->div_list_);
    }
    
    std::string toString(bool embed) const {
        auto it=mul_list_.cbegin();
        assert(it != mul_list_.cend());
        std::string ret=(*it)->toString(true);
        while (++it != mul_list_.cend()) {
            ret+="*";
            ret+=(*it)->toString(true);
        }
        for (auto& expr : div_list_) {
            ret+="/";
            ret+=expr->toString(true);
        }
        
        return ret;
    }
    
    ETYPE getType() const { return MULDIV; }
    
    Rank getRank() const { return rank_; }
    
    virtual ~MulDiv() {
        // all Expr* stored in the two lists are all borrowed references
        // hence no need to free.
    }
private:
    ExprList mul_list_;
    ExprList div_list_;
    Rank rank_;
};
#endif /* muldiv_hpp */
