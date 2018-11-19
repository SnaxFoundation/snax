/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */

#include <snax/chain/chain_id_type.hpp>
#include <snax/chain/exceptions.hpp>

namespace snax { namespace chain {

   void chain_id_type::reflector_verify()const {
      SNAX_ASSERT( *reinterpret_cast<const fc::sha256*>(this) != fc::sha256(), chain_id_type_exception, "chain_id_type cannot be zero" );
   }

} }  // namespace snax::chain

namespace fc {

   void to_variant(const snax::chain::chain_id_type& cid, fc::variant& v) {
      to_variant( static_cast<const fc::sha256&>(cid), v);
   }

   void from_variant(const fc::variant& v, snax::chain::chain_id_type& cid) {
      from_variant( v, static_cast<fc::sha256&>(cid) );
   }

} // fc
