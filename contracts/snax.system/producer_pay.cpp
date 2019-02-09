#include "snax.system.hpp"

#include <snax.token/snax.token.hpp>

namespace snaxsystem {

   const int64_t  min_pervote_daily_pay = 100'0000;
   const int64_t  min_activated_stake   = 10'000'000'000'0000;
   const double   continuous_rate       = 0.04879;          // 5% annual rate
   const double   perblock_rate         = 0.0025;           // 0.25%
   const double   standby_rate          = 0.0075;           // 0.75%
   const uint32_t blocks_per_year       = 52*7*24*2*3600;   // half seconds per year
   const uint32_t seconds_per_year      = 52*7*24*3600;
   const uint32_t blocks_per_day        = 2 * 24 * 3600;
   const uint32_t blocks_per_hour       = 2 * 3600;
   const uint64_t useconds_per_day      = 24 * 3600 * uint64_t(1000000);
   const uint64_t useconds_per_year     = seconds_per_year*1000000ll;
   const asset    min_per_block_amount  = asset(15'8548);
   const asset    min_per_block_amount_multiplied_by_1e9 = min_per_block_amount * 1'000'000'000;

   void system_contract::onblock( block_timestamp timestamp, account_name producer ) {
      using namespace snax;

      require_auth(N(snax));

      /** until activated stake crosses this threshold no new rewards are paid */
      if( _gstate.total_activated_stake < min_activated_stake )
         return;

      const auto ct = current_time();

      if( _gstate.last_pervote_bucket_fill == 0 )  /// start the presses
         _gstate.last_pervote_bucket_fill = ct;

        /**
         * At startup the initial producer may not be one that is registered / elected
         * and therefore there may be no producer object for them.
         */
      auto prod = _producers.find(producer);
      if ( prod != _producers.end() ) {

         _gstate.total_unpaid_blocks++;

         const asset token_supply = token( N(snax.token)).get_supply(symbol_type(system_token_symbol).name() );

         const asset system_token_supply_soft_limit = asset(token( N(snax.token)).get_supply(symbol_type(system_token_symbol).name() ).amount / 10);

         const asset supply_difference = system_token_supply_soft_limit - token_supply;

         const asset bp_reward = supply_difference > asset(0) && supply_difference >= min_per_block_amount_multiplied_by_1e9
                     ? asset(supply_difference.amount / 1'000'000'000)
                     : min_per_block_amount;

         const asset semi_bp_reward = asset(bp_reward.amount / 2);

         if (bp_reward > asset(0) && semi_bp_reward > asset(0)) {

               INLINE_ACTION_SENDER(snax::token, issue)( N(snax.token), {N(snax),N(active)},
                                                                                   { N(snax.bpay), semi_bp_reward, "fund per-block bucket" } );

               INLINE_ACTION_SENDER(snax::token, issue)( N(snax.token), {N(snax),N(active)},
                                                                                   { N(snax.vpay), semi_bp_reward, "fund per-vote bucket" } );
         }

         auto to_producers         = bp_reward;
         auto to_per_block_pay   = to_producers / 2;
         auto to_per_vote_pay      = to_producers - to_per_block_pay;

         _gstate.pervote_bucket  += to_per_vote_pay.amount;
         _gstate.perblock_bucket += to_per_block_pay.amount;

         _gstate.last_pervote_bucket_fill = ct;
         _gstate.last_bp_semi_reward = semi_bp_reward;
         _producers.modify( prod, 0, [&](auto& p ) {
               p.unpaid_blocks++;
         });
      }

      /// only update block producers once every minute, block_timestamp is in half seconds
      if( timestamp.slot - _gstate.last_producer_schedule_update.slot > 120 ) {
         update_elected_producers( timestamp );

         if( (timestamp.slot - _gstate.last_name_close.slot) > blocks_per_day ) {
            name_bid_table bids(_self,_self);
            auto idx = bids.get_index<N(highbid)>();
            auto highest = idx.begin();
            if( highest != idx.end() &&
                highest->high_bid > 0 &&
                highest->last_bid_time < (current_time() - useconds_per_day) &&
                _gstate.thresh_activated_stake_time > 0 &&
                (current_time() - _gstate.thresh_activated_stake_time) > 14 * useconds_per_day ) {
                   _gstate.last_name_close = timestamp;
                   idx.modify( highest, 0, [&]( auto& b ){
                         b.high_bid = -b.high_bid;
               });
            }
         }
      }

      _global.set(_gstate, _self);
   }

   using namespace snax;
   void system_contract::claimrewards( const account_name& owner ) {
      require_auth(owner);

      const auto& prod = _producers.get( owner );
      snax_assert( prod.active(), "producer does not have an active key" );

      snax_assert( _gstate.total_activated_stake >= min_activated_stake,
                    "cannot claim rewards until the chain is activated (at least 10% of all tokens participate in voting)" );

      const auto ct = current_time();

      snax_assert( ct - prod.last_claim_time > useconds_per_day, "already claimed rewards within past day" );

      int64_t producer_per_block_pay = 0;
      if( _gstate.total_unpaid_blocks > 0 ) {
         producer_per_block_pay = ((_gstate.perblock_bucket - _gstate.last_bp_semi_reward.amount) * prod.unpaid_blocks) / _gstate.total_unpaid_blocks;
      }
      int64_t producer_per_vote_pay = 0;
      if( _gstate.total_producer_vote_weight > 0 ) {
         producer_per_vote_pay  = int64_t(((_gstate.pervote_bucket - _gstate.last_bp_semi_reward.amount) * prod.total_votes ) / _gstate.total_producer_vote_weight);
      }
      if( producer_per_vote_pay < min_pervote_daily_pay ) {
         producer_per_vote_pay = 0;
      }
      _gstate.pervote_bucket      -= producer_per_vote_pay;
      _gstate.perblock_bucket     -= producer_per_block_pay;
      _gstate.total_unpaid_blocks -= prod.unpaid_blocks;

      _producers.modify( prod, 0, [&](auto& p) {
          p.last_claim_time = ct;
          p.unpaid_blocks = 0;
      });

      if( producer_per_block_pay > 0 ) {
         INLINE_ACTION_SENDER(snax::token, transfer)( N(snax.token), {N(snax.bpay),N(active)},
                                                       { N(snax.bpay), owner, asset(producer_per_block_pay), std::string("producer block pay") } );
      }
      if( producer_per_vote_pay > 0 ) {
         INLINE_ACTION_SENDER(snax::token, transfer)( N(snax.token), {N(snax.vpay),N(active)},
                                                       { N(snax.vpay), owner, asset(producer_per_vote_pay), std::string("producer vote pay") } );
      }

      _global.set(_gstate, _self);
   }

} //namespace snaxsystem
