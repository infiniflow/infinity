/*
 *    Copyright (c) <2002-2009> <Jean-Philippe Barrette-LaPierre>
 *    
 *    Permission is hereby granted, free of charge, to any person obtaining
 *    a copy of this software and associated documentation files 
 *    (curlpp), to deal in the Software without restriction, 
 *    including without limitation the rights to use, copy, modify, merge,
 *    publish, distribute, sublicense, and/or sell copies of the Software,
 *    and to permit persons to whom the Software is furnished to do so, 
 *    subject to the following conditions:
 *    
 *    The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *    
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
 *    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 *    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TYPE_LIST_HPP
#define TYPE_LIST_HPP

#include "NullType.hpp"



#define TYPE_LIST_1(T1) utilspp::tl::TypeList<T1, utilspp::NullType>
#define TYPE_LIST_2(T1, T2 ) ::utilspp::tl::TypeList<T1, TYPE_LIST_1( T2)>
#define TYPE_LIST_3(T1, T2, T3 ) ::utilspp::tl::TypeList<T1, TYPE_LIST_2( T2, T3)>
#define TYPE_LIST_4(T1, T2, T3, T4 ) ::utilspp::tl::TypeList<T1, TYPE_LIST_3( T2, T3, T4)>
#define TYPE_LIST_5(T1, T2, T3, T4, T5)			\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_4(T2, T3, T4, T5)>
#define TYPE_LIST_6(T1, T2, T3, T4, T5, T6)			\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_5(T2, T3, T4, T5, T6)>
#define TYPE_LIST_7(T1, T2, T3, T4, T5, T6, T7)			\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_6(T2, T3, T4, T5, T6, T7)>
#define TYPE_LIST_8(T1, T2, T3, T4, T5, T6, T7, T8)			\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_7(T2, T3, T4, T5, T6, T7, T8)>
#define TYPE_LIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9)		\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_8(T2, T3, T4, T5, T6, T7, T8, T9)>
#define TYPE_LIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)		\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10)>
#define TYPE_LIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)	\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
#define TYPE_LIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)	\
  ::utilspp::tl::TypeList<T1, TYPE_LIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
#define TYPE_LIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
  ::utilspp::tl::TypeList<T1, TYPE_LIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
#define TYPE_LIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) \
  ::utilspp::tl::TypeList<T1, TYPE_LIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
#define TYPE_LIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) \
  ::utilspp::tl::TypeList<T1, TYPE_LIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>


namespace utilspp
{
  namespace tl
  {
    template<class T, class U>
    struct TypeList
    {
      typedef T head;
      typedef U tail;
    };

    //Calculating length of TypeLists
    template<class TList> 
    struct length;

    template<> 
    struct length<NullType>
    {
      enum { value = 0 };
    };

    template<class T, class U>
    struct length<TypeList<T, U> >
    {
      enum { value = 1 + length<U>::value };
    };

    /**
     * Returns the type at a given position (zero-based)
     * in TList. If the index is greather than or equal to 
     * the length of TList, a compile-time error occurs.
     */
    template<class TList, unsigned int index>
    struct TypeAt;

    template<class THead, class TTail>
    struct TypeAt<TypeList<THead, TTail>, 0>
    {
      typedef THead Result;
    };
     
    template<class THead, class TTail, unsigned int i>
    struct TypeAt<TypeList<THead, TTail>, i>
    {
      typedef typename TypeAt<TTail, i - 1>::Result Result;
    };
     
    /**
     * Returns the type at a given position (zero-based)
     * in TList. If the index is greather than or equal to 
     * the length of TList, OutOfBound template class is 
     * returned.
     */
    template<class TList, unsigned int index, class OutOfBound = utilspp::NullType>
    struct TypeAtNonStrict;

    template<class THead, class TTail, class OutOfBound>
    struct TypeAtNonStrict<TypeList<THead, TTail>, 0, OutOfBound>
    {
      typedef THead Result;
    };
     
    template<class THead, class TTail, unsigned int i, class OutOfBound>
    struct TypeAtNonStrict<TypeList<THead, TTail>, i, OutOfBound>
    {
      typedef typename TypeAtNonStrict<TTail, i - 1>::Result Result;
    };

    template<unsigned int i, class OutOfBound>
    struct TypeAtNonStrict<utilspp::NullType, i , OutOfBound>
    {
      typedef OutOfBound Result;
    };


    //Searching TypeLists
    template<class TList, class T>
    struct IndexOf;

    template<class T>
    struct IndexOf<NullType, T>
    {
      enum { value = -1 };
    };

    template<class TTail, class T>
    struct IndexOf<TypeList<T, TTail>, T>
    {
      enum { value = 0 };
    };

    template<class THead, class TTail, class T>
    struct IndexOf<TypeList<THead, TTail>, T>
    {
    private:
      enum { temp = IndexOf<TTail, T>::value };

    public:
      enum { value = temp == -1 ? -1 : 1 + temp };
    };

    //Appending to TypeLists
    template<class TList, class T> 
    struct append;

    template <> 
    struct append<NullType, NullType>
    {
      typedef NullType Result;
    };

    template<class T> 
    struct append<NullType, T>
    {
      typedef TYPE_LIST_1(T) Result;
    };

    template<class THead, class TTail>
    struct append<NullType, TypeList<THead, TTail> >
    {
      typedef TypeList<THead, TTail> Result;
    };

    template <class THead, class TTail, class T>
    struct append<TypeList<THead, TTail>, T>
    {
      typedef TypeList<THead, typename append<TTail, T>::Result>
      Result;
    };

    //Erasing a type from a TypeList
    template<class TList, class T> 
    struct erase;
        
    template<class T>
    struct erase<NullType, T>
    {
      typedef NullType Result;
    };
        
    template<class T, class TTail>
    struct erase<TypeList<T, TTail>, T>
    {
      typedef TTail Result;
    };
        
    template<class THead, class TTail, class T>
    struct erase<TypeList<THead, TTail>, T>
    {
      typedef TypeList<THead, typename erase<TTail, T>::Result>
      Result;
    };
  }
}      


#endif

