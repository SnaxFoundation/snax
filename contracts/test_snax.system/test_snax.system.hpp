/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <test_snax.system/native.hpp>
#include <snaxlib/asset.hpp>
#include <snaxlib/time.hpp>
#include <snaxlib/privileged.hpp>
#include <snaxlib/singleton.hpp>
#include <test_snax.system/exchange_state.hpp>

#include <string>

namespace snaxsystem {

   using snax::asset;
   using snax::indexed_by;
   using snax::const_mem_fun;
   using snax::block_timestamp;

   struct name_bid {
     account_name            newname;
     account_name            high_bidder;
     int64_t                 high_bid = 0; ///< negative high_bid == closed auction waiting to be claimed
     uint64_t                last_bid_time = 0;

     auto     primary_key()const { return newname;                          }
     uint64_t by_high_bid()const { return static_cast<uint64_t>(-high_bid); }
   };

   typedef snax::multi_index< N(namebids), name_bid,
                               indexed_by<N(highbid), const_mem_fun<name_bid, uint64_t, &name_bid::by_high_bid>  >
                               >  name_bid_table;


   struct snax_global_state : snax::blockchain_parameters {
      uint64_t free_ram()const { return max_ram_size - total_ram_bytes_reserved; }

      uint64_t             max_ram_size = 5ll*1024 * 1024 * 1024;
      uint64_t             total_ram_bytes_reserved = 0;
      int64_t              total_ram_stake = 0;

      block_timestamp      last_producer_schedule_update;
      uint64_t             last_pervote_bucket_fill = 0;
      int64_t              pervote_bucket = 0;
      int64_t              perblock_bucket = 0;
      uint32_t             total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
      int64_t              total_activated_stake = 0;
      uint64_t             thresh_activated_stake_time = 0;
      uint16_t             last_producer_schedule_size = 0;
      double               total_producer_vote_weight = 0; /// the sum of all producer votes
      block_timestamp      last_name_close;
      block_timestamp      start_time = block_timestamp(snax::time_point_sec(now()));
      bool                 initialized = false;
      int64_t              system_parabola_a = 4'385'772;
      int64_t              system_parabola_b = -1'324'503'311;
      asset                last_bp_semi_reward = asset(0);

      // explicit serialization macro is not necessary, used here only to improve compilation time
      SNAXLIB_SERIALIZE_DERIVED( snax_global_state, snax::blockchain_parameters,
                                (max_ram_size)(total_ram_bytes_reserved)(total_ram_stake)
                                (last_producer_schedule_update)(last_pervote_bucket_fill)
                                (pervote_bucket)(perblock_bucket)(total_unpaid_blocks)(total_activated_stake)(thresh_activated_stake_time)
                                (last_producer_schedule_size)(total_producer_vote_weight)(last_name_close)(start_time)(initialized)
                                (system_parabola_a)(system_parabola_b)(last_bp_semi_reward)
                            )
   };

   struct producer_info {
      account_name          owner;
      double                total_votes = 0;
      snax::public_key     producer_key; /// a packed public key object
      bool                  is_active = true;
      std::string           url;
      uint32_t              unpaid_blocks = 0;
      uint64_t              last_claim_time = 0;
      block_timestamp       last_block_time;
      uint16_t              location = 0;

      uint64_t primary_key()const { return owner;                                   }
      double   by_votes()const    { return is_active ? -total_votes : total_votes;  }
      bool     active()const      { return is_active;                               }
      void     deactivate()       { producer_key = public_key(); is_active = false; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      SNAXLIB_SERIALIZE( producer_info, (owner)(total_votes)(producer_key)(is_active)(url)
                        (unpaid_blocks)(last_claim_time)(last_block_time)(location) )
   };

   struct account_with_balance {
       asset    balance;
       uint64_t primary_key()const { return balance.symbol.name(); }
   };

   // @abi table platsteps i64
   struct platform_request {
       block_timestamp request;
       asset token_amount;

       uint64_t primary_key() const { return request.to_time_point().time_since_epoch().count(); }
   };

   // @abi table platlocks i64
   struct platform_lock {
       block_timestamp time;

       uint64_t primary_key() const { return time.to_time_point().time_since_epoch().count(); }
   };

   struct voter_info {
      account_name                owner = 0; /// the voter
      account_name                proxy = 0; /// the proxy set by the voter, if any
      std::vector<account_name>   producers; /// the producers approved by this voter if no proxy set
      int64_t                     staked = 0;

      /**
       *  Every time a vote is cast we must first "undo" the last vote weight, before casting the
       *  new vote weight.  Vote weight is calculated as:
       *
       *  staked.amount * 2 ^ ( weeks_since_launch/weeks_per_year)
       */
      double                      last_vote_weight = 0; /// the vote weight cast the last time the vote was updated

      /**
       * Total vote weight delegated to this voter.
       */
      double                      proxied_vote_weight= 0; /// the total vote weight delegated to this voter as a proxy
      bool                        is_proxy = 0; /// whether the voter is a proxy for others


      uint32_t                    reserved1 = 0;
      time                        reserved2 = 0;
      snax::asset                reserved3;

      uint64_t primary_key()const { return owner; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      SNAXLIB_SERIALIZE( voter_info, (owner)(proxy)(producers)(staked)(last_vote_weight)(proxied_vote_weight)(is_proxy)(reserved1)(reserved2)(reserved3) )
   };

   /// @abi table escband
   struct escrow_bandwidth {
       account_name owner;
       block_timestamp created;
       uint8_t period_count;
       asset initial_amount;
       asset amount;

       uint64_t  primary_key()const { return block_timestamp(created).to_time_point().time_since_epoch().to_seconds(); }

       SNAXLIB_SERIALIZE( escrow_bandwidth, (owner)(created)(period_count)(initial_amount)(amount) )
   };

   typedef snax::multi_index< N(accounts), account_with_balance > _accounts_balances;

   typedef snax::multi_index< N(platsteps), platform_request > _platform_requests;

   typedef snax::multi_index< N(platlocks), platform_lock > _platform_locks;

   typedef snax::multi_index< N(voters), voter_info >  voters_table;


   typedef snax::multi_index< N(producers), producer_info,
                               indexed_by<N(prototalvote), const_mem_fun<producer_info, double, &producer_info::by_votes>  >
                               >  producers_table;

   typedef snax::singleton< N(global), snax_global_state > global_state_singleton;

   typedef snax::multi_index<N(escband), escrow_bandwidth> escrow_bandwidth_table;

   //   static constexpr uint32_t     max_inflation_rate = 5;  // 5% annual inflation
   static constexpr uint32_t     seconds_per_day = 24 * 3600;
   static constexpr uint64_t     system_token_symbol = CORE_SYMBOL;

   static double snax_vote_multipliers[] = {
        1.00000000000000,
        0.90000000000000,
        0.80000000000000,
        0.70000000000000,
        0.55000000000000,
        0.40000000000000,
        0.25000000000000,
        0.10000000000000,
        0.05000000000000,
        0.02500000000000,
        0.01250000000000,
        0.00625000000000,
        0.00312500000000,
        0.00156250000000,
        0.00078125000000,
        0.00039062500000,
        0.00019531250000,
        0.00009765625000,
        0.00004882812500,
        0.00002441406250,
        0.00001220703125,
        0.00000610351563,
        0.00000244140625,
        0.00000244140625,
        0.00000244140625,
        0.00000244140625,
        0.00000244140625,
        0.00000244140625,
        0.00000244140625,
        0.00000244140625,
   };

   class system_contract : public native {
      private:
         voters_table           _voters;
         producers_table        _producers;
         global_state_singleton _global;

         snax_global_state     _gstate;
         rammarket              _rammarket;

      public:
         system_contract( account_name s );
         ~system_contract();

         // Actions:
         void onblock( block_timestamp timestamp, account_name producer );
                      // const block_header& header ); /// only parse first 3 fields of block header

         // functions defined in delegate_bandwidth.cpp

         /**
          *  Stakes SNAX from the balance of 'from' for the benfit of 'receiver'.
          *  If transfer == true, then 'receiver' can unstake to their account
          *  Else 'from' can unstake at any time.
          */
         void delegatebw( account_name from, account_name receiver,
                          asset stake_net_quantity, asset stake_cpu_quantity, bool transfer );

         void escrowbw( account_name from, account_name receiver,
                          asset stake_net_quantity, asset stake_cpu_quantity, bool transfer, uint8_t period_count );



         /**
          *  Decreases the total tokens delegated by from to receiver and/or
          *  frees the memory associated with the delegation if there is nothing
          *  left to delegate.
          *
          *  This will cause an immediate reduction in net/cpu bandwidth of the
          *  receiver.
          *
          *  A transaction is scheduled to send the tokens back to 'from' after
          *  the staking period has passed. If existing transaction is scheduled, it
          *  will be canceled and a new transaction issued that has the combined
          *  undelegated amount.
          *
          *  The 'from' account loses voting power as a result of this call and
          *  all producer tallies are updated.
          */
         void undelegatebw( account_name from, account_name receiver,
                            asset unstake_net_quantity, asset unstake_cpu_quantity );

         void lockplatform( account_name& platform );

         void emitplatform( account_name& platform );

         void resetvotes();

         /**
          * Increases receiver's ram quota based upon current price and quantity of
          * tokens provided. An inline transfer from receiver to system contract of
          * tokens will be executed.
          */
         void buyram( account_name buyer, account_name receiver, asset tokens );
         void buyrambytes( account_name buyer, account_name receiver, uint32_t bytes );

         /**
          *  Reduces quota my bytes and then performs an inline transfer of tokens
          *  to receiver based upon the average purchase price of the original quota.
          */
         void sellram( account_name receiver, int64_t bytes );

         /**
          *  This action is called after the delegation-period to claim all pending
          *  unstaked tokens belonging to owner
          */
         void refund( account_name owner );

         // functions defined in voting.cpp

         void regproducer( const account_name producer, const public_key& producer_key, const std::string& url, uint16_t location );

         void unregprod( const account_name producer );

         void setram( uint64_t max_ram_size );

         void voteproducer( const account_name voter, const account_name proxy, const std::vector<account_name>& producers );

         void regproxy( const account_name proxy, bool isproxy );

         void setplatforms( const std::vector<snax::platform_config_extended>& platforms );

         void setparams( const snax::blockchain_parameters& params );

         // functions defined in producer_pay.cpp
         void claimrewards( const account_name& owner );

         void setpriv( account_name account, uint8_t ispriv );

         void rmvproducer( account_name producer );

         void bidname( account_name bidder, account_name newname, asset bid );
      private:
         std::tuple<double, double> solve_quadratic_equation(double a, double b, double c) const;
         std::tuple<double, double> get_parabola(double x0, double y0) const;
         double calculate_parabola(double a, double b, double c, double x) const;
         double convert_asset_to_double(asset value) const;
         double apply_vote_weight(const account_name voter, const double vote_weight, const uint8_t iter) const {
             return voter == N(snax.team) ? vote_weight * snax_vote_multipliers[iter]: vote_weight;
         }
         asset get_balance(account_name account);

         void update_elected_producers( block_timestamp timestamp );

         // Implementation details:

         //defind in delegate_bandwidth.cpp
         void changebw( account_name from, account_name receiver,
                        asset stake_net_quantity, asset stake_cpu_quantity, bool transfer );

         //defined in voting.hpp
         static snax_global_state get_default_parameters();

         void update_votes( const account_name voter, const account_name proxy, const std::vector<account_name>& producers, bool voting );

         // defined in voting.cpp
         void propagate_weight_change( const voter_info& voter );
   };

} /// snaxsystem
