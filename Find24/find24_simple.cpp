//
//  find24_simple.cpp
//  Find24
//
//  Created by Hong Tang on 8/13/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#include "find24_simple.hpp"
#include "find24.hpp"

std::vector<std::string> find24(int target, std::vector<int>& elems)
{
    Find24 helper(target, elems);
    helper.run(true);
    return helper.getExpr();
}
