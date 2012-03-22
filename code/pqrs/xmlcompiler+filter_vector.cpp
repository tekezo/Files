#include <boost/algorithm/string.hpp>
#include "pqrs/bridge.h"
#include "pqrs/vector.hpp"
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  xmlcompiler::filter_vector::filter_vector(const symbolmap& symbolmap, const boost::property_tree::ptree& pt)
  {
    for (auto it : pt) {
      /*  */ if (it.first == "not") {
        add(symbolmap, BRIDGE_FILTERTYPE_APPLICATION_NOT, "ApplicationType", it.second.data());
      } else if (it.first == "only") {
        add(symbolmap, BRIDGE_FILTERTYPE_APPLICATION_ONLY, "ApplicationType", it.second.data());
      } else if (it.first == "device_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_DEVICE_NOT, "", it.second.data());
      } else if (it.first == "device_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_DEVICE_ONLY, "", it.second.data());
      } else if (it.first == "config_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_CONFIG_NOT, "ConfigIndex", it.second.data());
      } else if (it.first == "config_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_CONFIG_ONLY, "ConfigIndex", it.second.data());
      } else if (it.first == "modifier_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_MODIFIER_NOT, "", it.second.data());
      } else if (it.first == "modifier_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_MODIFIER_ONLY, "", it.second.data());
      } else if (it.first == "inputmode_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODE_NOT, "InputMode", it.second.data());
      } else if (it.first == "inputmode_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODE_ONLY, "InputMode", it.second.data());
      } else if (it.first == "inputmodedetail_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODEDETAIL_NOT, "InputModeDetail", it.second.data());
      } else if (it.first == "inputmodedetail_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODEDETAIL_ONLY, "InputModeDetail", it.second.data());
      }
    }
  }

  const std::vector<uint32_t>&
  xmlcompiler::filter_vector::get(void) const
  {
    return data_;
  }

  void
  xmlcompiler::filter_vector::add(const symbolmap& symbolmap,
                                  uint32_t filter_type,
                                  const std::string& type,
                                  const std::string& string)
  {
    std::vector<std::string> splits;
    boost::split(splits, string, boost::is_any_of(","), boost::token_compress_on);

    // trim values
    for (auto value : splits) {
      boost::trim(s);
    }
    pqrs::vector::remove_empty_strings(split);

    data_.push_back(split.size() + 1);         // +1 == filter_type
    data_.push_back(filter_type);

    for (auto value : splits) {
      // support '|' for <modifier_only>.
      // For example: <modifier_only>ModifierFlag::COMMAND_L|ModifierFlag::CONTROL_L, ModifierFlag::COMMAND_L|ModifierFlag::OPTION_L</modifier_only>
      std::vector<std::string> items;
      boost::split(items, value, boost::is_any_of("|"), boost::token_compress_on);

      uint32_t filter_value;
      for (auto i : items) {
        boost::trim(i);
        std::vector<std::string> type_name;
        boost::iter_split(type_name, i, boost::algorithm::first_finder("::"));

        std::string symbol_type;
        std::string symbol_name;
        if (type_name.size() == 2) {
          symbol_type = type_name[0];
          symbol_name = type_name[1];
        } else {
          symbol_type = type;
          symbol_name = type_name[0];
        }

        boost::optional v = symbolmap.get(symbol_type, symbol_name);
        if (! v) {
          throw xmlcompiler_runtime_error(std::string("Unknown Variable: " + symbol_type + "::" + symbol_name));
        }
        filter_value |= *v;
      }
      data_.push_back(filter_value);
    }
  }
}
