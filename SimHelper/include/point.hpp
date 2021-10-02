#ifndef POINT_HPP
#define POINT_HPP

#include "point.h"
#include <cmath>
#include <ostream>

namespace Helper
{
    template <typename T>
    inline Point2<T>::Point2(): x(0), y(0){}
    
    template <typename T>
    inline Point2<T>::Point2(T _x, T _y): x(_x), y(_y){}

    template <typename T>
    inline Point2<T>::Point2(const Point2& pt): x(pt.x), y(pt.y){}

    template <typename T>
    inline auto Point2<T>::calcNormSqr() const
    {
        return x*x + y*y;
    }

    template <typename T>
    inline auto Point2<T>::calcNorm() const
    {
        return std::sqrt(x*x + y*y);
    }    

    template <typename T>
    inline Point2<T>& Point2<T>::operator=(const Point2& p)
    {
        x = p.x;
        y = p.y;

        return *this;
    }

    template <typename T>
    static inline bool operator==(const Point2<T>& p1, const Point2<T>& p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }

    template <typename T>
    static inline bool operator!=(const Point2<T>& p1, const Point2<T>& p2)
    {
        return p1.x != p2.x && p1.y != p2.y;
    }

    template <typename T>
    static inline Point2<T> operator*=(const Point2<T>& p1, const Point2<T>& p2)
    {
        p1.x *= p2.x;
        p1.y *= p2.y;
        return p1;
    }

    template <typename T>
    static inline Point2<T> operator+=(const Point2<T>& p1, const Point2<T>& p2)
    {
        p1.x += p2.x;
        p1.y += p2.y;
        return p1;
    }

    template <typename T>
    static inline Point2<T> operator+(const Point2<T>& p1, const Point2<T>& p2)
    {
        return Point2<T>(static_cast<T>(p1.x + p2.x), static_cast<T>(p1.y + p2.y));
    }

    template <typename T>
    static inline Point2<T> operator-(const Point2<T>& p1, const Point2<T>& p2)
    {
        return Point2<T>(static_cast<T>(p1.x - p2.x), static_cast<T>(p1.y - p2.y));
    }

    template <typename T>
    static inline Point2<T> operator-(const Point2<T>& p1)
    {
        return Point2<T>(static_cast<T>(-p1.x), static_cast<T>(-p1.y));
    }

    template <typename T>
    static inline Point2<T> operator/(const Point2<T>& p1, double p2)
    {
        return Point2<T>(static_cast<T>(p1.x/p2), static_cast<T>(p1.y/p2));
    }

    template <typename T>
    static inline Point2<T> operator*(const Point2<T>& p1, double p2)
    {
        return Point2<T>(static_cast<T>(p1.x * p2), static_cast<T>(p1.y * p2));
    }

    template <typename T>
    inline Point3<T>::Point3(): x(0), y(0), z(0){}
    
    template <typename T>
    inline Point3<T>::Point3(T _x, T _y, T _z): x(_x), y(_y), z(_z){}

    template <typename T>
    inline Point3<T>::Point3(const Point3& pt): x(pt.x), y(pt.y), z(pt.z){}

    template <typename T>
    inline auto Point3<T>::calcNormSqr() const
    {
        return x*x + y*y + z*z;
    }

    template <typename T>
    inline auto Point3<T>::calcNorm() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }    

    template<typename T>
    inline auto Point3<T>::dot(const Point3<T>& p)
    {
        return x*p.x + y*p.y + z*p.z;
    }
    template <typename T>
    inline Point3<T>& Point3<T>::operator=(const Point3& p)
    {
        x = p.x;
        y = p.y;
        z = p.z;

        return *this;
    }

    template <typename T>
    inline std::ostream& operator<<(std::ostream& os, const Point3<T>&p)
    {
        return os << "X: " << std::to_string(p.x) << " Y: " << std::to_string(p.y) << " Z: " << std::to_string(p.z);
    }
    template <typename T>
    static inline bool operator==(const Point3<T>& p1, const Point3<T>& p2)
    {
        return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
    }

    template <typename T>
    static inline bool operator!=(const Point3<T>& p1, const Point3<T>& p2)
    {
        return p1.x != p2.x && p1.y != p2.y && p1.z != p2.z;
    }

    template <typename T>
    static inline Point3<T> operator*=(Point3<T>& p1, const Point3<T>& p2)
    {
        p1.x *= p2.x;
        p1.y *= p2.y;
        p1.z *= p2.z;
        return p1;
    }

    template <typename T>
    inline Point3<T> operator+=(Point3<T>& p1, const Point3<T>& p2)
    {
        p1.x += p2.x;
        p1.y += p2.y;
        p1.z += p2.z;

        return p1;
    }

    template <typename T>
    inline Point3<T> operator+(const Point3<T>& p1, const Point3<T>& p2)
    {
        return Point3<T>(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
    }

    template <typename T>
    inline Point3<T> operator-(const Point3<T>& p1, const Point3<T>& p2)
    {
        return Point3<T>(static_cast<T>(p1.x - p2.x), static_cast<T>(p1.y - p2.y), static_cast<T>(p1.z - p2.z));
    }

    template <typename T>
    inline Point3<T> operator-(const Point3<T>& p1)
    {
        return Point3<T>(static_cast<T>(-p1.x), static_cast<T>(-p1.y), static_cast<T>(-p1.z));
    }

    template <typename T>
    static inline Point3<T> operator/(const Point3<T>& p1, double p2)
    {
        return Point3<T>(static_cast<T>(p1.x/p2), static_cast<T>(p1.y/p2), static_cast<T>(p1.z/p2));
    }

    template <typename T>
    static inline Point3<T> operator*(const Point3<T>& p1, double p2)
    {
        return Point3<T>(static_cast<T>(p1.x * p2), static_cast<T>(p1.y * p2), static_cast<T>(p1.z * p2));
    }

    template <typename T>
    static inline Point3<T> operator*(double p2, const Point3<T>& p1)
    {
        return Point3<T>(static_cast<T>(p1.x * p2), static_cast<T>(p1.y * p2), static_cast<T>(p1.z * p2));
    }
}

#endif