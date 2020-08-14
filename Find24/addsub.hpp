//
//  addsub.hpp
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#ifndef addsub_hpp
#define addsub_hpp

#include "expr.hpp"
#include "rational.hpp"
#include <string>

class AddSub : public Expr {
public:
    AddSub(const Expr* left, const Expr* right, bool isSub) {
        if (left->getType() == ADDSUB) {
            const AddSub* expr=dynamic_cast<const AddSub*>(left);
            add_list_=expr->add_list_;
            sub_list_=expr->sub_list_;
        } else {
            add_list_.push_back(left);
        }
        
        if (right->getType() == ADDSUB) {
            const AddSub* expr=dynamic_cast<const AddSub*>(right);
            if (isSub) {
                mergeList(add_list_, expr->sub_list_);
                mergeList(sub_list_, expr->add_list_);
            } else {
                mergeList(add_list_, expr->add_list_);
                mergeList(sub_list_, expr->sub_list_);
            }
        } else {
            if (isSub) {
                addToList(sub_list_, right);
            } else {
                addToList(add_list_, right);
            }
        }
        
        rank_ = calcRank(ADDSUB, add_list_, sub_list_);
    }
    
    int cmp(const Expr& other) const {
        const AddSub* expr=dynamic_cast<const AddSub*>(&other);
        int ret=compareExprList(add_list_, expr->add_list_);
        if (ret!=0) return ret;
        return compareExprList(sub_list_, expr->sub_list_);
    }
    
    std::string toString(bool embed) const {
        auto it=add_list_.cbegin();
        assert(it != add_list_.cend());
        std::string ret;
        if (embed) ret+="(";
        ret+=(*it)->toString(true);
        
        while (++it != add_list_.cend()) {
            ret+="+";
            ret+=(*it)->toString(true);
        }
        
        for (auto& expr : sub_list_) {
            ret+="-";
            ret+=expr->toString(true);
        }
        
        if (embed) ret+=")";
        
        return ret;
    }
    
    ETYPE getType() const { return ADDSUB; }
    
    Rank getRank() const { return rank_; }
    
    virtual ~AddSub() {
        // all Expr* stored in the two lists are all borrowed references
        // hence no need to free.
    }
    
private:
    ExprList add_list_;
    ExprList sub_list_;
    Rank rank_;
};

#endif /* addsub_hpp */
