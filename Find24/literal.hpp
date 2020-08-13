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
        assert(value_.divisor()==1);
        assert(expr->value_.divisor()==1);
        return value_.dividend()-expr->value_.dividend();
    }
    
    std::string toString(bool embed) const {
        return value_.toString();
    }
    
    Rational eval() const {
        return value_;
    }
    
    ETYPE getType() const {
        return LITERAL;
    }
    
private:
    const Rational value_;
};
#endif /* literal_hpp */
