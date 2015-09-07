#ifndef __dbg_h__
#define __dbg_h__



// Debug tag
#define DTAG "OpenMaka::Native"

#ifdef __ANDROID__

#include <android/log.h>
#define log(D, M, ...) __android_log_print(ANDROID_LOG_INFO, D, "[INFO] " M "", ##__VA_ARGS__)

#else

#include <iostream>
#define log(value) std::cout << value << std::endl;

#endif // __ANDROID__
#endif // _dbg_h_
