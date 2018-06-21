//**********************************************************************
// FILE:       assert_at_compile_time.h
//
// PURPOSE:    Define a macro used to throw an assertion at compile-time
//
// AUTHOR:     Andrew Dalgleish 1998/08/26
//
// Copyright (c) 1998 Axon Instruments, Inc.
// All rights reserved.



// Only include this file once
#ifndef  INC_assert_at_compile_time_h
#define  INC_assert_at_compile_time_h

//----------------------------------------------------------------------
// Define a macro which can assert at compile time
#ifndef  ASSERT_AT_COMPILE_TIME
#ifdef _DEBUG
// Debug version:
//
// Declare a char array variable of size 1 if expr is true, or size 0 (which is illegal) if expr is false.
//
// Note that the expression must be able to be evaluated at COMPILE time, for example:
// (sizeof(foo) > sizeof(bar))   is ok
// (sizeof(foo) > 42)            is ok
// (sizeof(foo) > bar)           is NOT ok (unless bar is a literal constant).
//
// The extra macro definitions are used to expand the __LINE__ so each variable declaration has a unique name.
#define  ASSERT_AT_COMPILE_TIME(expr)         ASSERT_AT_COMPILE_TIME1(expr, __LINE__)
#define  ASSERT_AT_COMPILE_TIME1(expr, line)  ASSERT_AT_COMPILE_TIME2(expr, line)
#define  ASSERT_AT_COMPILE_TIME2(expr, line)  static char const ASSERT_AT_COMPILE_TIME_ ## line [(expr) ? 1 : 0]
#else    // _DEBUG
// Release version:
//
// Declare a char array variable of size 1
// This is so we do not end up with a floating semi-colon, which may be illegal (depending on the context).
//
// Hopefully the optimizer will remove the unused variable.
// If not, we waste one byte.
#define  ASSERT_AT_COMPILE_TIME(expr)         ASSERT_AT_COMPILE_TIME1(expr, __LINE__)
#define  ASSERT_AT_COMPILE_TIME1(expr, line)  ASSERT_AT_COMPILE_TIME2(expr, line)
#define  ASSERT_AT_COMPILE_TIME2(expr, line)  static char const ASSERT_AT_COMPILE_TIME_ ## line [1]
#endif   // _DEBUG
#endif   // ASSERT_AT_COMPILE_TIME

#endif   // INC_assert_at_compile_time_h

