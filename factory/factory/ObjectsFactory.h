#ifndef OBJECTS_FACTORY_H_INCLUDED
#define OBJECTS_FACTORY_H_INCLUDED

/*!
  \file ObjectsFactory.h
  \author gbug
  \brief Define a factory to create objects of arbitraty types by their string ids.
*/

#include <string>
#include <functional>
#include <utility>
#include <stdexcept>
#include <unordered_map>

template<class Factory> class ObjectsFactory;

/// Factory to create objects of arbitrary types by their string ids.
/// ...
template<class T, class... Args> class ObjectsFactory<T(Args...)>
{
  using registry = std::unordered_map<std::string, std::function<T(Args...)>>;

  inline static registry *factory_ptr = nullptr;
  static registry& factory()
  {
    if (!factory_ptr)
      factory_ptr = new registry;
    return *factory_ptr;
  }

public:
  ObjectsFactory() = delete;
  ~ObjectsFactory() = default;
  ObjectsFactory(ObjectsFactory &&) = delete;
  ObjectsFactory(const ObjectsFactory &) = delete;
  ObjectsFactory& operator=(ObjectsFactory &&) = delete;
  ObjectsFactory& operator=(const ObjectsFactory &) = delete;

  ObjectsFactory(std::string name, std::function<T(Args...)> fun)
  {
    if (factory().find(name) != factory().end())
      throw std::logic_error(
        "ObjectFactory: id \"" + name + "\" defined more than once!");
    factory().emplace(name, fun);
  }

  /// Build the object registered with label "name".
  template<class... Ts>
    static T build(const std::string &name, Ts&&... args)
  {
    static_assert(
      sizeof...(Args) == sizeof...(Ts) &&
      (std::is_same_v<Args, Ts> && ...), "arguments aren't match");

    auto it = factory().find(name);
    if (it == factory().end())
      throw std::runtime_error(
        "ObjectsFactory: id \"" + name + "\" is not registered.");
    return it->second(std::forward<Ts>(args)...);
  }

  template<class Cont> static void dump_names(Cont &cont)
  {
    for (auto f : factory())
      cont.push_back(f.first);
  }
}; // class ObjectsFactory<T(Args...)>

template<class T, class... Args>
  ObjectsFactory(std::string, std::function<T(Args...)>) -> ObjectsFactory<T(Args...)>;


#include <vector>
#include <iomanip>
#include <algorithm>

namespace IO
{

  /// Default (base) logger. Do nothing because nothing to do.
  struct FactoryLogger {};

  using FactoryLoggerF = FactoryLogger(std::ostream &out);

  /// helper function for text representation of a specific factory
  /// should be called from the factory logger's contstructor
  template<class Factory> void print_factory(
    std::ostream &message, const std::string &name, const std::string &description)
  {
    std::vector<std::string> names;
    ObjectsFactory<Factory>::dump_names(names);
    std::ranges::sort(names);

    message
      << "Factory: \"" << name << "\".\n"
      << "Description: \"" << description << "\".\n"
      << "Available object(s):\n";

    for (int counter = 0; auto &name : names)
      message << std::setw(3) << (++counter) << ". " << name << '\n';
  }

} // namespace IO

#endif // OBJECTS_FACTORY_H_INCLUDED
