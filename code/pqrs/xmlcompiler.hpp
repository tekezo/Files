#ifndef PQRS_XMLCOMPILER_HPP
#define PQRS_XMLCOMPILER_HPP

#include <vector>
#include <tr1/memory>
#include <boost/property_tree/ptree.hpp>
#include "pqrs/string.hpp"

namespace pqrs {
  class xmlcompiler {
  public:
    xmlcompiler(void);

    bool reload(void);

    class appdef {
    public:
      const std::string& get_name(void)   { return name_; }
      void set_name(const std::string& v) { name_ = v; }
      void add_rule_equal(const std::string& v);
      void add_rule_prefix(const std::string& v);
      bool is_rules_matched(const std::string& identifier);

    private:
      std::string name_;
      std::vector<std::string> rules_equal_;
      std::vector<std::string> rules_prefix_;
    };

  private:
    bool read_xml(const char* xmlfilepath, boost::property_tree::ptree& pt, bool with_replacement);

    bool reload_replacementdef_(void);
    void traverse_replacementdef_(const boost::property_tree::ptree& pt);

    bool reload_appdef_(void);
    void traverse_appdef_(const boost::property_tree::ptree& pt);

    std::string errormessage_;
    pqrs::string::replacement replacement_;

    std::vector<std::tr1::shared_ptr<appdef> > app_;
  };
}

#endif
