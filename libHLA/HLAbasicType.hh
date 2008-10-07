// ----------------------------------------------------------------------------
// HLAbasicType.hh - IEEE 1516.2 compliant datatypes
// Copyright (C) 2008  Petr Gotthard <petr.gotthard@centrum.cz>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1, as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// $Id: HLAbasicType.hh,v 1.1 2008/08/02 14:03:14 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_BASICTYPE_HH
#define _HLATYPES_BASICTYPE_HH

#include <HLAbuffer.hh>

namespace libhla {

/* HLAbasicType<DATATYPE, STORAGE, ENCODING>
 * defines a user-convenient DATATYPE, stored in STORAGE using given ENCODING.
 *
 * The data are stored in a buffer of sizeof(STORAGE).
 *
 * The buffer is casted to a DATATYPE that provide data access operators. The
 * data can be accessed in an usual way.
 * The DATATYPE may have any sizeof(), but must have static-cast to STORAGE.
 *
 * For example:
 * typedef HLAbasicType<long, uint32_t, LittleEndian> HLAinteger32BE;
 * HLAdata<HLAinteger32BE> value;
 *
 * value = 42;
 */

//! Swap <i> bytes of the <T> type
/*! Template specializations are defined for each possible <i>.
 */
template<class T, int i = sizeof(T)>
struct __swap;

//! Conversion to the Little Endian encoding
template<class T>
struct LittleEndian
{
    inline const T operator()(const T& x) const {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        return x;
#elif __BYTE_ORDER == __BIG_ENDIAN
        return __swap<T>()( x );
#else
#error Undefined __BYTE_ORDER
#endif
    }
};

//! Conversion to the Big Endian encoding
template<class T>
struct BigEndian
{
    inline const T operator()(const T& x) const {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        return __swap<T>()( x );
#elif __BYTE_ORDER == __BIG_ENDIAN
        return x;
#endif
    }
};

template<class T>
struct __swap<T,1>
{
    inline const T operator()(const T& x) const {
        return x;
    }
};

template<class T>
struct __swap<T,2>
{
    inline const T operator()(const T& x) const {
        union {
            uint16_t u16;
            T x;
        } result;
        result.u16 =
            (*(uint16_t*)&x<<8 | *(uint16_t*)&x>>8);
        return result.x;
    }
};

template<class T>
struct __swap<T,4>
{
    inline const T operator()(const T& x) const {
        union {
            uint32_t u32;
            T x;
        } result;
        result.u32 =
            (*(uint32_t*)&x<<24 | *(uint32_t*)&x>>24 |
            (*(uint32_t*)&x & 0x0000ff00UL)<<8 |
            (*(uint32_t*)&x & 0x00ff0000UL)>>8);
        return result.x;
    }
};

template<class T>
struct __swap<T,8>
{
    inline const T operator()(const T& x) const {
        union {
            uint64_t u64;
            T x;
        } result;
        result.u64 =
            (*(uint64_t*)&x<<56 | *(uint64_t*)&x>>56 |
            (*(uint64_t*)&x & 0x000000000000ff00ULL)<<40 |
            (*(uint64_t*)&x & 0x0000000000ff0000ULL)<<24 |
            (*(uint64_t*)&x & 0x00000000ff000000ULL)<< 8 |
            (*(uint64_t*)&x & 0x000000ff00000000ULL)>> 8 |
            (*(uint64_t*)&x & 0x0000ff0000000000ULL)>>24 |
            (*(uint64_t*)&x & 0x00ff000000000000ULL)>>40);
        return result.x;
    }
};

//! HLA basic type, represented by <T> type stored in <S> using <E> encoding.
template<class T, class S, template<class T>class E>
struct HLAbasicType
{
    HLAbasicType& operator = (const T& data)
    {
#ifndef NDEBUG
        __HLAbuffer::__check_memory(this, __sizeof());
#endif
        *(S*)this = E<S>()(data);
        return *this;
    }

    operator T() const
    {
#ifndef NDEBUG
        __HLAbuffer::__check_memory(this, __sizeof());
#endif
        return E<S>()(*(S*)this);
    }

    static const size_t emptysizeof()
    { return __sizeof(); }

    static const size_t __sizeof()
    { return sizeof(S); }

    void copy(void* source)
    {
#ifndef NDEBUG
        __HLAbuffer::__check_memory(this, __sizeof());
#endif
        memcpy((char*)this, source, __sizeof());
    }

    static const size_t m_octetBoundary = sizeof(S);
    static const bool m_isVariable = false;
};

template<class T, class S, template<class T>class E>
std::ostream& PrintBuffer(std::ostream& stream, HLAbasicType<T,S,E>& buffer)
{ return __print_buffer(stream, (void*)&buffer, buffer.__sizeof()); }

/* IEEE 1516.2, Table 23:
 * Basic data representation table
 */
typedef HLAbasicType<short, int16_t, BigEndian> HLAinteger16BE;
typedef HLAbasicType<long, int32_t, BigEndian> HLAinteger32BE;
typedef HLAbasicType<long long, int64_t, BigEndian> HLAinteger64BE;
typedef HLAbasicType<float, float, BigEndian> HLAfloat32BE;
typedef HLAbasicType<double, double, BigEndian> HLAfloat64BE;
typedef HLAbasicType<wchar_t, wchar_t, BigEndian> HLAoctetPairBE;

typedef HLAbasicType<short, int16_t, LittleEndian> HLAinteger16LE;
typedef HLAbasicType<long, int32_t, LittleEndian> HLAinteger32LE;
typedef HLAbasicType<long long, int64_t, LittleEndian> HLAinteger64LE;
typedef HLAbasicType<float, float, LittleEndian> HLAfloat32LE;
typedef HLAbasicType<double, double, LittleEndian> HLAfloat64LE;
typedef HLAbasicType<wchar_t, wchar_t, LittleEndian> HLAoctetPairLE;

typedef HLAbasicType<char, char, BigEndian> HLAoctet;

/* Additional datatypes used by RPR-FOM
 */
typedef HLAbasicType<unsigned short, uint16_t, BigEndian> Unsignedinteger16BE;
typedef HLAbasicType<unsigned long, uint32_t, BigEndian> Unsignedinteger32BE;
typedef HLAbasicType<unsigned long, uint64_t, BigEndian> Unsignedinteger64BE;

} // namespace libhla

#endif // _HLATYPES_BASICTYPE_HH

// $Id: HLAbasicType.hh,v 1.1 2008/08/02 14:03:14 gotthardp Exp $
