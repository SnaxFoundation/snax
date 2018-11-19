#pragma once

#include <snaxlib/snax.hpp>

namespace snax {

   class sudo : public contract {
      public:
         sudo( account_name self ):contract(self){}

         void exec();

   };

} /// namespace snax
