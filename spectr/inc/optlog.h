/**
 * @file optlog.h
 * @author  Sergey Simonov
 * @brief   Simple stream (<<) looger with filter by verbose and message level
 *
 */
#pragma once
#include <iostream>

namespace optlog {

    // Compile time logger 
    template<std::ostream* stream, int verbLevel, int MsgLevel>
    struct ConstOptLog {
        template <typename ValType>
        const ConstOptLog& operator<<(const ValType& val) const {
            if constexpr (verbLevel >= MsgLevel) {
                (*stream) << val;
            }
            return *this;
        }
        // to support << std::endl
        const ConstOptLog& operator<<([[maybe_unused]] std::basic_ostream<char>& (*manip)(std::basic_ostream<char>&)) const {
            if  constexpr (verbLevel >= MsgLevel) {
                return operator<< <>(manip);
            }
            else {
                return *this;
            }
        }
    };


    // Run time logger 
    class OptLog {
    private:
        std::ostream& stream;
        const int verbLevel;
        const int msgLevel;
    public:
        OptLog(std::ostream& stream, int verbLevel, int msgLevel) :
            stream(stream), verbLevel(verbLevel), msgLevel(msgLevel) {
        };

        template <typename ValType>
        const OptLog& operator<<(const ValType& val) const {
            if (verbLevel >= msgLevel) {
                stream << val;
            }
            return *this;
        }
        // to support << std::endl
        const OptLog& operator<<(std::basic_ostream<char>& (*manip)(std::basic_ostream<char>&)) const {
            if (verbLevel >= msgLevel) {
                return operator<< <>(manip);
            }
            else {
                return *this;
            }
        }
    };

} // namespace optlog


// Default value, if it wasn't setup in build system
#ifndef VERBOSE_LEVEL
    #define VERBOSE_LEVEL 3
#endif

// Any case ouput
[[maybe_unused]] static optlog::ConstOptLog<&std::cout, VERBOSE_LEVEL, 0> log0;

// Output only when VERBOSE_LEVEL>0
[[maybe_unused]] static optlog::ConstOptLog<&std::cout, VERBOSE_LEVEL, 1> log1;
