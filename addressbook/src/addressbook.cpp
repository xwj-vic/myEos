#include <addressbook.hpp>

ACTION addressbook::upsert(name user, string first_name, string last_name, uint64_t age, string street, string city, string state) {
  require_auth(user);
  address_index addresses(get_self(), get_self().value); //实例化表
  auto iterator = addresses.find(user.value); //创建迭代器
  if(iterator == addresses.end()) {
    addresses.emplace(user, [&](auto& row){   //mplace方法的回调函数必须使用lamba函数来创建引用,插入data
      row.key = user;
      row.first_name = first_name;
      row.last_name = last_name;
      row.age = age;
      row.street = street;
      row.city = city;
      row.state = state;
    });
    send_summary(user, "successfully emplaced record to addressbook");  //调用发送通知的方法
    increment_counter(user, "emplace");
  } else {
    addresses.modify(iterator, user, [&](auto& row){  //更新data
      row.key = user;
      row.first_name = first_name;
      row.last_name = last_name;
      row.age = age;
      row.street = street;
      row.city = city;
      row.state = state;
    });
    send_summary(user, "successfully modified record in addressbook."); //调用发送通知的方法
    increment_counter(user, "modify");
  }
}

ACTION addressbook::deleteuser(name user) {
  require_auth(user);
  address_index addresses(get_self(), get_self().value); //实例化表
  auto iterator = addresses.find(user.value); //创建迭代器
  check(iterator != addresses.end(), "Record does not exist");
  addresses.erase(iterator);  //删除record
  send_summary(user, "successfully erased record from addressbook"); //调用发送通知的方法
  increment_counter(user, "erase");
}

/**
  使用require_recipient将操作复制到发件人
  调用require_recipient将一个帐户添加到require_recipient中，并确保这些帐户收到有关正在执行的操作的通知。通知就像将操作的“副本”发送到require_recipient集中的帐户
*/
ACTION addressbook::notify(name user, string msg) {
  require_auth(get_self());  //为避免伪造，要求此操作的调用中提供的授权来自合同本身
  require_recipient(user);
}



EOSIO_DISPATCH(addressbook, (upsert)(deleteuser)(notify))
