#include <abcounter.hpp>

ACTION abcounter::count(name user, string type) {
  require_auth(name("addressbook"));
  count_index counts(get_self(), get_self().value);

  auto iterator = counts.find(user.value);

  if (iterator == counts.end()) {
    counts.emplace(name("addressbook"), [&]( auto& row ) {
          row.key = user;
          row.emplaced = (type == "emplace") ? 1 : 0;
          row.modified = (type == "modify") ? 1 : 0;
          row.erased = (type == "erase") ? 1 : 0;
      });
  } else {
    counts.modify(iterator, name("addressbook"), [&]( auto& row ) {
          if(type == "emplace") { row.emplaced += 1; }
          if(type == "modify") { row.modified += 1; }
          if(type == "erase") { row.erased += 1; }
      });
  }
}



EOSIO_DISPATCH(abcounter, (count))
