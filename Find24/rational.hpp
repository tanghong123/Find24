//
//  rational.h
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#ifndef rational_hpp
#define rational_hpp

#include <assert.h>
#include <string>

class Rational {
public:
    Rational(int64_t dividend, int64_t divisor=1) :
    dividend_(dividend),
    divisor_(divisor)
    {
        assert(divisor != 0);
        normalize();
    }
    
    Rational operator + (const Rational& other) const
    {
        return Rational(
                        dividend_*other.divisor_+other.dividend_*divisor_,
                        divisor_*other.divisor_);
    }
    
    Rational operator - (const Rational& other) const
    {
        return Rational(
                        dividend_*other.divisor_-other.dividend_*divisor_,
                        divisor_*other.divisor_);
    }
    
    Rational operator * (const Rational& other) const
    {
        return Rational(dividend_*other.dividend_, divisor_*other.divisor_);
    }
    
    Rational operator / (const Rational& other) const
    {
        return Rational(dividend_*other.divisor_, divisor_*other.dividend_);
    }
    
    bool operator == (const Rational& other) const
    {
        return (dividend_==other.dividend_) && (divisor_==other.divisor_);
    }
    
    bool operator < (const Rational& other) const
    {
        return dividend_*other.divisor_-other.dividend_*divisor_ < 0;
    }
    
    int cmp(const Rational& other) const
    {
        int64_t result = dividend_*other.divisor_-other.dividend_*divisor_;
        return (result == 0) ? 0 : (result > 0) ? 1 : -1;
    }
    
    std::string toString() const {
        if (dividend_==0) return "0";
        std::string ret=std::to_string(dividend_);
        if (divisor_!=1) {
            ret+="/";
            ret+=std::to_string(divisor_);
        }
        
        return ret;
    }
    
    int64_t dividend() const { return dividend_; }
    int64_t divisor() const { return divisor_; }
    
private:
    int64_t dividend_;
    int64_t divisor_;
    static int64_t gdc(int64_t left, int64_t right) {
        assert(right!=0);
        // making sure both parameters are non-negative
        if (left<0) left=-left;
        if (right<0) right=-right;
        
        if (left==0) return right;
        
        // both left and right are positive numbers
        if (left<right) std::swap(left, right);
        
        while ( (left%right) != 0) {
            int64_t tmp=left%right;
            left=right;
            right=tmp;
        }
        return right;
    }
    
    void normalize() {
        int64_t x=gdc(dividend_, divisor_);
        dividend_/=x;
        divisor_/=x;
        // make sure divisor is always positive
        if (divisor_<0) {
            divisor_=-divisor_;
            dividend_=-dividend_;
        }
    }
};

#endif /* rational_hpp */
