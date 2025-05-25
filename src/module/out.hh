#pragma once

#include <iostream>
#include <format>
#include <sstream>
#include <math.h>

static constexpr const char* ANSI_RESET = "\x1b[0m";

static constexpr const char* ANSI_FG_RED = "\x1b[31m";
static constexpr const char* ANSI_FG_GREEN = "\x1b[32m";
static constexpr const char* ANSI_FG_YELLOW = "\x1b[33m";
static constexpr const char* ANSI_FG_MAGENTA = "\x1b[35m";

#define MP_INFO(str) \
        {std::cout << ANSI_FG_GREEN << str << ANSI_RESET << std::endl; \
        std::cout.flush();}

#define MP_DEBUG_OUT(str) \
        {std::cout << ANSI_FG_MAGENTA \
                   << "[DEBUG] " \
                   << str \
                   << ANSI_RESET << std::endl; \
        std::cout.flush();}

#define MP_DEBUG_VAR(var) \
        {MP_DEBUG_OUT(#var << " = " << var)}

#define MP_WARNING(str) \
        {std::cout << ANSI_FG_YELLOW \
                  << "[WARNING] " \
                  << str \
                  << ANSI_RESET \
                  << " (in function " << __FUNCTION__ << ":" << __LINE__ \
                  << " in file " << __FILE__ << ")" \
                  << std::endl; \
        std::cout.flush();}

#define MP_ERROR(str) \
        std::cout << ANSI_FG_RED \
                  << "[ERROR] " \
                  << str \
                  << ANSI_RESET \
                  << " (in function " << __FUNCTION__ << ":" << __LINE__ \
                  << " in file " << __FILE__ << ")" \
                  << std::endl

#define MP_ERROR_throw(st) \
        {MP_ERROR(st); \
        std::stringstream str_strm; \
        str_strm << "[ERROR] " << st; \
        throw std::runtime_error(str_strm.str());}

// Assertions

#define MP_ASSERT(exp) \
        {if(!(exp)) MP_ERROR_throw("Assertion failed: " << (#exp));}

#define MP_ASSERT_EQ(a, b) \
        {if((a) != (b)) MP_ERROR_throw("Assertion failed: " << (a) << " == " << (b));}

#define MP_ASSERT_NEQ(a, b) \
        {if((a) == (b)) MP_ERROR_throw("Assertion failed: " << (a) << " != " << (b));}

#define MP_ASSERT_G(a, b) \
        {if((a) <= (b)) MP_ERROR_throw("Assertion failed: " << (a) << " > " << (b));}

#define MP_ASSERT_GEQ(a, b) \
        {if((a) < (b)) MP_ERROR_throw("Assertion failed: " << (a) << " >= " << (b));}

#define MP_ASSERT_L(a, b) \
        {if((a) >= (b)) MP_ERROR_throw("Assertion failed: " << (a) << " < " << (b));}

#define MP_ASSERT_LEQ(a, b) \
        {if((a) > (b)) MP_ERROR_throw("Assertion failed: " << (a) << " <= " << (b));}

#define MP_ASSERT_FINITE(a) \
        {MP_ASSERT(isfinite(a));}
