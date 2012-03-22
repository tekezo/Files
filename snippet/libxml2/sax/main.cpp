#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <string>
#include <libxml/SAX2.h>
#include "pqrs/string.hpp"

xmlSAXHandler make_sax_handler();

static void OnStartElementNs(void* ctx,
                             const xmlChar* localname,
                             const xmlChar* prefix,
                             const xmlChar* URI,
                             int nb_namespaces,
                             const xmlChar** namespaces,
                             int nb_attributes,
                             int nb_defaulted,
                             const xmlChar** attributes
                             );

static void OnEndElementNs(void* ctx,
                           const xmlChar* localname,
                           const xmlChar* prefix,
                           const xmlChar* URI
                           );

static void OnCharacters(void* ctx, const xmlChar* ch, int len);

int
main(int argc, char* argv[])
{
  std::string xml;
  pqrs::string::replacement replacement;
  pqrs::string::string_by_replacing_double_curly_braces_from_file(xml, "sample.xml", replacement);

  xmlSAXHandler SAXHander = make_sax_handler();
  xmlSAXUserParseMemory(&SAXHander, NULL, xml.c_str(), xml.size());

  sleep(1000);

  return 0;
}

xmlSAXHandler make_sax_handler () {
  xmlSAXHandler SAXHander;

  memset(&SAXHander, 0, sizeof(xmlSAXHandler));

  SAXHander.initialized = XML_SAX2_MAGIC;
  SAXHander.startElementNs = OnStartElementNs;
  SAXHander.endElementNs = OnEndElementNs;
  SAXHander.characters = OnCharacters;

  return SAXHander;
}

static void OnStartElementNs(
  void* ctx,
  const xmlChar* localname,
  const xmlChar* prefix,
  const xmlChar* URI,
  int nb_namespaces,
  const xmlChar** namespaces,
  int nb_attributes,
  int nb_defaulted,
  const xmlChar** attributes
  ) {
  printf("<%s>\n", localname);
}

static void OnEndElementNs(
  void* ctx,
  const xmlChar* localname,
  const xmlChar* prefix,
  const xmlChar* URI
  ) {
  printf("</%s>\n", localname);
}

static void OnCharacters(void* ctx, const xmlChar* ch, int len) {
  char chars[len + 1];
  strncpy(chars, (const char*)ch, len);
  chars[len] = (char)NULL;
  printf("[%s]\n", chars);
}
