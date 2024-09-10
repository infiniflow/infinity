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

#ifndef UTILSPP_TYPETRAIT_HPP
#define UTILSPP_TYPETRAIT_HPP

#include "NullType.hpp"

namespace utilspp
{
  template<typename T>
  class TypeTrait
  {
  private:
    template<typename U>
    struct unreference
    {
      typedef U type;
    };

    template<typename U>
    struct unreference<U &>
    {
      typedef U type;
    };

    template<typename U>
    struct unconst
    {
      typedef U type;
    };

    template<typename U>
    struct unconst<const U>
    {
      typedef U type;
    };

  public:
    typedef typename unreference<T>::type NonReference;
    typedef typename unconst<T>::type NonConst;
    typedef typename unconst<typename unreference<T>::type>::type NonParam;
  };

  template<class T>
  struct PointerOnMemberFunction
  {
    typedef utilspp::NullType ClassType;
    typedef utilspp::NullType ReturnType;
    typedef utilspp::NullType Param1Type;
    typedef utilspp::NullType Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef utilspp::NullType ParamList;
  };

  template<typename V, typename W>
  struct PointerOnMemberFunction<W(V::*)()>
  {
    typedef V ClassType;
    typedef W ReturnType;
    
    typedef utilspp::NullType Param1Type;
    typedef utilspp::NullType Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef utilspp::NullType ParamList;
  };

  template<typename V, typename W, typename X>
  struct PointerOnMemberFunction<W(V::*)(X)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X ParamType;
    typedef utilspp::NullType Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_1(X) ParamList;
  };

  template<typename V, typename W, typename X, typename Y>
  struct PointerOnMemberFunction<W(V::*)(X, Y)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X ParamType;
    typedef Y Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_2(X, Y) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X ParamType;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_3(X, Y, Z) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X ParamType;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_4(X, Y, Z, A) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X ParamType;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_5(X, Y, Z, A, B) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_6(X, Y, Z, A, B, C) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_7(X, Y, Z, A, B, C, D) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_8(X, Y, Z, A, B, C, D, E) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E, F)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef F Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_9(X, Y, Z, A, B, C, D, E, F) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E, F, G)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef F Param9Type;
    typedef G Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_10(X, Y, Z, A, B, C, D, E, F, G) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E, F, G, H)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef F Param9Type;
    typedef G Param10Type;
    typedef H Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_11(X, Y, Z, A, B, C, D, E, F, G, H) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E, F, G, H, I)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef F Param9Type;
    typedef G Param10Type;
    typedef H Param11Type;
    typedef I Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_12(X, Y, Z, A, B, C, D, E, F, G, H, I) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E, F, G, H, I, J)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef F Param9Type;
    typedef G Param10Type;
    typedef H Param11Type;
    typedef I Param12Type;
    typedef J Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_13(X, Y, Z, A, B, C, D, E, F, G, H, I, J) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E, F, G, H, I, J, K)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef F Param9Type;
    typedef G Param10Type;
    typedef H Param11Type;
    typedef I Param12Type;
    typedef J Param13Type;
    typedef K Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_14(X, Y, Z, A, B, C, D, E, F, G, H, I, J, K) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L>
  struct PointerOnMemberFunction<W(V::*)(X, Y, Z, A, B, C, D, E, F, G, H, I, J, K, L)>
  {
    typedef V ClassType;
    typedef W ReturnType;
    typedef X Param1Type;
    typedef Y Param2Type;
    typedef Z Param3Type;
    typedef A Param4Type;
    typedef B Param5Type;
    typedef C Param6Type;
    typedef D Param7Type;
    typedef E Param8Type;
    typedef F Param9Type;
    typedef G Param10Type;
    typedef H Param11Type;
    typedef I Param12Type;
    typedef J Param13Type;
    typedef K Param14Type;
    typedef L Param15Type;

    typedef TYPE_LIST_15(X, Y, Z, A, B, C, D, E, F, G, H, I, J, K, L) ParamList;
  };

  template<typename T>
  struct PointerOnFunction
  {
    typedef utilspp::NullType ReturnType;

    typedef utilspp::NullType Param1Type;
    typedef utilspp::NullType Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef utilspp::NullType ParamList;
  };

  template<typename V>
  struct PointerOnFunction<V(*)()>
  {
    typedef V ReturnType;
    typedef utilspp::NullType Param1Type;
    typedef utilspp::NullType Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;
 
    typedef utilspp::NullType ParamList;
 };

  template<typename V, typename W>
  struct PointerOnFunction<V(*)(W)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef utilspp::NullType Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_1(W) ParamList;
  };

  template<typename V, typename W, typename X>
  struct PointerOnFunction<V(*)(W, X)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef utilspp::NullType Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_2(W, X) ParamList;
  };

  template<typename V, typename W, typename X, typename Y>
  struct PointerOnFunction<V(*)(W, X, Y)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef utilspp::NullType Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_3(W, X, Y) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z>
  struct PointerOnFunction<V(*)(W, X, Y, Z)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef utilspp::NullType Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;
    
    typedef TYPE_LIST_4(W, X, Y, Z) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef utilspp::NullType Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_5(W, X, Y, Z, A) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef utilspp::NullType Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_6(W, X, Y, Z, A, B) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef utilspp::NullType Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_7(W, X, Y, Z, A, B, C) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef utilspp::NullType Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_8(W, X, Y, Z, A, B, C, D) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D, E)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef E Param9Type;
    typedef utilspp::NullType Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_9(W, X, Y, Z, A, B, C, D, E) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D, E, F)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef E Param9Type;
    typedef F Param10Type;
    typedef utilspp::NullType Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_10(W, X, Y, Z, A, B, C, D, E, F) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D, E, F, G)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef E Param9Type;
    typedef F Param10Type;
    typedef G Param11Type;
    typedef utilspp::NullType Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_11(W, X, Y, Z, A, B, C, D, E, F, G) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D, E, F, G, H)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef E Param9Type;
    typedef F Param10Type;
    typedef G Param11Type;
    typedef H Param12Type;
    typedef utilspp::NullType Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_12(W, X, Y, Z, A, B, C, D, E, F, G, H) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D, E, F, G, H, I)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef E Param9Type;
    typedef F Param10Type;
    typedef G Param11Type;
    typedef H Param12Type;
    typedef I Param13Type;
    typedef utilspp::NullType Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_13(W, X, Y, Z, A, B, C, D, E, F, G, H, I ) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D, E, F, G, H, I, J)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef E Param9Type;
    typedef F Param10Type;
    typedef G Param11Type;
    typedef H Param12Type;
    typedef I Param13Type;
    typedef J Param14Type;
    typedef utilspp::NullType Param15Type;

    typedef TYPE_LIST_14(W, X, Y, Z, A, B, C, D, E, F, G, H, I, J ) ParamList;
  };

  template<typename V, typename W, typename X, typename Y, typename Z, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K>
  struct PointerOnFunction<V(*)(W, X, Y, Z, A, B, C, D, E, F, G, H, I, J, K)>
  {
    typedef V ReturnType;
    typedef W Param1Type;
    typedef X Param2Type;
    typedef Y Param3Type;
    typedef Z Param4Type;
    typedef A Param5Type;
    typedef B Param6Type;
    typedef C Param7Type;
    typedef D Param8Type;
    typedef E Param9Type;
    typedef F Param10Type;
    typedef G Param11Type;
    typedef H Param12Type;
    typedef I Param13Type;
    typedef J Param14Type;
    typedef K Param15Type;

    typedef TYPE_LIST_15(W, X, Y, Z, A, B, C, D, E, F, G, H, I, J, K) ParamList;
  };

}

#endif

