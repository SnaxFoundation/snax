/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <snax/chain/block_header_state.hpp>
#include <snax/chain/block.hpp>
#include <snax/chain/transaction_metadata.hpp>
#include <snax/chain/action_receipt.hpp>

namespace snax { namespace chain {

   struct block_state : public block_header_state {
      explicit block_state( const block_header_state& cur ):block_header_state(cur){}
      block_state( const block_header_state& prev, signed_block_ptr b, bool skip_validate_signee );
      block_state( const block_header_state& prev, block_timestamp_type when );
      block_state() = default;

      /// weak_ptr prev_block_state....
      signed_block_ptr                                    block;
      bool                                                validated = false;
      bool                                                in_current_chain = false;

      /// this data is redundant with the data stored in block, but facilitates
      /// recapturing transactions when we pop a block
      vector<transaction_metadata_ptr>                    trxs;
   };

   using block_state_ptr = std::shared_ptr<block_state>;

} } /// namespace snax::chain

FC_REFLECT_DERIVED( snax::chain::block_state, (snax::chain::block_header_state), (block)(validated)(in_current_chain) )
