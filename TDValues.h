#pragma once

#include <array>
#include <cstddef>
#include "CHOP_CPlusPlusBase.h"

namespace tekt {
  namespace impl {

    template <typename T>
    struct arity : std::integral_constant<std::size_t, 1> {};

    template<>
    struct arity<Vector> : std::integral_constant<std::size_t, 3> {};

    template<>
    struct arity<Color> : std::integral_constant<std::size_t, 4> {};

    template<typename T, std::size_t I>
    float& tupleField(T& t);
    template<typename T, std::size_t I>
    const float& tupleField(const T& t);

    template<>
    inline float& tupleField<Vector, 0>(Vector & t) { return t.x; }
    template<>
    inline float& tupleField<Vector, 1>(Vector & t) { return t.y; }
    template<>
    inline float& tupleField<Vector, 2>(Vector & t) { return t.z; }

    template<>
    inline const float& tupleField<Vector, 0>(const Vector & t) { return t.x; }
    template<>
    inline const float& tupleField<Vector, 1>(const Vector & t) { return t.y; }
    template<>
    inline const float& tupleField<Vector, 2>(const Vector & t) { return t.z; }

    template<>
    inline float& tupleField<Color, 0>(Color & t) { return t.r; }
    template<>
    inline float& tupleField<Color, 1>(Color & t) { return t.g; }
    template<>
    inline float& tupleField<Color, 2>(Color & t) { return t.b; }
    template<>
    inline float& tupleField<Color, 3>(Color & t) { return t.a; }

    template<>
    inline const float& tupleField<Color, 0>(const Color & t) { return t.r; }
    template<>
    inline const float& tupleField<Color, 1>(const Color & t) { return t.g; }
    template<>
    inline const float& tupleField<Color, 2>(const Color & t) { return t.b; }
    template<>
    inline const float& tupleField<Color, 3>(const Color & t) { return t.a; }

    template<std::size_t N>
    struct arrayMaker {};

    template<>
    struct arrayMaker<2> {
      template<typename T>
      static std::array<T, 2> make(T val) { return { val, val }; }
    };

    template<>
    struct arrayMaker<3> {
      template<typename T>
      static std::array<T, 3> make(T val) { return { val, val, val }; }
    };

    template<>
    struct arrayMaker<4> {
      template<typename T>
      static std::array<T, 4> make(T val) { return { val, val, val, val }; }
    };
  }
}
