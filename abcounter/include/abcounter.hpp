#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT abcounter : public contract {
  public:
    using contract::contract;

    ACTION count(name user, string type);

    /**
      动作包装器
      第一个参数：指要调用的action
      第二个参数：指action函数
    */
    using count_action = action_wrapper<name("count"), &abcounter::count>;

  private:
    TABLE counter {
      name    key;
      uint64_t emplaced;
      uint64_t modified;
      uint64_t erased;
      uint64_t primary_key() const { return key.value; }
    };
    typedef multi_index<name("counts"), counter> count_index;
};
