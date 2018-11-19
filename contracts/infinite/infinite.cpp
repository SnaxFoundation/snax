/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#include <snaxlib/print.hpp> /// defines transfer struct (abi)

extern "C" {

    /// The apply method just prints forever
    void apply( uint64_t, uint64_t, uint64_t ) {
       int idx = 0;
       while(true) {
          snax::print(idx++);
       }
    }
}
