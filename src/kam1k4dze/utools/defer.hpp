/**
 * @file   defer.hpp
 * @brief  This file provides a macro for deferring the execution of a function
 *         call to the end of the current scope. This is particularly useful
 *         for cleanup code that needs to be executed before the scope ends,
 *         regardless of whether an early return is executed.
 *
 * @date   April 2024
 */

#pragma once

struct defer_dummy {};
template <class F> struct deferrer {
  F f;
  ~deferrer() { f(); }
};
template <class F> deferrer<F> operator*(defer_dummy, F f) { return {f}; }
#define DEFER_(LINE) zz_defer##LINE
#define DEFER(LINE) DEFER_(LINE)
/**
 * @brief Macro to defer a function call to the end of the scope.
 *
 * This macro is used to defer a function call to the end of the scope. This is
 * useful for cleanup code that needs to be executed before the scope ends.
 *
 * @code
 * void foo() {
 *  FILE* file = fopen("file.txt", "r");
 *  if (!file) return;
 *  defer fclose(file);
 *  ...
 * } // fclose(file) is called here
 *
 * @endcode
 *
 * In this example, the `fclose(file);` call is deferred until the end of the
 * current scope. This ensures that the file is properly closed even if an early
 * return is executed.
 *
 */
#define defer auto DEFER(__LINE__) = defer_dummy{} *[&]()
