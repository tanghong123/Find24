//
//  literal.hpp
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#ifndef literal_hpp
#define literal_hpp

#include "expr.hpp"
#include "rational.hpp"
#include <string>

class Literal : public Expr {
public:
    Literal(int val) :
    lit_(val), rank_(RankBuilder(ExprType::LITERAL).getRank()) { }
    
    int cmp(const Expr& other) const {
        const Literal* expr=dynamic_cast<const Literal*>(&other);
        return lit_ - expr->lit_;
    }
    
    std::string toString(bool embed) const {
        return std::to_string(lit_);
    }
    
    ExprType getType() const {
        return ExprType::LITERAL;
    }
    
    Rank getRank() const {
        return rank_;
    }
    
private:
    const int lit_;
    Rank rank_;
};
#endif /* literal_hpp */
