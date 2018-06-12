#include <string>
#include <ctime>
#include <vector>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <memory>

using boost::property_tree::json_parser_error;
using boost::property_tree::ptree;

enum return_val
{
  none = -1,
  right = 1,
  error = 0,
  read_only = 0,
  read_write = 1,
};

class Simple_task : public std::enable_shared_from_this<Simple_task>
{
protected:
  std::string name;
  std::string description;
  time_t start_time{0};
  time_t deadline{0};
  bool status{false};
  int id{0};

public:
  void set_description(std::string new_description) { description = new_description; }
  void set_name(std::string new_name) { name = new_name; }
  void set_start_time(time_t new_start_time) { start_time = new_start_time; }
  void set_deadline(time_t new_deadline) { deadline = new_deadline; }
  void set_done() { status = true; }
  void set_undone() { status = false; }
  void set_id(int new_id) { id = new_id; }

  std::string get_name() { return name; }
  std::string get_description() { return description; }
  time_t get_start_time() { return start_time; }
  time_t get_deadline() { return deadline; }
  int get_id() { return id; }
  virtual std::pair<int, int> get_status() { return std::pair<int, int>(status, true); }

  virtual void add_sub_task(int id) {}
  virtual int pop_sub_task(int id) { return return_val::none; }
  virtual std::vector<int> remove_sub_tasks() { return std::vector<int>(); }
  virtual std::vector<int> get_sub_tasks() { return std::vector<int>(); }
  virtual int to_json(ptree &pt);
};

class Complex_task : public Simple_task
{
public:
  std::vector<int> sub_tasks;

  void add_sub_task(int id);
  int pop_sub_task(int id);
  std::vector<int> remove_sub_tasks();
  std::vector<int> get_sub_tasks() { return sub_tasks; }
  std::pair<int, int> get_status();
  int to_json(ptree &pt);
};

class Desk : public Complex_task
{
private:
public:
  std::string git_path;
  int desk_from_json(std::string path); //load desk to memory from json
  int to_json(ptree &pt);               //write desk to json
};

class User
{
private:
  std::string username;
  std::string password;
  std::map<int, int> perm_desks; //the ratio between username and permission for desks id
public:
  User(std::string username,
       std::string password) : username(username), password(password) {}

  std::string get_name() { return username; }
  std::string get_password() { return password; }
  int get_permission(int id);
  std::vector<int> get_desks();

  void set_name(std::string new_username) { username = new_username; }
  void set_password(std::string new_password) { password = new_password; }
  void set_perm(int id, int flag) { perm_desks[id] = flag; }
  int pop_perm(int id);

  int to_json();
  int from_json();
};

class logic //some logic api for server
{
private:
  logic() {}
  logic(logic const &) = delete;
  logic &operator=(logic const &) = delete;

  template <typename T>
  int create_task(int id);
  std::map<int, std::shared_ptr<Simple_task>> loaded_users_desks;

  friend std::vector<int> Complex_task::remove_sub_tasks();
  friend std::pair<int, int> Complex_task::get_status();

public:
  static logic &get_instance();

  std::vector<int> load_desks(User user); //load desks to memory
  int write_desks();                      //write to file
  std::pair<int, int> get_status(int id); // return status for desk
  int create_simple_task(int id) { return create_task<Simple_task>(id); }
  int create_complex_task(int id) { return create_task<Complex_task>(id); }
  int create_desk(int id) { return create_task<Desk>(id); }
  int remove_task(int id);
  std::shared_ptr<Simple_task> get_task(int id);
};

//methods of classes
//User
std::vector<int> User::get_desks()
{
  std::vector<int> res;
  for (auto it : perm_desks)
  {
    res.push_back(it.first);
  }
  return res;
}

int User::get_permission(int id)
{
  auto it = perm_desks.find(id);
  return it != perm_desks.end() ? perm_desks[id] : return_val::none;
}

int User::pop_perm(int id)
{
  auto elem = perm_desks.find(id);
  if (elem != perm_desks.end())
  {
    perm_desks.erase(elem);
    return return_val::right;
  }
  return return_val::error;
}

int User::to_json()
{
  ptree pt;

  for (auto it : perm_desks)
  {
    pt.put(std::to_string(it.first), std::to_string(it.second));
  }
  write_json(username + "_" + password, pt);
  return return_val::right;
}

int User::from_json()
{
  ptree pt;
  try
  {
    read_json(username + "_" + password, pt);

    for (auto sub : pt)
    {
      set_perm(std::stoi(sub.first), sub.second.get_value<int>());
    }
    return return_val::right;
  }
  catch (const json_parser_error &jpe)
  {
    return return_val::error;
  }
}

//logic
logic &logic::get_instance()
{
  static logic l;
  return l;
}

std::shared_ptr<Simple_task> logic::get_task(int id)
{
  auto elem = logic::loaded_users_desks.find(id);
  if (elem != logic::loaded_users_desks.end())
  {
    return elem->second;
  }
  return std::shared_ptr<Simple_task>(nullptr);
}

int logic::write_desks()
{

  for (auto it : loaded_users_desks)
  {
    ptree pt;
    it.second->to_json(pt);
    write_json(std::to_string(it.first), pt);
  }
  return return_val::right;
}

std::vector<int> logic::load_desks(User user)
{
  ptree pt;
  std::vector<int> idxs = user.get_desks();
  std::vector<int> loaded;
  for (auto id : idxs)
  {
    if (loaded_users_desks.find(id) == loaded_users_desks.end())
    {
      try
      {
        read_json(std::to_string(id), pt);
        std::string type = pt.get<std::string>("type");
        std::shared_ptr<Simple_task> task;
        if (type == "Simple_task")
        {
          create_simple_task(id);
        }
        if (type == "Complex_task")
        {
          create_complex_task(id);
        }
        if (type == "Desk")
        {
          create_desk(id);
        }
        task = get_task(id);
        task->set_name(pt.get<std::string>("name"));
        task->set_description(pt.get<std::string>("description"));
        task->set_deadline(pt.get<time_t>("deadline"));
        task->set_start_time(pt.get<time_t>("start_time"));
        if (type != "Simple_task")
        {
          ptree arr = pt.get_child("sub_tasks");
          for (auto sub : arr)
          {
            task->add_sub_task(std::stoi(sub.first));
          }
        }
        if (type == "Desk")
        {
          //something whith path
        }
        loaded.push_back(id);
      }
      catch (const json_parser_error &jpe)
      {
        user.pop_perm(id);
      }
    }
  }
  return loaded;
}

std::pair<int, int> logic::get_status(int id)
{
  auto elem = logic::loaded_users_desks.find(id);
  if (elem != logic::loaded_users_desks.end())
  {
    return elem->second->get_status();
  }
  else
  {
    return std::pair<int, int>(-1, -1); //desk didn't found
  }
}

template <typename T>
int logic::create_task(int id)
{
  std::shared_ptr<Simple_task> s(new T);
  s->set_id(id);
  loaded_users_desks[id] = s;
  return id;
}

int logic::remove_task(int id)
{
  auto elem = loaded_users_desks.find(id);
  if (elem != loaded_users_desks.end())
  {
    elem->second->remove_sub_tasks();
    if (remove(std::to_string(elem->second->get_id()).c_str()))
    {
      loaded_users_desks.erase(elem);
      return return_val::right;
    }
  }
  return return_val::error; //desk didn't found
}

//Desk
int Desk::to_json(ptree &pt)
{
  Complex_task::to_json(pt);
  pt.put(std::to_string(id) + ".type", "Desk");
  return return_val::right;
}

//Complex_task
void Complex_task::add_sub_task(int id)
{
  sub_tasks.push_back(id);
}

std::vector<int> Complex_task::remove_sub_tasks()
{
  std::vector<int> popped;
  for (auto id : sub_tasks)
  {
    auto elem = logic::get_instance().loaded_users_desks.find(id);
    if (elem != logic::get_instance().loaded_users_desks.end())
    {
      elem->second->remove_sub_tasks();
      if (remove(std::to_string(elem->second->get_id()).c_str()))
      {
        logic::get_instance().loaded_users_desks.erase(elem);
        popped.push_back(id);
      }
    }
  }
  return popped;
}

int Complex_task::pop_sub_task(int id)
{
  auto elem = sub_tasks.begin();
  for (; elem != sub_tasks.end(); elem++)
  {
    if ((*elem) == id)
    {
      sub_tasks.erase(elem);
      return return_val::right;
    }
  }
  return return_val::error;
}

std::pair<int, int> Complex_task::get_status()
{
  std::pair<int, int> status(0, 0);
  for (auto id_task : sub_tasks)
  {
    std::pair<int, int> sub_status(0, 0);

    auto elem = logic::get_instance().loaded_users_desks.find(id_task);
    if (elem != logic::get_instance().loaded_users_desks.end())
    {
      status.second++;
      sub_status = elem->second->get_status();
    }
    if (sub_status.first == sub_status.second && sub_status.second != 0)
      status.first++;
  }
  return status;
}

int Complex_task::to_json(ptree &pt)
{
  Simple_task::to_json(pt);
  pt.put(std::to_string(id) + ".type", "Complex_task");
  ptree sub_array;
  for (auto it : sub_tasks)
  {
    sub_array.put("", it);
  }
  pt.add_child(std::to_string(id) + ".sub_tasks", sub_array);

  return return_val::right;
}

//Simple_task

int Simple_task::to_json(ptree &pt)
{
  std::string sid(std::to_string(id));
  pt.put(sid + ".type", "Simple_task");
  pt.put(sid + ".name", name);
  pt.put(sid + ".start_time", start_time);
  pt.put(sid + ".deadline", deadline);
  pt.put(sid + ".status", status);
  return return_val::right;
}