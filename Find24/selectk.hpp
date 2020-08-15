//
//  selectk.hpp
//  Find24
//
//  Created by Hong Tang on 8/14/20.
//  Copyright © 2020 Hong Tang. All rights reserved.
//

#ifndef selectk_hpp
#define selectk_hpp

template<typename Op>
void selectK(int n, int k, Op op)
{
    int selection[k];
    int index=0;
    selection[0]=-1;
    while (index>=0) {
        ++selection[index];
        int elemNeeded=k-index-1;
        if (selection[index] + elemNeeded >= n) {
            --index;
            continue;
        }
        
        for (int i=index+1; i<k; ++i) {
            selection[i]=selection[i-1]+1;
        }
        index=k-1;
        
        do {
            op(selection, k);
            ++selection[index];
        } while (selection[index] < n);
        --index;
    }
}

#endif /* selectk_hpp */
