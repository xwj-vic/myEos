#include <eosio/eosio.hpp>


using namespace std;
using namespace eosio;

CONTRACT addressbook : public contract {
  public:
    using contract::contract;

    /** 
      声明两个action: upsert和deleteuser，具体实现将在cpp文件中 
      ACTION：是一个标签，一定要加，声明是一个action
    */

    ACTION upsert(name user, string first_name, string last_name, uint64_t age, string street, string city, string state);

    ACTION deleteuser(name user);

    /**
        内联动作，
        例子：添加一个通知操作，发送msg给指定user
    */
    ACTION notify(name user, string msg);

  private:

    /** 
      TABLE：标签，声明这是一张table
    */
    TABLE person {  //定义一张table，用于存储data
      name key;
      string first_name;
      string last_name;
      uint64_t age;
      string street;
      string city;
      string state;
      auto primary_key() const { return key.value; }  //定义主键
      uint64_t get_secondary_1() const { return age; } //  二级索引，（必须是数字字段）
    };

    /**
      设置为多索引
      name("people")  命名为people 
      person  传入上面定义的表结构
      address_index 类型将用于实例化表
      indexed_by<name("byage"), const_mem_fun<person, uint64_t, &person::get_secondary_1>> : 二级索引

        name("byage"): 第一个参数二级索引名称为byage
        uint64_t:第二个类型参数指定为函数调用运算符
        &person::get_secondary_1：该函数将提取const值作为索引键。在这种情况下，我们将其指向我们先前创建的getter
    */
    typedef multi_index<name("people"), person, indexed_by<name("byage"), const_mem_fun<person, uint64_t, &person::get_secondary_1>>> address_index;


    /**
      私有方法
      内联操作，调用notify这个action,将msg发送出去
      action(
            permission_level: 权限
            code: 所调用的合约，这里调用的是自身合约，所以使用get_self()
            action: 需要调用的action
            data: 发送的数据
          );  
      send():发送
    */
    void send_summary(name user, string message) {
        action(
          permission_level{get_self(),name("active") },
          get_self(),
          name("notify"),
          std::make_tuple(user, name{user}.to_string() + message)).send();
    }


    /**
      对外部合同的内联操作
    */
    void increment_counter(name user, string type) {
        action(
        permission_level{get_self(),name("active")},
        name("abcounter"),
        name("count"),
        std::make_tuple(user, type)).send();
    }
};
