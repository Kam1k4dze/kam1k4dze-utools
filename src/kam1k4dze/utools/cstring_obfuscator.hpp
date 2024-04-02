/**
 * @file   cstring_obfuscator.hpp
 * @brief  This file provides a set of tools for obfuscating at compile-time and
 *         deobfuscating at runtime C-strings. This is useful for preventing sensitive
 *         strings from being easily extracted from binary files.
 *
 *         The file defines a set of macros and a crypt namespace containing
 *         the Xor_string class template for encrypting and decrypting strings.
 *         The encryption uses a simple XOR cipher with a compile-time generated
 *         key.
 *
 *         The macros XorString, _c, XorWS, XorWideString, and _cw provide
 *         convenient ways to create encrypted strings and decrypt them at runtime.
 *@note define TBX_XSTR_SEED before including this file to change the seed value
 * @date   April 2024
 */
#pragma once


namespace crypt {
// =============================================================================


#ifndef TBX_XSTR_SEED
/**
 * @brief Seed for the random number generator for string encryption.
 *
 * If the user does not define TBX_XSTR_SEED before including this file,
 * the default seed value will be 3421.
 */
#define TBX_XSTR_SEED (3421ull)
#endif

// -----------------------------------------------------------------------------

// @return a pseudo random number clamped at 0xFFFFFFFF
constexpr unsigned long long linear_congruent_generator(unsigned rounds) {
  return 1013904223ull +
         (1664525ull * ((rounds > 0) ? linear_congruent_generator(rounds - 1)
                                     : (TBX_XSTR_SEED))) %
             0xFFFFFFFF;
}

// -----------------------------------------------------------------------------

#define Random() linear_congruent_generator(10)
#define XSTR_RANDOM_NUMBER(Min, Max) (Min + (Random() % (Max - Min + 1)))

// -----------------------------------------------------------------------------

constexpr const unsigned long long XORKEY = XSTR_RANDOM_NUMBER(0, 0xFF);

// -----------------------------------------------------------------------------

template <typename Char>
constexpr Char encrypt_character(const Char character, int index) {
  return character ^ (static_cast<Char>(XORKEY) + index);
}

// -----------------------------------------------------------------------------

template <unsigned size, typename Char> class Xor_string {
public:
  const unsigned _nb_chars = (size - 1);
  Char _string[size];

  // if every goes alright this constructor should be executed at compile time
  inline constexpr Xor_string(const Char *string) : _string{} {
    for (unsigned i = 0u; i < size; ++i)
      _string[i] = encrypt_character<Char>(string[i], i);
  }

  // This is executed at runtime.
  // HACK: although decrypt() is const we modify '_string' in place
  const Char *decrypt() const {
    Char *string = const_cast<Char *>(_string);
    for (unsigned t = 0; t < _nb_chars; t++) {
      string[t] = string[t] ^ (static_cast<Char>(XORKEY) + t);
    }
    string[_nb_chars] = '\0';
    return string;
  }
};

} // namespace crypt


/**
 * @brief Creates a named compile-time encrypted C-string.
 *
 * This macro creates a constexpr instance of the crypt::Xor_string class with
 * the given name and C-string. The C-string is encrypted at compile-time
 * and can be decrypted at runtime by calling the decrypt method on the created
 * object.
 *
 * @param name The variable name for the crypt::Xor_string instance.
 * @param my_string The C-string to be encrypted.
 *
 * @code
 * XorS(my_encrypted_string, L"Hello, World!");
 * std::cout << my_encrypted_string.decrypt() << std::endl; // Outputs: Hello, World!
 * @endcode
 *
 * @note The C-string must be a string literal.
 * @note The C-string must be null-terminated.
 */
#define XorS(name, my_string)                                                  \
  constexpr crypt::Xor_string<(sizeof(my_string) / sizeof(char)), char> name(  \
      my_string)

// Because of a limitation/bug in msvc 2017 we need to declare
// crypt::Xor_string() as a constexpr otherwise the constructor is not evaluated
// at compile time. The lambda function is here to allow this declaration inside
// the macro because there is no such thing as casting to 'constexpr' (and
// casting to const does not solve this bug).

/**
 * @brief Creates an anonymous compile-time encrypted C-string and decrypts it
 * at runtime.
 *
 * This macro creates an anonymous constexpr instance of the crypt::Xor_string
 * class with the given C-string, and immediately calls its decrypt method. This
 * means that the C-string is encrypted at compile-time and decrypted at
 * runtime.
 *
 * This is useful to avoid C-strings being stored in binary files as plain text.
 *
 * @param my_string The C-string to be encrypted.
 *
 * @code
 * std::cout << XorString("Hello, World!") << std::endl; // Outputs: Hello,
 * World!
 * @endcode
 *
 * @note The C-string must be a string literal.
 * @note The C-string must be null-terminated.
 */
#define XorString(my_string)                                                   \
  [] {                                                                         \
    constexpr crypt::Xor_string<(sizeof(my_string) / sizeof(char)), char>      \
        expr(my_string);                                                       \
    return expr;                                                               \
  }()                                                                          \
      .decrypt()


/**
 * @brief Alias for XorString macro.
 *
 * This macro is an alias for the XorString macro, which creates an
 * anonymous compile-time encrypted C-string and decrypts it at runtime.
 *
 * @param string The C-string to be encrypted.
 *
 * @see XorString
 *
 * @code
 * std::cout << _c("Hello, World!") << std::endl; // Outputs: Hello, World!
 * @endcode
 *
 * @note The C-string must be a string literal.
 * @note The C-string must be null-terminated.
 */
#define _c(string) XorString(string)


/**
 * @brief Creates a named compile-time encrypted wide C-string.
 *
 * This macro creates a constexpr instance of the crypt::Xor_string class with
 * the given name and wide C-string. The C-string is encrypted at compile-time
 * and can be decrypted at runtime by calling the decrypt method on the created
 * object.
 *
 * @param name The variable name for the crypt::Xor_string instance.
 * @param my_string The wide C-string to be encrypted.
 *
 * @code
 * XorWS(my_encrypted_string, L"Hello, World!");
 * std::wcout << my_encrypted_string.decrypt() << std::endl; // Outputs: Hello,
 * World!
 * @endcode
 *
 * @note The C-string must be a wide string literal.
 * @note The C-string must be null-terminated.
 */
#define XorWS(name, my_string)                                                 \
  constexpr crypt::Xor_string<(sizeof(my_string) / sizeof(wchar_t)), wchar_t>  \
  name(my_string)

/**
 * @brief Creates an anonymous compile-time encrypted wide C-string and decrypts
 * it at runtime.
 *
 * This macro creates an anonymous constexpr instance of the crypt::Xor_string
 * class with the given wide C-string, and immediately calls its decrypt method.
 * This means that the wide C-string is encrypted at compile-time and decrypted
 * at runtime.
 *
 * This is useful to avoid wide C-strings being stored in binary files as plain
 * text.
 *
 * @param my_string The wide C-string to be encrypted.
 *
 * @code
 * std::wcout << XorWideString(L"Hello, World!") << std::endl; // Outputs:
 * Hello, World!
 * @endcode
 *
 * @note The C-string must be a wide string literal.
 * @note The C-string must be null-terminated.
 */
#define XorWideString(my_string)                                               \
  [] {                                                                         \
    constexpr crypt::Xor_string<(sizeof(my_string) / sizeof(wchar_t)),         \
                                wchar_t>                                       \
        expr(my_string);                                                       \
    return expr;                                                               \
  }()                                                                          \
      .decrypt()


/**
 * @brief Alias for XorWideString macro.
 *
 * This macro is an alias for the XorWideString macro, which creates an
 * anonymous compile-time encrypted wide C-string and decrypts it at runtime.
 *
 * @param string The wide C-string to be encrypted.
 *
 * @see XorWideString
 *
 * @code
 * std::wcout << _cw(L"Hello, World!") << std::endl; // Outputs: Hello, World!
 * @endcode
 *
 * @note The C-string must be a wide string literal.
 * @note The C-string must be null-terminated.
 */
#define _cw(string) XorWideString(string)


