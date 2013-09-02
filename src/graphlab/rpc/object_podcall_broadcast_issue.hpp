/*  
 * Copyright (c) 2009 Carnegie Mellon University. 
 *     All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS
 *  IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *  express or implied.  See the License for the specific language
 *  governing permissions and limitations under the License.
 *
 * For more about this software visit:
 *
 *      http://www.graphlab.ml.cmu.edu
 *
 */


#ifndef OBJECT_PODCALL_BROADCAST_ISSUE_HPP
#define OBJECT_PODCALL_BROADCAST_ISSUE_HPP
#include <iostream>
#include <graphlab/serialization/serialization_includes.hpp>
#include <graphlab/rpc/dc_types.hpp>
#include <graphlab/rpc/dc_internal_types.hpp>
#include <graphlab/rpc/dc_send.hpp>
#include <graphlab/rpc/object_podcall_issue.hpp>
#include <graphlab/rpc/object_podcall_dispatch.hpp>
#include <graphlab/rpc/is_rpc_call.hpp>
#include <graphlab/rpc/pod_template_structs.hpp>
#include <graphlab/rpc/dc_thread_get_send_buffer.hpp>
#include <boost/preprocessor.hpp>
#include <graphlab/rpc/mem_function_arg_types_def.hpp>

namespace graphlab{
namespace dc_impl {

/**
\ingroup rpc
\internal
\file object_podcall_broadcast_issue
 This is an internal function and should not be used directly

Marshalls a object function broadcast to a remote machine. 

*/

#define GENARGS(Z,N,_)  BOOST_PP_CAT(const T, N) BOOST_PP_CAT(&i, N)
#define GENI(Z,N,_) BOOST_PP_CAT(i, N)
#define GENT(Z,N,_) BOOST_PP_CAT(T, N)
#define GENARC(Z,N,_) BOOST_PP_CAT(s.t, N) = BOOST_PP_CAT(i, N);


#define REMOTE_PODCALL_BROADCAST_ISSUE_GENERATOR(Z,N,FNAME_AND_CALL) \
template<typename Iterator, typename T, typename F BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename T)> \
class  BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(2,0,FNAME_AND_CALL), N) { \
  public: \
  static void exec(dc_dist_object_base* rmi, std::vector<dc_send*> sender, unsigned char flags, \
                    Iterator target_begin, Iterator target_end, size_t objid, F remote_function BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM(N,GENARGS ,_) ) {  \
    BOOST_PP_CAT(pod_template_detail::pod_call_struct, N)<F BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, T)> s; \
    dispatch_type2 d = BOOST_PP_CAT(dc_impl::OBJECT_PODCALL_NONINTRUSIVE_DISPATCH,N)<distributed_control,T,F BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM(N, GENT ,_) >;   \
    s.remote_function = remote_function;  \
    s.dispatch_function = reinterpret_cast<size_t>(d);       \
    s.objid = objid;        \
    BOOST_PP_REPEAT(N, GENARC, _)                                       \
    Iterator iter = target_begin;                                       \
    while(iter != target_end) {                                         \
      ASSERT_LT((*iter), sender.size());                                 \
      sender[(*iter)]->copy_and_send_data((*iter),flags | POD_CALL , reinterpret_cast<char*>(&s), sizeof(s));    \
      if ((flags & CONTROL_PACKET) == 0)                                  \
        rmi->inc_bytes_sent((*iter), sizeof(s));                  \
      ++iter;                                                             \
    }                                                                   \
  }                                                                     \
}; 



/**
Generates a function call issue. 3rd argument is a tuple (issue name, dispacther name)
*/
BOOST_PP_REPEAT(7, REMOTE_PODCALL_BROADCAST_ISSUE_GENERATOR,  (object_podcall_broadcast_issue, _) )



#undef GENARC
#undef GENT
#undef GENI
#undef GENARGS
#undef REMOTE_PODCALL_BROADCAST_ISSUE_GENERATOR

} // namespace dc_impl
} // namespace graphlab

#include <graphlab/rpc/mem_function_arg_types_undef.hpp>

#endif

