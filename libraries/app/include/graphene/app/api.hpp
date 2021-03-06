/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <graphene/app/database_api.hpp>

#include <graphene/chain/protocol/types.hpp>

#include <graphene/debug_miner/debug_api.hpp>

#include <graphene/net/node.hpp>
#include <graphene/chain/message_object.hpp>

#include <fc/api.hpp>
#include <fc/optional.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/network/ip.hpp>

#include <boost/container/flat_set.hpp>

#include <functional>
#include <map>
#include <string>
#include <vector>

/**
 * @defgroup HistoryAPI History API
 * @defgroup Network_broadcastAPI Network broadcastAPI
 * @defgroup Network_NodeAPI Network NodeAPI
 * @defgroup CryptoAPI Crypto API
 * @defgroup MessagingAPI Messaging API
 * @defgroup LoginAPI LoginAPI
 */
namespace graphene { namespace app {
   using namespace graphene::chain;
   using namespace fc::ecc;
   using namespace std;

   class application;

   struct verify_range_result
   {
      bool        success;
      uint64_t    min_val;
      uint64_t    max_val;
   };
   
   struct verify_range_proof_rewind_result
   {
      bool                          success;
      uint64_t                      min_val;
      uint64_t                      max_val;
      uint64_t                      value_out;
      fc::ecc::blind_factor_type    blind_out;
      string                        message_out;
   };

   /**
    * @brief The history_api class implements the RPC API for account history
    *
    * This API contains methods to access account histories
    */
   class history_api
   {
      public:
         history_api(application& app):_app(app){}

         /**
          * @brief Get operations relevant to the specificed account.
          * @param account the account whose history should be queried
          * @param stop ID of the earliest operation to retrieve
          * @param limit maximum number of operations to retrieve (must not exceed 100)
          * @param start ID of the most recent operation to retrieve
          * @return a list of operations performed by account, ordered from most recent to oldest
          * @ingroup HistoryAPI
          */
         vector<operation_history_object> get_account_history(account_id_type account,
                                                              operation_history_id_type stop = operation_history_id_type(),
                                                              unsigned limit = 100,
                                                              operation_history_id_type start = operation_history_id_type())const;
         /**
          * @brief Get operations relevant to the specified account referenced.
          * by an event numbering specific to the account. The current number of operations
          * for the account can be found in the account statistics (or use 0 for start).
          * @param account the account whose history should be queried
          * @param stop sequence number of earliest operation. 0 is default and will
          * query 'limit' number of operations
          * @param limit maximum number of operations to retrieve (must not exceed 100)
          * @param start sequence number of the most recent operation to retrieve.
          * 0 is default, which will start querying from the most recent operation
          * @return a list of operations performed by account, ordered from most recent to oldest
          * @ingroup HistoryAPI
          */
         vector<operation_history_object> get_relative_account_history( account_id_type account,
                                                                        uint32_t stop = 0,
                                                                        unsigned limit = 100,
                                                                        uint32_t start = 0) const;

      private:
           application& _app;
   };

   /**
    * @brief The network_broadcast_api class allows broadcasting of transactions.
    */
   class network_broadcast_api : public std::enable_shared_from_this<network_broadcast_api>
   {
      public:
         network_broadcast_api(application& a);

         struct transaction_confirmation
         {
            transaction_id_type   id;
            uint32_t              block_num;
            uint32_t              trx_num;
            processed_transaction trx;
         };

         typedef std::function<void(variant/*transaction_confirmation*/)> confirmation_callback;

         /**
          * @brief Broadcast a transaction to the network.
          * @param trx the transaction to broadcast
          * @note the transaction will be checked for validity in the local database prior to broadcasting. If it fails to
          * apply locally, an error will be thrown and the transaction will not be broadcast
          * @ingroup Network_broadcastAPI
          */
         void broadcast_transaction(const signed_transaction& trx);

         /**
          *
          * @brief This call will not return until the transaction is included in a block.
          * @param trx the transaction to broadcast
          * @ingroup Network_broadcastAPI
          */
         fc::variant broadcast_transaction_synchronous( const signed_transaction& trx);

         /**
          * @brief This version of broadcast transaction registers a callback method that will be called when the transaction is
          * included into a block.  The callback method includes the transaction id, block number, and transaction number in the
          * block.
          * @param cb callback function
          * @param trx the transaction to broadcast
          * @ingroup Network_broadcastAPI
          */
         void broadcast_transaction_with_callback( confirmation_callback cb, const signed_transaction& trx);

         /**
          * @brief Broadcast a block to the network.
          * @param block the signed block to broadcast
          * @ingroup Network_broadcastAPI
          */
         void broadcast_block( const signed_block& block );

         /**
          * @brief Not reflected, thus not accessible to API clients.
          * This function is registered to receive the applied_block
          * signal from the chain database when a block is received.
          * It then dispatches callbacks to clients who have requested
          * to be notified when a particular txid is included in a block.
          * @param b the signed block
          * @ingroup Network_broadcastAPI
          */
         void on_applied_block( const signed_block& b );
      private:
         boost::signals2::scoped_connection             _applied_block_connection;
         map<transaction_id_type,confirmation_callback> _callbacks;
         application&                                   _app;
   };

   /**
    * @brief The network_node_api class allows maintenance of p2p connections.
    */
   class network_node_api
   {
      public:
         network_node_api(application& a);

         /**
          * @brief Returns general network information, such as p2p port.
          * @return general network information
          * @ingroup Network_NodeAPI
          */
         fc::variant_object get_info() const;

         /**
          * @brief Connects to a new peer.
          * @param ep the IP/Port of the peer to connect to
          * @ingroup Network_NodeAPI
          */
         void add_node(const fc::ip::endpoint& ep);

         /**
          * @brief Get status of all current connections to peers.
          * @return status of all connected peers
          * @ingroup Network_NodeAPI
          */
         std::vector<net::peer_status> get_connected_peers() const;

         /**
          * @brief Get advanced node parameters, such as desired and max number of connections.
          * @return advanced node parameters
          * @ingroup Network_NodeAPI
          */
         fc::variant_object get_advanced_node_parameters() const;

         /**
          * @brief Set advanced node parameters, such as desired and max number of connections.
          * @param params a JSON object containing the name/value pairs for the parameters to set
          * @ingroup Network_NodeAPI
          */
         void set_advanced_node_parameters(const fc::variant_object& params);

         /**
          * @brief Get a list of potential peers we can connect to.
          * @return a list of potential peers
          * @ingroup Network_NodeAPI
          */
         std::vector<net::potential_peer_record> get_potential_peers() const;

        /**
         * @brief This method allows user to start seeding plugin from running application.
         * @param account_id ID of the account controlling this seeder
         * @param content_private_key El Gamal content private key
         * @param seeder_private_key private key of the account controlling this seeder
         * @param free_space allocated disk space, in MegaBytes
         * @param seeding_price price per MegaBytes
         * @param seeding_symbol seeding price asset, e.g. DCT
         * @param packages_path packages storage path
         * @param region_code optional ISO 3166-1 alpha-2 two-letter region code
         * @ingroup Network_NodeAPI
         */
         void seeding_startup(const account_id_type& account_id,
                              const DInteger& content_private_key,
                              const fc::ecc::private_key& seeder_private_key,
                              const uint64_t free_space,
                              const uint32_t seeding_price,
                              const string seeding_symbol,
                              const string packages_path,
                              const string region_code = "" );

      private:
         application& _app;
   };

   /**
    *
    */
   class crypto_api
   {
      public:
         crypto_api();

         /**
          * @param key
          * @param hash
          * @param i
          * @ingroup CryptoAPI
          */
         fc::ecc::blind_signature blind_sign( const extended_private_key_type& key, const fc::ecc::blinded_hash& hash, int i );

         /**
          * @param key
          * @param bob
          * @param sig
          * @param hash
          * @param i
          * @ingroup CryptoAPI
          */
         signature_type unblind_signature( const extended_private_key_type& key,
                                              const extended_public_key_type& bob,
                                              const fc::ecc::blind_signature& sig,
                                              const fc::sha256& hash,
                                              int i );

         /**
          * @param blind
          * @param value
          * @ingroup CryptoAPI
          */
         fc::ecc::commitment_type blind( const fc::ecc::blind_factor_type& blind, uint64_t value );

         /**
          * @param blinds_in
          * @param non_neg
          * @param CryptoAPI
          */
         fc::ecc::blind_factor_type blind_sum( const std::vector<blind_factor_type>& blinds_in, uint32_t non_neg );

         /**
          * @param commits_in
          * @param neg_commits_in
          * @param excess
          * @ingroup CryptoAPI
          */
         bool verify_sum( const std::vector<commitment_type>& commits_in, const std::vector<commitment_type>& neg_commits_in, int64_t excess );

         /**
          * @param commit
          * @param proof
          * @ingroup CryptoAPI
          */
         verify_range_result verify_range( const fc::ecc::commitment_type& commit, const std::vector<char>& proof );

         /**
          * @param min_value
          * @param commit
          * @param commit_blind
          * @param nonce
          * @param base10_exp
          * @param min_bits
          * @param actual_value
          * @ingroup CryptoAPI
          */
         std::vector<char> range_proof_sign( uint64_t min_value, 
                                             const commitment_type& commit, 
                                             const blind_factor_type& commit_blind, 
                                             const blind_factor_type& nonce,
                                             int8_t base10_exp,
                                             uint8_t min_bits,
                                             uint64_t actual_value );
                                       
         /**
          * @param nonce
          * @param commit
          * @param proof
          * @ingroup CryptoAPI
          */
         verify_range_proof_rewind_result verify_range_proof_rewind( const blind_factor_type& nonce,
                                                                     const fc::ecc::commitment_type& commit, 
                                                                     const std::vector<char>& proof );
         
         /**
          * @param proof
          * @ingroup CryptoAPI
          */
         range_proof_info range_get_info( const std::vector<char>& proof );
   };

   /**
   * @brief The messaging_api class implements instant messaging
   */
   class messaging_api
   {
   public:
      messaging_api(application& a);

      /**
       * @brief Receives message objects by sender and/or receiver.
       * @param sender name of message sender. If you dont want to filter by sender then let it empty
       * @param receiver name of message receiver. If you dont want to filter by receiver then let it empty
       * @param max_count maximal number of last messages to be displayed
       * @return a vector of message objects
       * @ingroup MessagingAPI
       */
      vector<message_object> get_message_objects(optional<account_id_type> sender, optional<account_id_type> receiver, uint32_t max_count) const;
   private:
      application& _app;
   };

   /**
    * @brief The login_api class implements the bottom layer of the RPC API
    *
    * All other APIs must be requested from this API.
    */
   class login_api
   {
      public:
         login_api(application& a);
         ~login_api();

         /**
          * @brief Authenticate to the RPC server.
          * @note This must be called prior to requesting other APIs. Other APIs may not be accessible until the client
          * has sucessfully authenticated.
          * @param user username to login with
          * @param password password to login with
          * @return \c true if logged in successfully, \c false otherwise
          * @ingroup LoginAPI
          */
         bool login(const string& user, const string& password);

         /**
          * @brief Retrieve the network broadcast API.
          * @ingroup LoginAPI
          */
         fc::api<network_broadcast_api> network_broadcast()const;
         /**
          * @brief Retrieve the database API.
          * @ingroup LoginAPI
          */
         fc::api<database_api> database()const;
         /**
          * @brief Retrieve the history API.
          * @ingroup LoginAPI
          */
         fc::api<history_api> history()const;
         /**
          * @brief Retrieve the network node API.
          * @ingroup LoginAPI
          */
         fc::api<network_node_api> network_node()const;
         /**
          * @brief Retrieve the cryptography API.
          * @ingroup LoginAPI
          */
         fc::api<crypto_api> crypto()const;
         /**
         * @brief Retrieve the messaging API.
         * @ingroup LoginAPI
         */
         fc::api<messaging_api> messaging()const;
         /**
          * @brief Retrieve the debug API (if available).
          * @ingroup LoginAPI
          */
         fc::api<graphene::debug_miner::debug_api> debug()const;

      private:
         /**
          * @brief Called to enable an API, not reflected.
          * @param api_name name of the API we are trying to enable
          * @ingroup LoginAPI
          */
         void enable_api( const string& api_name );

         application& _app;
         optional< fc::api<database_api> > _database_api;
         optional< fc::api<network_broadcast_api> > _network_broadcast_api;
         optional< fc::api<network_node_api> > _network_node_api;
         optional< fc::api<history_api> >  _history_api;
         optional< fc::api<crypto_api> > _crypto_api;
         optional< fc::api<messaging_api> > _messaging_api;
         optional< fc::api<graphene::debug_miner::debug_api> > _debug_api;
   };

}}  // graphene::app

FC_REFLECT( graphene::app::network_broadcast_api::transaction_confirmation,
        (id)(block_num)(trx_num)(trx) )
FC_REFLECT( graphene::app::verify_range_result,
        (success)(min_val)(max_val) )
FC_REFLECT( graphene::app::verify_range_proof_rewind_result,
        (success)(min_val)(max_val)(value_out)(blind_out)(message_out) )
//FC_REFLECT_TYPENAME( fc::ecc::compact_signature );
//FC_REFLECT_TYPENAME( fc::ecc::commitment_type );

FC_API(graphene::app::history_api,
       (get_account_history)
       (get_relative_account_history)
     )
FC_API(graphene::app::network_broadcast_api,
       (broadcast_transaction)
       (broadcast_transaction_with_callback)
       (broadcast_block)
     )
FC_API(graphene::app::network_node_api,
       (get_info)
       (add_node)
       (get_connected_peers)
       (get_potential_peers)
       (get_advanced_node_parameters)
       (set_advanced_node_parameters)
       (seeding_startup)
     )
FC_API(graphene::app::crypto_api,
       (blind_sign)
       (unblind_signature)
       (blind)
       (blind_sum)
       (verify_sum)
       (verify_range)
       (range_proof_sign)
       (verify_range_proof_rewind)
       (range_get_info)
     )
FC_API(graphene::app::messaging_api,
      (get_message_objects)
     )
FC_API(graphene::app::login_api,
       (login)
       (network_broadcast)
       (database)
       (history)
       (network_node)
       (crypto)
       (debug)
       (messaging)
     )
