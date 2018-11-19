#include <snaxlib/snax.hpp>

using namespace snax;

class hello : public snax::contract {
  public:
      using contract::contract;

      /// @abi action 
      void hi( account_name user ) {
         print( "Hello, ", name{user} );
      }
};

SNAX_ABI( hello, (hi) )
