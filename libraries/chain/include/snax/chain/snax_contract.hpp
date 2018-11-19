/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <snax/chain/types.hpp>
#include <snax/chain/contract_types.hpp>

namespace snax { namespace chain {

   class apply_context;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{
   void apply_snax_newaccount(apply_context&);
   void apply_snax_updateauth(apply_context&);
   void apply_snax_deleteauth(apply_context&);
   void apply_snax_linkauth(apply_context&);
   void apply_snax_unlinkauth(apply_context&);

   /*
   void apply_snax_postrecovery(apply_context&);
   void apply_snax_passrecovery(apply_context&);
   void apply_snax_vetorecovery(apply_context&);
   */

   void apply_snax_setcode(apply_context&);
   void apply_snax_setabi(apply_context&);

   void apply_snax_canceldelay(apply_context&);
   ///@}  end action handlers

} } /// namespace snax::chain
