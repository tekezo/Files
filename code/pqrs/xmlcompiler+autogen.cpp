#include <exception>
#include <boost/algorithm/string.hpp>
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  bool
  xmlcompiler::reload_autogen_(void)
  {
    bool retval = false;

    confignamemap_.clear();
    remapclasses_initialize_vector_.clear();

    const char* paths[] = {
      "/Users/tekezo/Library/Application Support/KeyRemap4MacBook/private.xml",
      "/Library/org.pqrs/KeyRemap4MacBook/prefpane/checkbox.xml",
    };
    for (auto xmlfilepath : paths) {
      boost::property_tree::ptree pt;
      if (! pqrs::xmlcompiler::read_xml_(xmlfilepath, pt, true)) {
        continue;
      }

      // add_configindex_and_keycode_to_symbolmap_
      //   1st loop: <identifier>notsave.*</identifier>
      //   2nd loop: other <identifier>
      //
      // We need to assign higher priority to notsave.* settings.
      // So, adding configindex by 2steps.
      add_configindex_and_keycode_to_symbolmap_(pt, true);
      add_configindex_and_keycode_to_symbolmap_(pt, false);

      traverse_autogen_(pt);

      // Set retval to true if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide devicedef.xml is not loaded.)
      retval = true;
    }

    return retval;
  }

  void
  xmlcompiler::add_configindex_and_keycode_to_symbolmap_(const boost::property_tree::ptree& pt, bool handle_notsave)
  {
    for (auto it : pt) {
      if (it.first != "identifier") {
        add_configindex_and_keycode_to_symbolmap_(it.second, handle_notsave);
      } else {
        auto identifier = boost::trim_copy(it.second.data());
        normalize_identifier(identifier);

        // ----------------------------------------
        // Do not treat essentials.
        auto attr_essential = it.second.get_optional<std::string>("<xmlattr>.essential");
        if (attr_essential) {
          continue;
        }

        // ----------------------------------------
        if (handle_notsave != boost::starts_with(identifier, "notsave_")) {
          continue;
        }

        // ----------------------------------------
        auto attr_vk_config = it.second.get_optional<std::string>("<xmlattr>.vk_config");
        if (attr_vk_config) {
          const char* names[] = {
            "VK_CONFIG_TOGGLE_",
            "VK_CONFIG_FORCE_ON_",
            "VK_CONFIG_FORCE_OFF_",
            "VK_CONFIG_SYNC_KEYDOWNUP_",
          };
          for (auto n : names) {
            symbolmap_keycode_.add("KeyCode", std::string(n) + identifier);
          }
        }

        // ----------------------------------------
        symbolmap_keycode_.add("ConfigIndex", identifier);
      }
    }
  }

  void
  xmlcompiler::traverse_autogen_(const boost::property_tree::ptree& pt)
  {
  }
}
