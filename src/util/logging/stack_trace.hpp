#ifndef MAPLE_2A80AFA205F64E6292C8DC660C409359_HPP
#define MAPLE_2A80AFA205F64E6292C8DC660C409359_HPP

#include <Windows.h>

void install_exception_logger();

void log_stack_trace(LogLevel log_level);

void log_stack_trace(LogLevel log_level, CONTEXT &context);

LONG WINAPI exception_logger_hook(PEXCEPTION_POINTERS ex);

#endif//MAPLE_2A80AFA205F64E6292C8DC660C409359_HPP
