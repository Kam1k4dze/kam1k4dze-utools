/**
 * \file   utools.hpp
 * \brief  This is the main include file for the utools library. 
 * 
 *         The utools library provides a collection of useful tools and utilities for C++. 
 *         This file includes the headers for all the components of the library, 
 *         allowing users to include the entire library with a single include directive.
 * 
 *         The library currently includes the following components:
 *         - defer: Provides a macro for deferring the execution of a function call to the end of the current scope.
 *         - cstring_obfuscator: Provides a set of tools for obfuscating at compile-time and deobfuscating at runtime C-style strings.
 * 
 * \author Kam1k4dze
 * \date   April 2024
 */
#pragma once
#include <kam1k4dze/utools/defer.hpp>
#include <kam1k4dze/utools/cstring_obfuscator.hpp>
