/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <snaxlib/snax.hpp>
#include <snaxlib/token.hpp>
#include <snaxlib/reflect.hpp>
#include <snaxlib/generic_currency.hpp>

#include <bancor/converter.hpp>
#include <currency/currency.hpp>

namespace bancor {
   typedef snax::generic_currency< snax::token<N(other),S(4,OTHER)> >  other_currency;
   typedef snax::generic_currency< snax::token<N(bancor),S(4,RELAY)> > relay_currency;
   typedef snax::generic_currency< snax::token<N(currency),S(4,CUR)> > cur_currency;

   typedef converter<relay_currency, other_currency, cur_currency > example_converter;
} /// bancor

