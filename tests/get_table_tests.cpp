#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <snax/testing/tester.hpp>
#include <snax/chain/abi_serializer.hpp>
#include <snax/chain/wasm_snax_constraints.hpp>
#include <snax/chain/resource_limits.hpp>
#include <snax/chain/exceptions.hpp>
#include <snax/chain/wast_to_wasm.hpp>
#include <snax/chain_plugin/chain_plugin.hpp>

#include <asserter/asserter.wast.hpp>
#include <asserter/asserter.abi.hpp>

#include <stltest/stltest.wast.hpp>
#include <stltest/stltest.abi.hpp>

#include <snax.system/snax.system.wast.hpp>
#include <snax.system/snax.system.abi.hpp>

#include <snax.token/snax.token.wast.hpp>
#include <snax.token/snax.token.abi.hpp>

#include <fc/io/fstream.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>
#include <fc/io/json.hpp>

#include <array>
#include <utility>

#ifdef NON_VALIDATING_TEST
#define TESTER tester
#else
#define TESTER validating_tester
#endif

using namespace snax;
using namespace snax::chain;
using namespace snax::testing;
using namespace fc;

BOOST_AUTO_TEST_SUITE(get_table_tests)

BOOST_FIXTURE_TEST_CASE( get_scope_test, TESTER ) try {
   produce_blocks(2);

   create_accounts({ N(snax.token), N(snax.ram), N(snax.ramfee), N(snax.stake),
      N(snax.bpay), N(snax.vpay), N(snax.team), N(snax.creator), N(snax.airdrop), N(snax.saving), N(snax.names), N(snax.util) });

   std::vector<account_name> accs{N(inita), N(initb), N(initc), N(initd)};
   create_accounts(accs);
   produce_block();

   set_code( N(snax.token), snax_token_wast );
   set_abi( N(snax.token), snax_token_abi );
   produce_blocks(1);

   // create currency
   auto act = mutable_variant_object()
         ("issuer",       "snax")
         ("maximum_supply", snax::chain::asset::from_string("1000000000.0000 SNAX"));
   push_action(N(snax.token), N(create), N(snax.token), act );

   // issue
   for (account_name a: accs) {
      push_action( N(snax.token), N(issue), "snax", mutable_variant_object()
                  ("to",      name(a) )
                  ("quantity", snax::chain::asset::from_string("999.0000 SNAX") )
                  ("memo", "")
                  );
   }
   produce_blocks(1);

   // iterate over scope
   snax::chain_apis::read_only plugin(*(this->control), fc::microseconds(INT_MAX));
   snax::chain_apis::read_only::get_table_by_scope_params param{N(snax.token), N(accounts), "inita", "", 10};
   snax::chain_apis::read_only::get_table_by_scope_result result = plugin.read_only::get_table_by_scope(param);

   BOOST_REQUIRE_EQUAL(4, result.rows.size());
   BOOST_REQUIRE_EQUAL("", result.more);
   if (result.rows.size() >= 4) {
      BOOST_REQUIRE_EQUAL(name(N(snax.token)), result.rows[0].code);
      BOOST_REQUIRE_EQUAL(name(N(inita)), result.rows[0].scope);
      BOOST_REQUIRE_EQUAL(name(N(accounts)), result.rows[0].table);
      BOOST_REQUIRE_EQUAL(name(N(snax)), result.rows[0].payer);
      BOOST_REQUIRE_EQUAL(1, result.rows[0].count);

      BOOST_REQUIRE_EQUAL(name(N(initb)), result.rows[1].scope);
      BOOST_REQUIRE_EQUAL(name(N(initc)), result.rows[2].scope);
      BOOST_REQUIRE_EQUAL(name(N(initd)), result.rows[3].scope);
   }

   param.lower_bound = "initb";
   param.upper_bound = "initd";
   result = plugin.read_only::get_table_by_scope(param);
   BOOST_REQUIRE_EQUAL(2, result.rows.size());
   BOOST_REQUIRE_EQUAL("", result.more);
   if (result.rows.size() >= 2) {
      BOOST_REQUIRE_EQUAL(name(N(initb)), result.rows[0].scope);
      BOOST_REQUIRE_EQUAL(name(N(initc)), result.rows[1].scope);
   }

   param.limit = 1;
   result = plugin.read_only::get_table_by_scope(param);
   BOOST_REQUIRE_EQUAL(1, result.rows.size());
   BOOST_REQUIRE_EQUAL("initc", result.more);

   param.table = name(0);
   result = plugin.read_only::get_table_by_scope(param);
   BOOST_REQUIRE_EQUAL(1, result.rows.size());
   BOOST_REQUIRE_EQUAL("initc", result.more);

   param.table = N(invalid);
   result = plugin.read_only::get_table_by_scope(param);
   BOOST_REQUIRE_EQUAL(0, result.rows.size());
   BOOST_REQUIRE_EQUAL("", result.more);

} FC_LOG_AND_RETHROW() /// get_scope_test

BOOST_AUTO_TEST_SUITE_END()
