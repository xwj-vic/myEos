#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

CONTRACT hodl : public contract {
  public:
    using contract::contract;
    hodl(name receiver, name code, datastream<const char *> ds):contract(receiver, code, ds), hodl_symbol("SYS", 4){}  //构造函数初始化使用的代币

    [[eosio::on_notify("eosio.token::transfer")]]
    ACTION deposit(name hodler, name to, eosio::asset quantity, string memo);

    ACTION party(name hodler);

  private:

     static const uint32_t the_party = 1528549200;
     const symbol hodl_symbol;  //使用“ SYS”符号

    TABLE balance {
         // asset是一种表示数字令牌资产的类型
        eosio::asset funds;   
        uint64_t primary_key() const { return funds.symbol.raw(); }
    };
    typedef multi_index<name("balance"), balance> balance_table;

    // 当前UTC时间
    uint32_t now() {
        return current_time_point().sec_since_epoch();
    }
};
