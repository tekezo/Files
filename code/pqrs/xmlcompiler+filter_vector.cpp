#include <boost/algorithm/string.hpp>
#include "pqrs/bridge.h"
#include "pqrs/vector.hpp"
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  xmlcompiler::filter_vector::filter_vector(const symbolmap& symbolmap, const boost::property_tree::ptree& pt)
  {
    for (auto it : pt) {
      /*  */ if (it.first == "not") {
        add(symbolmap, BRIDGE_FILTERTYPE_APPLICATION_NOT,  "ApplicationType::", it.second.data());
      } else if (it.first == "only") {
        add(symbolmap, BRIDGE_FILTERTYPE_APPLICATION_ONLY, "ApplicationType::", it.second.data());
      } else if (it.first == "device_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_DEVICE_NOT,  "", it.second.data());
      } else if (it.first == "device_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_DEVICE_ONLY, "", it.second.data());
      } else if (it.first == "config_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_CONFIG_NOT,  "ConfigIndex::", it.second.data());
      } else if (it.first == "config_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_CONFIG_ONLY, "ConfigIndex::", it.second.data());
      } else if (it.first == "modifier_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_MODIFIER_NOT,  "", it.second.data());
      } else if (it.first == "modifier_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_MODIFIER_ONLY, "", it.second.data());
      } else if (it.first == "inputmode_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODE_NOT,  "InputMode::", it.second.data());
      } else if (it.first == "inputmode_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODE_ONLY, "InputMode::", it.second.data());
      } else if (it.first == "inputmodedetail_not") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODEDETAIL_NOT,  "InputModeDetail::", it.second.data());
      } else if (it.first == "inputmodedetail_only") {
        add(symbolmap, BRIDGE_FILTERTYPE_INPUTMODEDETAIL_ONLY, "InputModeDetail::", it.second.data());
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
                                  const std::string& prefix,
                                  const std::string& string)
  {
    std::vector<std::string> splits;
    boost::split(splits, string, boost::is_any_of(","), boost::token_compress_on);

    // trim values
    for (auto value : splits) {
      boost::trim(value);
    }
    pqrs::vector::remove_empty_strings(splits);

    data_.push_back(splits.size() + 1);         // +1 == filter_type
    data_.push_back(filter_type);

    for (auto value : splits) {
      // support '|' for <modifier_only>.
      // For example: <modifier_only>ModifierFlag::COMMAND_L|ModifierFlag::CONTROL_L, ModifierFlag::COMMAND_L|ModifierFlag::OPTION_L</modifier_only>
      std::vector<std::string> items;
      boost::split(items, value, boost::is_any_of("|"), boost::token_compress_on);

      uint32_t filter_value = 0;
      for (auto i : items) {
        boost::trim(i);
        auto v = symbolmap.get(prefix + i);
        if (! v) {
          throw xmlcompiler_runtime_error(std::string("Unknown Variable: " + prefix + i));
        }
        filter_value |= *v;
      }
      data_.push_back(filter_value);
    }
  }
}
