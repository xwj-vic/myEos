#include <hodl.hpp>
/**
  存款
  on_notify属性确保仅当通知来自eosio.token合同且来自eosio.token的transfer操作时，传入的通知才转发到存款操作
*/ 
[[eosio::on_notify("eosio.token::transfer")]]
ACTION hodl::deposit(name hodler, name to, eosio::asset quantity, string memo) {
  // 检查合同是否未转让给自己
    if (to != get_self() || hodler == get_self()) {
        print("These are not the droids you are looking for.");
        return;
    }

    //提款时间尚未过去
    check(now() < the_party, "You're way late"); 

    // 传入的转账具有有效数量的令牌
    check(quantity.amount > 0, "When pigs fly");

    // 传入的symbol使用我们在构造函数中指定的令牌
    check(quantity.symbol == hodl_symbol, "These are not the droids you are looking for.");

    // 更新余额
    balance_table balance(get_self(), hodler.value);
    auto hodl_it = balance.find(hodl_symbol.raw());

    if (hodl_it != balance.end())  
        balance.modify(hodl_it, get_self(), [&](auto &row) {
        row.funds += quantity;
        });
    else
        balance.emplace(get_self(), [&](auto &row) {
        row.funds = quantity;
        });
}


/**
  取款
  达到取款时间，则将代币转回原账户
*/
ACTION hodl::party(name hodler) {
      require_auth(hodler);
      check(now() > the_party, "Hold your horses");
      balance_table balance(get_self(), hodler.value);
      auto hodl_it = balance.find(hodl_symbol.raw());

      check(hodl_it != balance.end(), "You're not allowed to party");

      //内联操作，调用eosio.token的转账操作
      action{
        permission_level{get_self(), name("active")},
        name("eosio.token"),
        name("transfer"),
        std::make_tuple(get_self(), hodler, hodl_it -> funds, string("Party! Your hodl is free."))
      }.send();
      balance.erase(hodl_it);
}

EOSIO_DISPATCH(hodl, (deposit)(party))
