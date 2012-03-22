#include <algorithm>
#include <exception>
#include <boost/algorithm/string.hpp>
#include "pqrs/xmlcompiler.hpp"
#include "pqrs/bridge.h"
#include "pqrs/vector.hpp"

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
    std::vector<std::tr1::shared_ptr<boost::property_tree::ptree> > pts;

    for (auto& xmlfilepath : paths) {
      std::tr1::shared_ptr<boost::property_tree::ptree> ptr(new boost::property_tree::ptree());
      if (! ptr) {
        continue;
      }
      pts.push_back(ptr);

      if (! pqrs::xmlcompiler::read_xml_(xmlfilepath, *ptr, true)) {
        continue;
      }
    }

    for (auto& ptr : pts) {
      // add_configindex_and_keycode_to_symbolmap_
      //   1st loop: <identifier>notsave.*</identifier>
      //   2nd loop: other <identifier>
      //
      // We need to assign higher priority to notsave.* settings.
      // So, adding configindex by 2steps.
      add_configindex_and_keycode_to_symbolmap_(*ptr, true);
      add_configindex_and_keycode_to_symbolmap_(*ptr, false);
    }

    for (auto& ptr : pts) {
      traverse_identifier_(*ptr);

      // Set retval to true if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide devicedef.xml is not loaded.)
      retval = true;
    }

    remapclasses_initialize_vector_.freeze();

    return retval;
  }

  void
  xmlcompiler::add_configindex_and_keycode_to_symbolmap_(const boost::property_tree::ptree& pt, bool handle_notsave)
  {
    for (auto& it : pt) {
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
          for (auto& n : names) {
            symbolmap_.add("KeyCode", std::string(n) + identifier);
          }
        }

        // ----------------------------------------
        symbolmap_.add("ConfigIndex", identifier);
      }
    }
  }

  void
  xmlcompiler::traverse_identifier_(const boost::property_tree::ptree& pt)
  {
    for (auto& it : pt) {
      if (it.first != "identifier") {
        traverse_identifier_(it.second);

      } else {
        auto attr_essential = it.second.get_optional<std::string>("<xmlattr>.essential");
        if (attr_essential) {
          continue;
        }

        std::vector<uint32_t> initialize_vector;
        auto raw_identifier = boost::trim_copy(it.second.data());
        auto identifier = raw_identifier;
        normalize_identifier(identifier);

        auto attr_vk_config = it.second.get_optional<std::string>("<xmlattr>.essential");
        if (attr_vk_config) {
          initialize_vector.push_back(5); // count
          initialize_vector.push_back(BRIDGE_VK_CONFIG);

          const char* names[] = {
            "VK_CONFIG_TOGGLE_",
            "VK_CONFIG_FORCE_ON_",
            "VK_CONFIG_FORCE_OFF_",
            "VK_CONFIG_SYNC_KEYDOWNUP_",
          };
          for (auto& n : names) {
            auto v = symbolmap_.get("KeyCode", std::string(n) + identifier);
            if (! v) {
              throw xmlcompiler_runtime_error(std::string(n) + " is not found in symbolmap.");
            }
            initialize_vector.push_back(*v);
          }
        }

        filter_vector fv;
        traverse_autogen_(pt, identifier, fv, initialize_vector);

        uint32_t configindex = *(symbolmap_.get("ConfigIndex", identifier));
        remapclasses_initialize_vector_.add(initialize_vector, configindex);
        confignamemap_[configindex] = raw_identifier;
      }
    }
  }

  void
  xmlcompiler::traverse_autogen_(const boost::property_tree::ptree& pt,
                                 const std::string& identifier,
                                 const filter_vector& parent_filter_vector,
                                 std::vector<uint32_t>& initialize_vector)
  {
    filter_vector fv(symbolmap_, pt);

    // Add passthrough filter.
    if (parent_filter_vector.empty() &&
        ! boost::starts_with(identifier, "passthrough_")) {
      auto v = symbolmap_.get("ConfigIndex::notsave_passthrough");
      if (! v) {
        throw xmlcompiler_logic_error("ConfigIndex::notsave_passthrough is not found in symbolmap.");
      }

      std::vector<uint32_t>& vec = fv.get();
      vec.push_back(2); // count
      vec.push_back(BRIDGE_FILTERTYPE_CONFIG_NOT);
      vec.push_back(*v);
    }

    // Add parent filters.
    pqrs::vector::push_back(fv.get(), parent_filter_vector.get());

    // ----------------------------------------
    for (auto& it : pt) {
      if (it.first == "autogen") {
        std::string autogen = boost::trim_copy(it.second.data());

        // drop whitespaces for preprocessor. (for FROMKEYCODE_HOME, etc)
        // Note: preserve space when --ShowStatusMessage--.
        if (! boost::starts_with(autogen, "--ShowStatusMessage--")) {
          pqrs::string::remove_whitespaces(autogen);
        }

        // [self handle_autogen : initialize_vector filtervec : filtervec autogen_text : autogen_text];
      }

      traverse_autogen_(it.second, identifier, fv, initialize_vector);
    }
  }
}

#if 0
// ======================================================================
// autogen

-(NSMutableArray*) combination : (NSArray*)input
{
  if ([input count] == 0) {
    NSMutableArray* a = [[NSMutableArray new] autorelease];
    return [NSMutableArray arrayWithObject : a];
  }

  id last = [input lastObject];

  NSRange range;
  range.location = 0;
  range.length = [input count] - 1;
  NSMutableArray* subarray = [self combination:[input subarrayWithRange:range]];

  NSMutableArray* newarray = [NSMutableArray arrayWithArray:subarray];
  for (NSMutableArray* a in subarray) {
    [newarray addObject :[a arrayByAddingObject : last]];
  }
  return newarray;
}

-(NSString*) getextrakey : (NSString*)keyname
{
  if ([keyname isEqualToString : @ "HOME"])           { return @ "CURSOR_LEFT";  }
  if ([keyname isEqualToString : @ "END"])            { return @ "CURSOR_RIGHT"; }
  if ([keyname isEqualToString : @ "PAGEUP"])         { return @ "CURSOR_UP";    }
  if ([keyname isEqualToString : @ "PAGEDOWN"])       { return @ "CURSOR_DOWN";  }
  if ([keyname isEqualToString : @ "FORWARD_DELETE"]) { return @ "DELETE";       }
  return @ "";
}

-(void) append_to_initialize_vector : (NSMutableArray*)initialize_vector filtervec : (NSArray*)filtervec params : (NSString*)params type : (unsigned int)type
{
  NSMutableArray* args = [[NSMutableArray new] autorelease];
  [args addObject :[NSNumber numberWithUnsignedInt : type]];

  if ([params length] > 0) {
    for (NSString* p in [params componentsSeparatedByString : @ ","]) {
      unsigned int datatype = 0;
      unsigned int newvalue = 0;
      for (NSString* value in [p componentsSeparatedByString : @ "|"]) {
        unsigned int newdatatype = 0;
        /*  */ if ([value hasPrefix : @ "KeyCode::"]) {
          newdatatype = BRIDGE_DATATYPE_KEYCODE;
        } else if ([value hasPrefix : @ "ModifierFlag::"]) {
          newdatatype = BRIDGE_DATATYPE_FLAGS;
        } else if ([value hasPrefix : @ "ConsumerKeyCode::"]) {
          newdatatype = BRIDGE_DATATYPE_CONSUMERKEYCODE;
        } else if ([value hasPrefix : @ "PointingButton::"]) {
          newdatatype = BRIDGE_DATATYPE_POINTINGBUTTON;
        } else if ([value hasPrefix : @ "ScrollWheel::"]) {
          newdatatype = BRIDGE_DATATYPE_SCROLLWHEEL;
        } else if ([value hasPrefix : @ "KeyboardType::"]) {
          newdatatype = BRIDGE_DATATYPE_KEYBOARDTYPE;
        } else if ([value hasPrefix : @ "DeviceVendor::"]) {
          newdatatype = BRIDGE_DATATYPE_DEVICEVENDOR;
        } else if ([value hasPrefix : @ "DeviceProduct::"]) {
          newdatatype = BRIDGE_DATATYPE_DEVICEPRODUCT;
        } else if ([value hasPrefix : @ "Option::"]) {
          newdatatype = BRIDGE_DATATYPE_OPTION;
        } else {
          @throw [NSException exceptionWithName : @ "<autogen> error" reason :[NSString stringWithFormat : @ "unknown datatype: %@ (%@)", value, params] userInfo : nil];
        }

        if (datatype && datatype != newdatatype) {
          // Don't connect different data type. (Example: KeyCode::A | ModifierFlag::SHIFT_L)
          @throw [NSException exceptionWithName : @ "<autogen> error" reason :[NSString stringWithFormat : @ "invalid connect(|): %@", params] userInfo : nil];
        }

        datatype = newdatatype;
        newvalue |= [keycode_ unsignedIntValue:value];
      }

      [args addObject :[NSNumber numberWithUnsignedInt : datatype]];
      [args addObject :[NSNumber numberWithUnsignedInt : newvalue]];
    }
  }

  [initialize_vector addObject :[NSNumber numberWithUnsignedInteger :[args count]]];
  [initialize_vector addObjectsFromArray : args];

  if ([filtervec count] > 0) {
    [initialize_vector addObjectsFromArray : filtervec];
  }
}

-(void) handle_autogen : (NSMutableArray*)initialize_vector filtervec : (NSArray*)filtervec autogen_text : (NSString*)autogen_text
{
  // ------------------------------------------------------------
  // preprocess
  //
  for (NSString* modifier in [NSArray arrayWithObjects : @ "COMMAND", @ "CONTROL", @ "SHIFT", @ "OPTION", nil]) {
    NSString* symbol = [NSString stringWithFormat : @ "VK_%@", modifier];
    if ([autogen_text rangeOfString : symbol].location != NSNotFound) {
      [self handle_autogen : initialize_vector filtervec : filtervec
       autogen_text :[autogen_text stringByReplacingOccurrencesOfString : symbol withString :[NSString stringWithFormat : @ "ModifierFlag::%@_L", modifier]]];
      [self handle_autogen : initialize_vector filtervec : filtervec
       autogen_text :[autogen_text stringByReplacingOccurrencesOfString : symbol withString :[NSString stringWithFormat : @ "ModifierFlag::%@_R", modifier]]];
      return;
    }
  }

  if ([autogen_text rangeOfString : @ "VK_MOD_CCOS_L"].location != NSNotFound) {
    autogen_text = [autogen_text stringByReplacingOccurrencesOfString : @ "VK_MOD_CCOS_L" withString : @ "ModifierFlag::COMMAND_L|ModifierFlag::CONTROL_L|ModifierFlag::OPTION_L|ModifierFlag::SHIFT_L"];
    [self handle_autogen : initialize_vector filtervec : filtervec autogen_text : autogen_text];
    return;
  }

  if ([autogen_text rangeOfString : @ "VK_MOD_CCS_L"].location != NSNotFound) {
    autogen_text = [autogen_text stringByReplacingOccurrencesOfString : @ "VK_MOD_CCS_L" withString : @ "ModifierFlag::COMMAND_L|ModifierFlag::CONTROL_L|ModifierFlag::SHIFT_L"];
    [self handle_autogen : initialize_vector filtervec : filtervec autogen_text : autogen_text];
    return;
  }

  if ([autogen_text rangeOfString : @ "VK_MOD_CCO_L"].location != NSNotFound) {
    autogen_text = [autogen_text stringByReplacingOccurrencesOfString : @ "VK_MOD_CCO_L" withString : @ "ModifierFlag::COMMAND_L|ModifierFlag::CONTROL_L|ModifierFlag::OPTION_L"];
    [self handle_autogen : initialize_vector filtervec : filtervec autogen_text : autogen_text];
    return;
  }

  if ([autogen_text rangeOfString : @ "VK_MOD_ANY"].location != NSNotFound) {
    // to reduce combination, we ignore same modifier combination such as (COMMAND_L | COMMAND_R).
    NSMutableArray* combination = [self combination :[NSArray arrayWithObjects : @ "VK_COMMAND", @ "VK_CONTROL", @ "ModifierFlag::FN", @ "VK_OPTION", @ "VK_SHIFT", nil]];
    for (NSMutableArray* a in combination) {
      [self handle_autogen : initialize_vector filtervec : filtervec
       autogen_text :[autogen_text stringByReplacingOccurrencesOfString : @ "VK_MOD_ANY" withString :[[a arrayByAddingObject : @ "ModifierFlag::NONE"] componentsJoinedByString : @ "|"]]];
    }
    return;
  }

  for (NSString* keyname in [NSArray arrayWithObjects : @ "HOME", @ "END", @ "PAGEUP", @ "PAGEDOWN", @ "FORWARD_DELETE", nil]) {
    if ([autogen_text rangeOfString :[NSString stringWithFormat : @ "FROMKEYCODE_%@,ModifierFlag::", keyname]].location != NSNotFound) {
      [self handle_autogen : initialize_vector filtervec : filtervec
       autogen_text :[autogen_text stringByReplacingOccurrencesOfString :[NSString stringWithFormat : @ "FROMKEYCODE_%@", keyname]
                      withString :[NSString stringWithFormat : @ "KeyCode::%@", keyname]]];
      [self handle_autogen : initialize_vector filtervec : filtervec
       autogen_text :[autogen_text stringByReplacingOccurrencesOfString :[NSString stringWithFormat : @ "FROMKEYCODE_%@,", keyname]
                      withString :[NSString stringWithFormat : @ "KeyCode::%@,ModifierFlag::FN|", [self getextrakey : keyname]]]];
      return;
    }

    if ([autogen_text rangeOfString :[NSString stringWithFormat : @ "FROMKEYCODE_%@", keyname]].location != NSNotFound) {
      [self handle_autogen : initialize_vector filtervec : filtervec
       autogen_text :[autogen_text stringByReplacingOccurrencesOfString :[NSString stringWithFormat : @ "FROMKEYCODE_%@", keyname]
                      withString :[NSString stringWithFormat : @ "KeyCode::%@", keyname]]];
      [self handle_autogen : initialize_vector filtervec : filtervec
       autogen_text :[autogen_text stringByReplacingOccurrencesOfString :[NSString stringWithFormat : @ "FROMKEYCODE_%@", keyname]
                      withString :[NSString stringWithFormat : @ "KeyCode::%@,ModifierFlag::FN", [self getextrakey : keyname]]]];
      return;
    }
  }

  if ([autogen_text rangeOfString : @ "--KeyOverlaidModifierWithRepeat--"].location != NSNotFound) {
    [self handle_autogen : initialize_vector filtervec : filtervec
     autogen_text :[autogen_text stringByReplacingOccurrencesOfString : @ "--KeyOverlaidModifierWithRepeat--"
                    withString : @ "--KeyOverlaidModifier--Option::KEYOVERLAIDMODIFIER_REPEAT,"]];
    return;
  }

  if ([autogen_text rangeOfString : @ "--StripModifierFromScrollWheel--"].location != NSNotFound) {
    [self handle_autogen : initialize_vector filtervec : filtervec
     autogen_text :[NSString stringWithFormat : @ "%@,ModifierFlag::NONE",
                    [autogen_text stringByReplacingOccurrencesOfString : @ "--StripModifierFromScrollWheel--"
                     withString : @ "--ScrollWheelToScrollWheel--"]]];
    return;
  }

  if ([autogen_text rangeOfString : @ "SimultaneousKeyPresses::Option::RAW"].location != NSNotFound) {
    [self handle_autogen : initialize_vector filtervec : filtervec
     autogen_text :[autogen_text stringByReplacingOccurrencesOfString : @ "SimultaneousKeyPresses::Option::RAW"
                    withString : @ "Option::SIMULTANEOUSKEYPRESSES_RAW"]];
    return;
  }

  // ------------------------------------------------------------
  // append to initialize_vector
  //

  if ([autogen_text hasPrefix : @ "--ShowStatusMessage--"]) {
    NSString* params = [autogen_text substringFromIndex :[@ "--ShowStatusMessage--" length]];
    params = [params stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];

    const char* utf8string = [params UTF8String];
    size_t length = strlen(utf8string);
    [initialize_vector addObject :[NSNumber numberWithUnsignedLong : (length + 1)]];
    [initialize_vector addObject :[NSNumber numberWithUnsignedInt : BRIDGE_STATUSMESSAGE]];
    for (size_t i = 0; i < length; ++i) {
      [initialize_vector addObject :[NSNumber numberWithChar : utf8string[i]]];
    }

    // no need filtervec
    return;
  }

  if ([autogen_text hasPrefix : @ "--SimultaneousKeyPresses--"]) {
    NSString* params = [autogen_text substringFromIndex :[@ "--SimultaneousKeyPresses--" length]];

    NSString* newkeycode = [NSString stringWithFormat:@ "VK_SIMULTANEOUSKEYPRESSES_%d", simultaneous_keycode_index_];
    [keycode_ append : @ "KeyCode" name : newkeycode];
    ++simultaneous_keycode_index_;

    params = [NSString stringWithFormat:@ "KeyCode::%@,%@", newkeycode, params];
    [self append_to_initialize_vector : initialize_vector filtervec : filtervec params : params type : BRIDGE_REMAPTYPE_SIMULTANEOUSKEYPRESSES];

    return;
  }

  static struct {
    NSString* symbol;
    unsigned int type;
  } info[] = {
    { @ "--KeyToKey--",                       BRIDGE_REMAPTYPE_KEYTOKEY },
    { @ "--KeyToConsumer--",                  BRIDGE_REMAPTYPE_KEYTOCONSUMER },
    { @ "--KeyToPointingButton--",            BRIDGE_REMAPTYPE_KEYTOPOINTINGBUTTON },
    { @ "--DoublePressModifier--",            BRIDGE_REMAPTYPE_DOUBLEPRESSMODIFIER },
    { @ "--HoldingKeyToKey--",                BRIDGE_REMAPTYPE_HOLDINGKEYTOKEY },
    { @ "--IgnoreMultipleSameKeyPress--",     BRIDGE_REMAPTYPE_IGNOREMULTIPLESAMEKEYPRESS },
    { @ "--KeyOverlaidModifier--",            BRIDGE_REMAPTYPE_KEYOVERLAIDMODIFIER },
    { @ "--ConsumerToConsumer--",             BRIDGE_REMAPTYPE_CONSUMERTOCONSUMER },
    { @ "--ConsumerToKey--",                  BRIDGE_REMAPTYPE_CONSUMERTOKEY },
    { @ "--PointingButtonToPointingButton--", BRIDGE_REMAPTYPE_POINTINGBUTTONTOPOINTINGBUTTON },
    { @ "--PointingButtonToKey--",            BRIDGE_REMAPTYPE_POINTINGBUTTONTOKEY },
    { @ "--PointingRelativeToScroll--",       BRIDGE_REMAPTYPE_POINTINGRELATIVETOSCROLL },
    { @ "--DropKeyAfterRemap--",              BRIDGE_REMAPTYPE_DROPKEYAFTERREMAP },
    { @ "--SetKeyboardType--",                BRIDGE_REMAPTYPE_SETKEYBOARDTYPE },
    { @ "--ForceNumLockOn--",                 BRIDGE_REMAPTYPE_FORCENUMLOCKON },
    { @ "--DropPointingRelativeCursorMove--", BRIDGE_REMAPTYPE_DROPPOINTINGRELATIVECURSORMOVE },
    { @ "--DropScrollWheel--",                BRIDGE_REMAPTYPE_DROPSCROLLWHEEL },
    { @ "--ScrollWheelToScrollWheel--",       BRIDGE_REMAPTYPE_SCROLLWHEELTOSCROLLWHEEL },
    { @ "--ScrollWheelToKey--",               BRIDGE_REMAPTYPE_SCROLLWHEELTOKEY },
    { NULL, 0 },
  };
  for (int i = 0; info[i].symbol; ++i) {
    if ([autogen_text hasPrefix : info[i].symbol]) {
      [self append_to_initialize_vector : initialize_vector filtervec : filtervec params :[autogen_text substringFromIndex :[info[i].symbol length]] type : info[i].type];
      return;
    }
  }

  @throw [NSException exceptionWithName : @ "<autogen> error" reason :[NSString stringWithFormat : @ "unknown parameters: %@", autogen_text] userInfo : nil];
}

#endif
