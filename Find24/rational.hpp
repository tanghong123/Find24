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
    Rational(int dividend, int divisor=1) :
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
        return dividend_*other.divisor_-other.dividend_*divisor_;
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
    
    int dividend() const { return dividend_; }
    int divisor() const { return divisor_; }
    
private:
    int dividend_;
    int divisor_;
    static int gdc(int left, int right) {
        assert(right!=0);
        // making sure both parameters are non-negative
        if (left<0) left=-left;
        if (right<0) right=-right;
        
        if (left==0) return right;
        
        // both left and right are positive numbers
        if (left<right) std::swap(left, right);
        
        while ( (left%right) != 0) {
            int tmp=left%right;
            left=right;
            right=tmp;
        }
        return right;
    }
    
    void normalize() {
        int x=gdc(dividend_, divisor_);
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
