//
//  main.cpp
//  Find24
//
//  Created by Hong Tang on 8/12/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#include <iostream>
#include <vector>
#include "find24_simple.hpp"

int main(int argc, char* argv[])
{
    if (argc<3) {
        std::cerr << "Usage: " << argv[0] << " <target> <n1> <n2> ... "
        << std::endl;
        return -1;
    }
    
    int target=atoi(argv[1]);
    if (target<=0) {
        std::cerr << "target must be a positive number" << std::endl;
        return -1;
    }
    
    std::vector<int> elems;
    for (int i=2; i<argc; ++i) {
        int elem=atoi(argv[i]);
        if (elem<=0) {
            std::cerr << "input must be positive number(s)" << std::endl;
            return -1;
        }
        elems.push_back(elem);
    }
    
    auto exprs = find24(target, elems);
    if (exprs.empty()) {
        std::cerr << "Oops, no solution found!" << std::endl;
    } else {
        std::cout << "Found " << exprs.size() << " solutions" << std::endl;
        for (auto& expr : exprs) {
            std::cout << expr << "=" << target << std::endl;
        }
    }
    
    return 0;
}
