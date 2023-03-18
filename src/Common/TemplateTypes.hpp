#pragma once

#include <type_traits>

template <typename T, typename... Args>
concept SameAs = ((std::is_same_v<T, Args>) || ...);

template <typename Base, typename Derived>
concept BaseOf = std::is_base_of_v<Base, Derived>;

template <typename T>
concept Enum = std::is_enum_v<T>;

template <typename T>
concept ScopedEnum = std::conjunction_v<std::is_enum<T>, std::negation<std::is_convertible<T, int>>>;

template <typename T>
concept Unsigned = std::is_unsigned_v<T>;

template <typename T>
concept Signed = std::is_signed_v<T>;

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept Pointer = std::is_pointer_v<T>;

template <typename T>
concept LValueRef = std::is_lvalue_reference_v<T>;