/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */

#include <snaxlib/snax.hpp>

namespace snax {

   class noop: public contract {
      public:
         noop( account_name self ): contract( self ) { }
         void anyaction( account_name from,
                         const std::string& /*type*/,
                         const std::string& /*data*/ )
         {
            require_auth( from );
         }
   };

   SNAX_ABI( noop, ( anyaction ) )

} /// snax     
