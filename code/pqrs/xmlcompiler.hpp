#ifndef PQRS_XMLCOMPILER_HPP
#define PQRS_XMLCOMPILER_HPP

#include "pqrs/string.hpp"
#include <boost/property_tree/ptree.hpp>

namespace pqrs {
  class xmlcompiler {
  public:
    xmlcompiler(void);

    bool reload(void);

  private:
    bool read_xml(const char* xmlfilepath, boost::property_tree::ptree& pt, bool with_replacement);

    bool reload_replacementdef_(void);
    void traverse_replacementdef_(const boost::property_tree::ptree& pt);

    pqrs::string::replacement replacement_;
    std::string errormessage_;
  };
}

#endif
