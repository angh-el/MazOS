#ifndef MY_STDINT_HPP
#define MY_STDINT_HPP

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

// typedef signed long int int32_t;
// typedef unsigned long int uint32_t;

typedef signed  int int32_t;
typedef unsigned  int uint32_t;

typedef signed long long int int64_t;
typedef unsigned long long int uint64_t;

typedef uint8_t boolean;
#define true 1
#define false 0

typedef uint64_t size_t;
typedef signed long ssize_t;
typedef int pid_t;

#define NULL ((void*)0)


#endif