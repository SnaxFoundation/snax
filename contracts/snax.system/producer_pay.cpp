#include "snax.system.hpp"

#include <snax.token/snax.token.hpp>

namespace snaxsystem {

   const int64_t  min_pervote_pay = 100'0000;
   const int64_t  min_activated_stake   = 10'000'000'000'0000;
   const uint32_t blocks_per_day        = 2 * 24 * 3600;
   const uint64_t useconds_per_day      = 24 * 3600 * uint64_t(1000000);
   const int64_t  min_per_block_amount  = 4'0000;

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

         const asset system_supply_limit = snax::token(N(snax.token)).get_max_supply(snax::symbol_type(system_token_symbol).name());

         const asset system_supply_soft_limit = system_supply_limit / 10;

         const asset platform_full_balance = get_platform_full_balance();

         const asset issued_supply = snax::token(N(snax.token)).get_supply(snax::symbol_type(system_token_symbol).name());

         const asset circulating_supply =
            issued_supply
            - platform_full_balance
            - get_balance(N(snax));

         const asset supply_difference = system_supply_soft_limit - circulating_supply;

         asset bp_reward = asset(
            static_cast<int64_t>(
                convert_asset_to_double(supply_difference) * 5e-10
                * get_block_reward_multiplier(
                    convert_asset_to_double(
                        circulating_supply
                        - _gstate.total_bp_reward
                        - asset(staked_by_team_initial)
                        - asset(airdrop_initial)
                        - asset(account_creator_initial)
                    ) / 1'0000
                )
            )
         );

         if (bp_reward.amount < min_per_block_amount)
             bp_reward = asset(min_per_block_amount);

         const asset semi_bp_reward = asset(bp_reward.amount / 2);

         if (semi_bp_reward > asset(0) && bp_reward + issued_supply < system_supply_limit) {
               INLINE_ACTION_SENDER(snax::token, issue)( N(snax.token), {N(snax),N(active)},
                                                                                   { N(snax.bpay), semi_bp_reward, "fund per-block bucket" } );
               INLINE_ACTION_SENDER(snax::token, issue)( N(snax.token), {N(snax),N(active)},
                                                                                   { N(snax.vpay), semi_bp_reward, "fund per-vote bucket" } );

               const asset to_per_block_pay   = semi_bp_reward;
               const asset to_per_vote_pay    = semi_bp_reward;

               _gstate.pervote_bucket  += to_per_vote_pay.amount;
               _gstate.perblock_bucket += to_per_block_pay.amount;
               _gstate.total_bp_reward += to_per_vote_pay + to_per_block_pay;

               _gstate.last_pervote_bucket_fill = ct;

         }

         _producers.modify( prod, 0, [&](auto& p ) {
               p.unpaid_blocks++;
               p.last_block_time = timestamp;
         });
      }

      /// only update block producers once every minute, block_timestamp is in half seconds
      if( timestamp.slot - _gstate.last_producer_schedule_update.slot >= 120 ) {
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

      const auto usecs_since_last_fill = ct - _gstate.last_pervote_bucket_fill;

      int64_t producer_per_block_pay = 0;
      if( _gstate.total_unpaid_blocks > 0 ) {
         producer_per_block_pay = (_gstate.perblock_bucket * prod.unpaid_blocks) / _gstate.total_unpaid_blocks;
      }
      int64_t producer_per_vote_pay = 0;
      if( _gstate.total_producer_vote_weight > 0 ) {
         producer_per_vote_pay  = int64_t((_gstate.pervote_bucket * prod.total_votes ) / _gstate.total_producer_vote_weight);
      }
      if( producer_per_vote_pay < min_pervote_pay ) {
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
