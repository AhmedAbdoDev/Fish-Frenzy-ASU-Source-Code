////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SFML_VECTOR2_HPP
#define SFML_VECTOR2_HPP

namespace sf
{
    ////////////////////////////////////////////////////////////
    /// \brief Utility template class for manipulating
    ///        2-dimensional vectors
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    class Vector2
    {
    public:
        ////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /// Creates a Vector2(0, 0).
        ///
        ////////////////////////////////////////////////////////////
        Vector2();

        ////////////////////////////////////////////////////////////
        /// \brief Construct the vector from its coordinates
        ///
        /// \param X X coordinate
        /// \param Y Y coordinate
        ///
        ////////////////////////////////////////////////////////////
        Vector2(T X, T Y);

        ////////////////////////////////////////////////////////////
        /// \brief Construct the vector from another type of vector
        ///
        /// This constructor doesn't replace the copy constructor,
        /// it's called only when U != T.
        /// A call to this constructor will fail to compile if U
        /// is not convertible to T.
        ///
        /// \param vector Vector to convert
        ///
        ////////////////////////////////////////////////////////////
        template <typename U>
        explicit Vector2(const Vector2<U> &vector);

        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        T x; ///< X coordinate of the vector
        T y; ///< Y coordinate of the vector
    };

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of unary operator -
    ///
    /// \param right Vector to negate
    ///
    /// \return Memberwise opposite of the vector
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> operator-(const Vector2<T> &right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator +=
    ///
    /// This operator performs a memberwise addition of both vectors,
    /// and assigns the result to \a left.
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a vector)
    ///
    /// \return Reference to \a left
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> &operator+=(Vector2<T> &left, const Vector2<T> &right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator -=
    ///
    /// This operator performs a memberwise subtraction of both vectors,
    /// and assigns the result to \a left.
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a vector)
    ///
    /// \return Reference to \a left
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> &operator-=(Vector2<T> &left, const Vector2<T> &right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator +
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a vector)
    ///
    /// \return Memberwise addition of both vectors
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> operator+(const Vector2<T> &left, const Vector2<T> &right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator -
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a vector)
    ///
    /// \return Memberwise subtraction of both vectors
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> operator-(const Vector2<T> &left, const Vector2<T> &right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator *
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a scalar value)
    ///
    /// \return Memberwise multiplication by \a right
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> operator*(const Vector2<T> &left, T right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator *
    ///
    /// \param left  Left operand (a scalar value)
    /// \param right Right operand (a vector)
    ///
    /// \return Memberwise multiplication by \a left
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> operator*(T left, const Vector2<T> &right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator *=
    ///
    /// This operator performs a memberwise multiplication by \a right,
    /// and assigns the result to \a left.
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a scalar value)
    ///
    /// \return Reference to \a left
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> &operator*=(Vector2<T> &left, T right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator /
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a scalar value)
    ///
    /// \return Memberwise division by \a right
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> operator/(const Vector2<T> &left, T right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator /=
    ///
    /// This operator performs a memberwise division by \a right,
    /// and assigns the result to \a left.
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a scalar value)
    ///
    /// \return Reference to \a left
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    Vector2<T> &operator/=(Vector2<T> &left, T right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator ==
    ///
    /// This operator compares strict equality between two vectors.
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a vector)
    ///
    /// \return True if \a left is equal to \a right
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    bool operator==(const Vector2<T> &left, const Vector2<T> &right);

    ////////////////////////////////////////////////////////////
    /// \relates Vector2
    /// \brief Overload of binary operator !=
    ///
    /// This operator compares strict difference between two vectors.
    ///
    /// \param left  Left operand (a vector)
    /// \param right Right operand (a vector)
    ///
    /// \return True if \a left is not equal to \a right
    ///
    ////////////////////////////////////////////////////////////
    template <typename T>
    bool operator!=(const Vector2<T> &left, const Vector2<T> &right);

#include <SFML/System/Vector2.inl>

    // Define the most common types
    typedef Vector2<int> Vector2i;
    typedef Vector2<unsigned int> Vector2u;
    typedef Vector2<float> Vector2f;
    typedef Vector2<int8_t> Vector2i8;

} // namespace sf

#endif // SFML_VECTOR2_HPP

////////////////////////////////////////////////////////////
/// \class sf::Vector2
/// \ingroup system
///
/// sf::Vector2 is a simple class that defines a mathematical
/// vector with two coordinates (x and y). It can be used to
/// represent anything that has two dimensions: a size, a point,
/// a velocity, etc.
///
/// The template parameter T is the type of the coordinates. It
/// can be any type that supports arithmetic operations (+, -, /, *)
/// and comparisons (==, !=), for example int or float.
///
/// You generally don't have to care about the templated form (sf::Vector2<T>),
/// the most common specializations have special typedefs:
/// \li sf::Vector2<float> is sf::Vector2f
/// \li sf::Vector2<int> is sf::Vector2i
/// \li sf::Vector2<unsigned int> is sf::Vector2u
///
/// The sf::Vector2 class has a small and simple interface, its x and y members
/// can be accessed directly (there are no accessors like setX(), getX()) and it
/// contains no mathematical function like dot product, cross product, length, etc.
///
/// Usage example:
/// \code
/// sf::Vector2f v1(16.5f, 24.f);
/// v1.x = 18.2f;
/// float y = v1.y;
///
/// sf::Vector2f v2 = v1 * 5.f;
/// sf::Vector2f v3;
/// v3 = v1 + v2;
///
/// bool different = (v2 != v3);
/// \endcode
///
/// Note: for 3-dimensional vectors, see sf::Vector3.
///
////////////////////////////////////////////////////////////