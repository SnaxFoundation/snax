/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */

#include <snaxlib/snax.hpp>

namespace asserter {
   struct assertdef {
      int8_t      condition;
      std::string message;

      SNAXLIB_SERIALIZE( assertdef, (condition)(message) )
   };
}
