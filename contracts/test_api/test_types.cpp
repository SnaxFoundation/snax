/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#include <snaxlib/snax.hpp>

#include "test_api.hpp"

void test_types::types_size() {

   snax_assert( sizeof(int64_t) == 8, "int64_t size != 8");
   snax_assert( sizeof(uint64_t) ==  8, "uint64_t size != 8");
   snax_assert( sizeof(uint32_t) ==  4, "uint32_t size != 4");
   snax_assert( sizeof(int32_t) ==  4, "int32_t size != 4");
   snax_assert( sizeof(uint128_t) == 16, "uint128_t size != 16");
   snax_assert( sizeof(int128_t) == 16, "int128_t size != 16");
   snax_assert( sizeof(uint8_t) ==  1, "uint8_t size != 1");

   snax_assert( sizeof(account_name) ==  8, "account_name size !=  8");
   snax_assert( sizeof(table_name) ==  8, "table_name size !=  8");
   snax_assert( sizeof(time) ==  4, "time size !=  4");
   snax_assert( sizeof(snax::key256) == 32, "key256 size != 32" );
}

void test_types::char_to_symbol() {

   snax_assert( snax::char_to_symbol('1') ==  1, "snax::char_to_symbol('1') !=  1");
   snax_assert( snax::char_to_symbol('2') ==  2, "snax::char_to_symbol('2') !=  2");
   snax_assert( snax::char_to_symbol('3') ==  3, "snax::char_to_symbol('3') !=  3");
   snax_assert( snax::char_to_symbol('4') ==  4, "snax::char_to_symbol('4') !=  4");
   snax_assert( snax::char_to_symbol('5') ==  5, "snax::char_to_symbol('5') !=  5");
   snax_assert( snax::char_to_symbol('a') ==  6, "snax::char_to_symbol('a') !=  6");
   snax_assert( snax::char_to_symbol('b') ==  7, "snax::char_to_symbol('b') !=  7");
   snax_assert( snax::char_to_symbol('c') ==  8, "snax::char_to_symbol('c') !=  8");
   snax_assert( snax::char_to_symbol('d') ==  9, "snax::char_to_symbol('d') !=  9");
   snax_assert( snax::char_to_symbol('e') == 10, "snax::char_to_symbol('e') != 10");
   snax_assert( snax::char_to_symbol('f') == 11, "snax::char_to_symbol('f') != 11");
   snax_assert( snax::char_to_symbol('g') == 12, "snax::char_to_symbol('g') != 12");
   snax_assert( snax::char_to_symbol('h') == 13, "snax::char_to_symbol('h') != 13");
   snax_assert( snax::char_to_symbol('i') == 14, "snax::char_to_symbol('i') != 14");
   snax_assert( snax::char_to_symbol('j') == 15, "snax::char_to_symbol('j') != 15");
   snax_assert( snax::char_to_symbol('k') == 16, "snax::char_to_symbol('k') != 16");
   snax_assert( snax::char_to_symbol('l') == 17, "snax::char_to_symbol('l') != 17");
   snax_assert( snax::char_to_symbol('m') == 18, "snax::char_to_symbol('m') != 18");
   snax_assert( snax::char_to_symbol('n') == 19, "snax::char_to_symbol('n') != 19");
   snax_assert( snax::char_to_symbol('o') == 20, "snax::char_to_symbol('o') != 20");
   snax_assert( snax::char_to_symbol('p') == 21, "snax::char_to_symbol('p') != 21");
   snax_assert( snax::char_to_symbol('q') == 22, "snax::char_to_symbol('q') != 22");
   snax_assert( snax::char_to_symbol('r') == 23, "snax::char_to_symbol('r') != 23");
   snax_assert( snax::char_to_symbol('s') == 24, "snax::char_to_symbol('s') != 24");
   snax_assert( snax::char_to_symbol('t') == 25, "snax::char_to_symbol('t') != 25");
   snax_assert( snax::char_to_symbol('u') == 26, "snax::char_to_symbol('u') != 26");
   snax_assert( snax::char_to_symbol('v') == 27, "snax::char_to_symbol('v') != 27");
   snax_assert( snax::char_to_symbol('w') == 28, "snax::char_to_symbol('w') != 28");
   snax_assert( snax::char_to_symbol('x') == 29, "snax::char_to_symbol('x') != 29");
   snax_assert( snax::char_to_symbol('y') == 30, "snax::char_to_symbol('y') != 30");
   snax_assert( snax::char_to_symbol('z') == 31, "snax::char_to_symbol('z') != 31");

   for(unsigned char i = 0; i<255; i++) {
      if((i >= 'a' && i <= 'z') || (i >= '1' || i <= '5')) continue;
      snax_assert( snax::char_to_symbol((char)i) == 0, "snax::char_to_symbol() != 0");
   }
}

void test_types::string_to_name() {

   snax_assert( snax::string_to_name("a") == N(a) , "snax::string_to_name(a)" );
   snax_assert( snax::string_to_name("ba") == N(ba) , "snax::string_to_name(ba)" );
   snax_assert( snax::string_to_name("cba") == N(cba) , "snax::string_to_name(cba)" );
   snax_assert( snax::string_to_name("dcba") == N(dcba) , "snax::string_to_name(dcba)" );
   snax_assert( snax::string_to_name("edcba") == N(edcba) , "snax::string_to_name(edcba)" );
   snax_assert( snax::string_to_name("fedcba") == N(fedcba) , "snax::string_to_name(fedcba)" );
   snax_assert( snax::string_to_name("gfedcba") == N(gfedcba) , "snax::string_to_name(gfedcba)" );
   snax_assert( snax::string_to_name("hgfedcba") == N(hgfedcba) , "snax::string_to_name(hgfedcba)" );
   snax_assert( snax::string_to_name("ihgfedcba") == N(ihgfedcba) , "snax::string_to_name(ihgfedcba)" );
   snax_assert( snax::string_to_name("jihgfedcba") == N(jihgfedcba) , "snax::string_to_name(jihgfedcba)" );
   snax_assert( snax::string_to_name("kjihgfedcba") == N(kjihgfedcba) , "snax::string_to_name(kjihgfedcba)" );
   snax_assert( snax::string_to_name("lkjihgfedcba") == N(lkjihgfedcba) , "snax::string_to_name(lkjihgfedcba)" );
   snax_assert( snax::string_to_name("mlkjihgfedcba") == N(mlkjihgfedcba) , "snax::string_to_name(mlkjihgfedcba)" );
   snax_assert( snax::string_to_name("mlkjihgfedcba1") == N(mlkjihgfedcba2) , "snax::string_to_name(mlkjihgfedcba2)" );
   snax_assert( snax::string_to_name("mlkjihgfedcba55") == N(mlkjihgfedcba14) , "snax::string_to_name(mlkjihgfedcba14)" );

   snax_assert( snax::string_to_name("azAA34") == N(azBB34) , "snax::string_to_name N(azBB34)" );
   snax_assert( snax::string_to_name("AZaz12Bc34") == N(AZaz12Bc34) , "snax::string_to_name AZaz12Bc34" );
   snax_assert( snax::string_to_name("AAAAAAAAAAAAAAA") == snax::string_to_name("BBBBBBBBBBBBBDDDDDFFFGG") , "snax::string_to_name BBBBBBBBBBBBBDDDDDFFFGG" );
}

void test_types::name_class() {

   snax_assert( snax::name{snax::string_to_name("azAA34")}.value == N(azAA34), "snax::name != N(azAA34)" );
   snax_assert( snax::name{snax::string_to_name("AABBCC")}.value == 0, "snax::name != N(0)" );
   snax_assert( snax::name{snax::string_to_name("AA11")}.value == N(AA11), "snax::name != N(AA11)" );
   snax_assert( snax::name{snax::string_to_name("11AA")}.value == N(11), "snax::name != N(11)" );
   snax_assert( snax::name{snax::string_to_name("22BBCCXXAA")}.value == N(22), "snax::name != N(22)" );
   snax_assert( snax::name{snax::string_to_name("AAAbbcccdd")} == snax::name{snax::string_to_name("AAAbbcccdd")}, "snax::name == snax::name" );

   uint64_t tmp = snax::name{snax::string_to_name("11bbcccdd")};
   snax_assert(N(11bbcccdd) == tmp, "N(11bbcccdd) == tmp");
}
