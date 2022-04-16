//
//  FiltFiltWrapped.h
//  FiltFIltSwift
//
//  Created by Nikita Tarkhov on 10.04.2022.
//

#ifndef FiltFiltWrapped_h
#define FiltFiltWrapped_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void filtfilt(const double* a, const double* b, size_t ab_len, const double* input, double* output, size_t lenght);

#ifdef __cplusplus
}
#endif

#endif /* FiltFiltWrapped_h */
