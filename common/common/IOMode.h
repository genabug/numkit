#ifndef IO_MODE_H_INCLUDED
#define IO_MODE_H_INCLUDED

/*!
  \file IOMode.h
  \authors boldar, gbug
  \brief IO manipulators and helpers for array-like types (Vector, Tensor, etc.)
*/

#include <cctype>
#include <locale>
#include <istream>
#include <ostream>
#include <ranges>

namespace IO
{
  class Mode final : public std::locale::facet
  {
    mutable bool brackets = true;

  public:
    static std::locale::id id;

    bool use_brackets() const { return brackets; }
    void use_brackets(bool flag) const { brackets = flag; }
  };

  inline std::locale::id Mode::id;

  inline bool use_brackets(std::ostream &out)
  {
    const std::locale &loc = out.getloc();
    return !std::has_facet<Mode>(loc) || std::use_facet<Mode>(loc).use_brackets();
  }

  inline std::ios_base& bareComps(std::ios_base &stream)
  {
    std::locale loc = stream.getloc();
    if (!std::has_facet<Mode>(loc))
      stream.imbue(std::locale(loc, new Mode));
    std::use_facet<Mode>(stream.getloc()).use_brackets(false);
    return stream;
  }

  inline std::ios_base& inBrackets(std::ios_base &stream)
  {
    std::locale loc = stream.getloc();
    if (!std::has_facet<Mode>(loc))
      stream.imbue(std::locale(loc, new Mode));
    std::use_facet<Mode>(stream.getloc()).use_brackets(true);
    return stream;
  }

  template<std::ranges::range R>
    void write_values(std::ostream &out, const R &data, char open, char close)
  {
    bool in_brackets = use_brackets(out);
    if (in_brackets)
      out << open;

    auto it = data.begin();
    if (it != data.end())
      out << *(it++);
    for (; it != data.end(); ++it)
      out << (in_brackets? ", " : " ") << *it;

    if (in_brackets)
      out << close;
  }

  template<std::ranges::range R>
    void read_values(std::istream &in, R &data, char open, char close)
  {
    in >> std::ws;
    bool in_brackets = (in.peek() == open);
    if (in_brackets)
      in.get();

    const auto size = std::ranges::size(data);
    for (size_t i = 0; auto &value : data)
    {
      if (!(in >> value))
        return;
      if (in_brackets)
      {
        ++i;
        if (int c = in.get(); (c != ',' && i != size) || (c != close && i == size))
        {
          in.setstate(std::ios::failbit);
          return;
        }
      }
    }
  }

} // namespace IO

#endif // IO_MODE_H_INCLUDED
