/**
 * Copyright (c) 2011-2014 sx developers (see AUTHORS)
 *
 * This file is part of sx.
 *
 * sx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "precompile.hpp"
#include <sx/prop_tree.hpp>

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <bitcoin/bitcoin.hpp>
#include <wallet/wallet.hpp>
#include <sx/define.hpp>
#include <sx/serializer/address.hpp>
#include <sx/serializer/ec_public.hpp>
#include <sx/serializer/header.hpp>
#include <sx/serializer/input.hpp>
#include <sx/serializer/output.hpp>
#include <sx/serializer/point.hpp>
#include <sx/serializer/prefix.hpp>
#include <sx/serializer/script.hpp>
#include <sx/serializer/transaction.hpp>
#include <sx/serializer/wrapper.hpp>

using namespace bc;
using namespace libwallet;
using namespace pt;

namespace sx {
namespace serializer {

// Edit with care - text property names trade DRY for readability.

ptree prop_tree(const header& header)
{
    const block_header_type& block_header = header;

    ptree tree;
    tree.put("bits", block_header.bits);
    tree.put("hash", base16(hash_block_header(block_header)));
    tree.put("merkle_tree_hash", base16(block_header.merkle));
    tree.put("nonce", block_header.nonce);
    tree.put("previous_block_hash", base16(block_header.previous_block_hash));
    tree.put("time_stamp", block_header.timestamp);
    tree.put("version", block_header.version);
    return tree;
}

ptree prop_tree(const std::vector<header>& headers)
{
    return prop_tree_list("header", headers);
}

ptree prop_tree(const history_row& row)
{
    ptree tree;
    tree.put("value", row.value);
    tree.put("output.point", point(row.output));

    // missing => pending
    if (row.output_height != 0)
        tree.put("output.height", row.output_height);

    // missing => unspent
    if (row.spend.hash != null_hash)
    {
        tree.put("input.point", point(row.spend));

        // missing => pending
        if (row.spend_height != 0)
            tree.put("input.height", row.spend_height);
    }

    return tree;
}

ptree prop_tree(const std::vector<history_row>& rows)
{
    return prop_tree_list("history", rows);
}

ptree prop_tree(const payment_address& history_address,
    const std::vector<history_row>& rows)
{
    ptree tree;
    tree.put("address", address(history_address));
    tree.add_child("histories", prop_tree(rows));
    return tree;
}

ptree prop_tree(const std::vector<balance_row>& rows,
    const payment_address& balance_address)
{
    ptree tree;
    uint64_t value(0);
    uint64_t total_recieved(0);
    uint64_t received_balance(0);
    uint64_t pending_balance(0);

    // We do not assert against the quality of server response values.
    for (const auto& row: rows)
    {
        total_recieved += value;

        if (row.spend.hash == null_hash)
            pending_balance += row.value;

        if (row.output_height != 0 && 
            (row.spend.hash == null_hash || row.spend_height != 0))
            received_balance += row.value;
    }

    tree.put("address", address(balance_address));
    tree.put("paid", total_recieved);
    tree.put("pending", pending_balance);
    tree.put("received", received_balance);
    return tree;
}

ptree prop_tree(const tx_input_type& tx_input)
{
    ptree tree;
    payment_address script_address;
    if (extract(script_address, tx_input.script))
        tree.put("address", address(script_address));

    tree.put("previous_output", point(tx_input.previous_output));
    tree.put("script", script(tx_input.script).mnemonic());
    tree.put("sequence", tx_input.sequence);
    return tree;
}

ptree prop_tree(const std::vector<tx_input_type>& tx_inputs)
{
    return prop_tree_list("input", tx_inputs);
}

ptree prop_tree(const input& input)
{
    const tx_input_type& tx_input = input;
    return prop_tree(tx_input);
}

ptree prop_tree(const std::vector<input>& inputs)
{
    return prop_tree_list("input", cast<input, tx_input_type>(inputs));
}

ptree prop_tree(const tx_output_type& tx_output)
{
    ptree tree;
    payment_address output_address;
    if (extract(output_address, tx_output.script))
        tree.put("address", address(output_address));

    tree.put("script", script(tx_output.script).mnemonic());

    // TODO: consider independent stealth object serialization.
    // TODO: this will eventually change privacy problems, see:
    // lists.dyne.org/lurker/message/20140812.214120.317490ae.en.html
    stealth_info stealth;
    if (extract_stealth_info(stealth, tx_output.script))
    {
        tree.put("stealth.bit_field", stealth.bitfield);
        tree.put("stealth.ephemeral_public_key",
            ec_public(stealth.ephem_pubkey));
    }

    tree.put("value", tx_output.value);
    return tree;
}

ptree prop_tree(const std::vector<tx_output_type>& tx_outputs)
{
    return prop_tree_list("output", tx_outputs);
}

ptree prop_tree(const output& output)
{
    const std::vector<tx_output_type>& tx_outputs = output;

    ptree tree;
    tree.put("pay_to", output.payto());
    tree.add_child("outputs", prop_tree(tx_outputs));
    return tree;
}

ptree prop_tree(const std::vector<output>& outputs)
{
    return prop_tree_list("output", outputs);
}

ptree prop_tree(const transaction& transaction)
{
    const tx_type& tx = transaction;

    ptree tree;
    tree.put("hash", base16(hash_transaction(tx)));
    tree.put("version", tx.version);
    tree.put("lock_time", tx.locktime);
    tree.add_child("inputs", prop_tree(tx.inputs));
    tree.add_child("outputs", prop_tree(tx.outputs));
    return tree;
}

ptree prop_tree(const std::vector<transaction>& transactions)
{
    return prop_tree_list("transaction", transactions);
}

ptree prop_tree(const tx_type& tx, const hash_digest& block_hash,
    const prefix& prefix)
{
    ptree tree;
    tree.add_child("watch.transaction", prop_tree(tx));
    tree.add("watch.block", base16(block_hash));
    tree.add("watch.prefix", prefix);
    return tree;
}

ptree prop_tree(const std::vector<ec_point>& public_keys)
{
    return prop_value_list("public_key", 
        cast<ec_point, ec_public>(public_keys));
}

ptree prop_tree(const stealth& address)
{
    const stealth_address& addr = address;

    // We don't serialize a "reuse key" value as this is strictly an 
    // optimization for the purpose of serialization and otherwise complicates
    // understanding of what is actually otherwise very simple behavior.
    // So instead we emit the reused key as one of the spend keys.
    // This means that it is typical to see the same key in scan and spend.

    auto spend_keys = cast<ec_point, ec_public>(addr.get_spend_pubkeys());

    ptree tree;
    tree.put("address.encoded", addr.encoded());
    tree.put("address.prefix", prefix(addr.get_prefix()));
    tree.put("address.scan_public_key", ec_public(addr.get_scan_pubkey()));
    tree.put("address.signatures", addr.get_signatures());
    tree.add_child("address.spend", prop_tree(addr.get_spend_pubkeys()));
    tree.put("address.testnet", addr.get_testnet());
    return tree;
}

ptree prop_tree(const std::vector<stealth>& addresses)
{
    return prop_tree_list("stealth", addresses);
}

ptree prop_tree(const wrapped_data& wrapped)
{
    ptree tree;
    tree.put("wrapper.version", wrapped.version);
    tree.put("wrapper.payload", base16(wrapped.payload));
    tree.put("wrapper.checksum", wrapped.checksum);
    return tree;
}

} // serializer
} // sx
