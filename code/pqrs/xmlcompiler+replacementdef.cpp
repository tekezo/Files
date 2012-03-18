#include <exception>
#include <boost/foreach.hpp>
#include "pqrs/xmlcompiler.hpp"

namespace pqrs {
  bool
  xmlcompiler::reload_replacementdef_(void)
  {
    bool retval = false;

    replacement_.clear();

    const char* paths[] = {
      "/Users/tekezo/Library/Application Support/KeyRemap4MacBook/private.xml",
      "/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/replacementdef.xml",
    };
    BOOST_FOREACH(const char* xmlfilepath, paths) {
      boost::property_tree::ptree pt;
      if (! pqrs::xmlcompiler::read_xml(xmlfilepath, pt, false)) {
        continue;
      }

      traverse_replacementdef_(pt);

      // Set retval to true if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide replacementdef.xml is not loaded.)
      retval = true;
    }

    return true;
  }

  void
  xmlcompiler::traverse_replacementdef_(const boost::property_tree::ptree& pt)
  {
    for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
      if (it->first != "replacementdef") {
        traverse_replacementdef_(it->second);
      } else {
        std::cout << "replacementdef" << std::endl;
        std::cout << (it->second).get_optional<std::string>("replacementname") << std::endl;
        std::cout << (it->second).get_optional<std::string>("replacementvalue") << std::endl;
      }
    }
#if 0

  } else {
    ConfigXMLParserReplacementDefData* newdata = [self parse_replacementdef:e];
    if (newdata) {
      // Adding to replacement_ if needed.
      if (! [replacement_ objectForKey : newdata.name]) {
        [replacement_ setObject : newdata.value forKey : newdata.name];
      }
    }
  }
}
#endif
}

#if 0

-(ConfigXMLParserReplacementDefData*) parse_replacementdef : (NSXMLElement*)replacementdefElement
{
  ConfigXMLParserReplacementDefData* newdata = [[ConfigXMLParserReplacementDefData new] autorelease];

  NSUInteger count = [replacementdefElement childCount];
  for (NSUInteger i = 0; i < count; ++i) {
    NSXMLElement* e = [self castToNSXMLElement:[replacementdefElement childAtIndex:i]];
    if (! e) continue;

    NSString* name = [e name];
    NSString* stringValue = [self trim:[e stringValue]];

    if ([name isEqualToString : @ "replacementname"]) {
      newdata.name = [NSString stringWithFormat : @ "#{%@}", stringValue];
    } else if ([name isEqualToString : @ "replacementvalue"]) {
      newdata.value = stringValue;
    }
  }

  if (! newdata.name) return nil;
  if (! newdata.value) return nil;

  return newdata;
}

-(void) traverse_replacementdef : (NSXMLElement*)element
{
  NSUInteger count = [element childCount];
  for (NSUInteger i = 0; i < count; ++i) {
    NSXMLElement* e = [self castToNSXMLElement:[element childAtIndex:i]];
    if (! e) continue;

    if (! [[e name] isEqualToString : @ "replacementdef"]) {
      [self traverse_replacementdef : e];

    } else {
      ConfigXMLParserReplacementDefData* newdata = [self parse_replacementdef:e];
      if (newdata) {
        // Adding to replacement_ if needed.
        if (! [replacement_ objectForKey : newdata.name]) {
          [replacement_ setObject : newdata.value forKey : newdata.name];
        }
      }
    }
  }
}
#endif
}
