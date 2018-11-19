#include <snaxlib/snax.hpp>
#include <snaxlib/print.hpp>
using namespace snax;

class payloadless : public snax::contract {
  public:
      using contract::contract;

      void doit() {
         print( "Im a payloadless action" );
      }
};

SNAX_ABI( payloadless, (doit) )
