//
//  FiltFilt.cpp
//  libfilter
//
//  Created by Nikita Tarkhov on 09.04.2022.
//

#include "FiltFilt.h"
#include "FiltFiltWrapped.h"
#include "Eigen/Dense"

using namespace Eigen;

#pragma mark - Initialization / Deinitialization

FiltFilt::FiltFilt(FiltFilt::Parameters parameters): parameters(parameters) {
    
}

FiltFilt::~FiltFilt() {
    
}

#pragma mark - Public methods

std::vector<double> FiltFilt::process(const std::vector<double> &X) {
    vectord A = parameters.a;
    vectord B = parameters.b;
    
    size_t len = X.size();     // length of input
    size_t na = A.size();
    size_t nb = B.size();
    size_t nfilt = (nb > na) ? nb : na;
    size_t nfact = 3 * (nfilt - 1); // length of edge transients

    if (len <= nfact)
        throw std::domain_error("Input data too short! Data must have length more than 3 times filter order.");

    // set up filter's initial conditions to remove DC offset problems at the
    // beginning and end of the sequence
    B.resize(nfilt, 0);
    A.resize(nfilt, 0);

    vectori rows, cols;
    //rows = [1:nfilt-1           2:nfilt-1             1:nfilt-2];
    addIndexRange(rows, 0, nfilt - 2);
    if (nfilt > 2)
    {
        addIndexRange(rows, 1, nfilt - 2);
        addIndexRange(rows, 0, nfilt - 3);
    }
    //cols = [ones(1,nfilt-1)         2:nfilt-1          2:nfilt-1];
    addIndexConst(cols, 0, nfilt - 1);
    if (nfilt > 2)
    {
        addIndexRange(cols, 1, nfilt - 2);
        addIndexRange(cols, 1, nfilt - 2);
    }
    // data = [1+a(2)         a(3:nfilt)        ones(1,nfilt-2)    -ones(1,nfilt-2)];

    auto klen = rows.size();
    vectord data;
    data.resize(klen);
    data[0] = 1 + A[1];  int j = 1;
    if (nfilt > 2)
    {
        for (int i = 2; i < nfilt; i++)
            data[j++] = A[i];
        for (int i = 0; i < nfilt - 2; i++)
            data[j++] = 1.0;
        for (int i = 0; i < nfilt - 2; i++)
            data[j++] = -1.0;
    }

    vectord leftpad = subvectorReverse(X, nfact, 1);
    double _2x0 = 2 * X[0];
    std::transform(leftpad.begin(), leftpad.end(), leftpad.begin(), [_2x0](double val) {return _2x0 - val; });

    vectord rightpad = subvectorReverse(X, len - 2, len - nfact - 1);
    double _2xl = 2 * X[len - 1];
    std::transform(rightpad.begin(), rightpad.end(), rightpad.begin(), [_2xl](double val) {return _2xl - val; });

    double y0;
    vectord signal1, signal2, zi;

    signal1.reserve(leftpad.size() + X.size() + rightpad.size());
    appendVector(signal1, leftpad);
    appendVector(signal1, X);
    appendVector(signal1, rightpad);

    // Calculate initial conditions
    MatrixXd sp = MatrixXd::Zero(maxVal(rows) + 1, maxVal(cols) + 1);
    for (size_t k = 0; k < klen; ++k)
        sp(rows[k], cols[k]) = data[k];
    auto bb = VectorXd::Map(B.data(), B.size());
    auto aa = VectorXd::Map(A.data(), A.size());
    MatrixXd zzi = (sp.inverse() * (bb.segment(1, nfilt - 1) - (bb(0) * aa.segment(1, nfilt - 1))));
    zi.resize(zzi.size());

    // Do the forward and backward filtering
    y0 = signal1[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val) { return val*y0; });
    filter(B, A, signal1, signal2, zi);
    std::reverse(signal2.begin(), signal2.end());
    y0 = signal2[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val) { return val*y0; });
    filter(B, A, signal2, signal1, zi);

    return subvectorReverse(signal1, signal1.size() - nfact - 1, nfact);
}

#pragma mark - Private methods

void FiltFilt::addIndexRange(vectori &indices, size_t beg, size_t end, size_t inc) {
    for (size_t i = beg; i <= end; i += inc)
        indices.push_back(i);
}

void FiltFilt::addIndexConst(vectori &indices, size_t value, size_t numel) {
    while (numel--)
        indices.push_back(value);
}

void FiltFilt::appendVector(vectord &vec, const vectord &tail) {
    vec.insert(vec.end(), tail.begin(), tail.end());
}

FiltFilt::vectord FiltFilt::subvectorReverse(const vectord &vec, size_t idx_end, size_t idx_start) {
    vectord result(&vec[idx_start], &vec[idx_end+1]);
    std::reverse(result.begin(), result.end());
    return result;
}

void FiltFilt::filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi)
{
    if (A.empty())
        throw std::domain_error("The feedback filter coefficients are empty.");
    if (std::all_of(A.begin(), A.end(), [](double coef){ return coef == 0; }))
        throw std::domain_error("At least one of the feedback filter coefficients has to be non-zero.");
    if (A[0] == 0)
        throw std::domain_error("First feedback coefficient has to be non-zero.");

    // Normalize feedback coefficients if a[0] != 1;
    auto a0 = A[0];
    if (a0 != 1.0)
    {
        std::transform(A.begin(), A.end(), A.begin(), [a0](double v) { return v / a0; });
        std::transform(B.begin(), B.end(), B.begin(), [a0](double v) { return v / a0; });
    }

    size_t input_size = X.size();
    size_t filter_order = std::max(A.size(), B.size());
    B.resize(filter_order, 0);
    A.resize(filter_order, 0);
    Zi.resize(filter_order, 0);
    Y.resize(input_size);

    const double *x = &X[0];
    const double *b = &B[0];
    const double *a = &A[0];
    double *z = &Zi[0];
    double *y = &Y[0];

    for (size_t i = 0; i < input_size; ++i)
    {
        size_t order = filter_order - 1;
        while (order)
        {
            if (i >= order)
                z[order - 1] = b[order] * x[i - order] - a[order] * y[i - order] + z[order];
            --order;
        }
        y[i] = b[0] * x[i] + z[0];
    }
    Zi.resize(filter_order - 1);
}

void filtfilt(const double* a, const double* b, size_t ab_len, const double* input, double* output, size_t lenght) {
    
    FiltFilt::Parameters parameters;
    FiltFilt::vectord inputVector;
    
    parameters.a.assign(a, a + ab_len);
    parameters.b.assign(b, b + ab_len);
    
    inputVector.assign(input, input + lenght);
    
    FiltFilt filter(parameters);
    
    FiltFilt::vectord outputVector = filter.process(inputVector);
    
    for (size_t i = 0; i < lenght; i++) {
        output[i] = outputVector[i];
    }
}
