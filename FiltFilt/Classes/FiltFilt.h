//
//  FiltFilt.hpp
//  libfilter
//
//  Created by Nikita Tarkhov on 09.04.2022.
//

#ifndef FiltFilt_hpp
#define FiltFilt_hpp

#if __cplusplus

#include <vector>


class FiltFilt {

public:
    
    typedef std::vector<size_t> vectori;
    typedef std::vector<double> vectord;
    
    struct Parameters {
        std::vector<double> a;
        std::vector<double> b;
    };

public:
    FiltFilt(Parameters parameters);
    ~FiltFilt();

public:
    std::vector<double> process(const std::vector<double> &X);
    
private:
    void addIndexRange(vectori &indices, size_t beg, size_t end, size_t inc = 1);
    void addIndexConst(vectori &indices, size_t value, size_t numel);
    void appendVector(vectord &vec, const vectord &tail);
    vectord subvectorReverse(const vectord &vec, size_t idx_end, size_t idx_start);
    void filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi);
    
private:
    inline size_t maxVal(const vectori& vec)
    {
        return std::max_element(vec.begin(), vec.end())[0];
    }
    
private:
    Parameters parameters;
};

#endif /* __cplusplus */

#endif /* FiltFilt_hpp */
