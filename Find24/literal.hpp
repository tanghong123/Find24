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
    Literal(int val) : value_(val) { }
    
    int cmp(const Expr& other) const {
        const Literal* expr=dynamic_cast<const Literal*>(&other);
        return value_-expr->value_;
    }
    
    std::string toString(bool embed) const {
        return std::to_string(value_);
    }
    
    Rational eval() const {
        return Rational(value_);
    }
    
    ETYPE getType() const {
        return LITERAL;
    }
    
private:
    int value_;
};
#endif /* literal_hpp */
