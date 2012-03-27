#include "pqrs/xmlcompiler.hpp"
#include "pqrs/xmlcompiler_bindings_clang.h"

typedef std::tr1::shared_ptr<pqrs::xmlcompiler> xmlcompiler_ptr;

struct pqrs_xmlcompiler {
  xmlcompiler_ptr xmlcompiler_ptr;
};

int
pqrs_xmlcompiler_initialize(struct pqrs_xmlcompiler** out,
                            const char* system_xml_directory,
                            const char* private_xml_directory)
{
  if (! out) return -1;
  // return if already initialized.
  if (*out) return -1;

  *out = new pqrs_xmlcompiler();
  (*out)->xmlcompiler_ptr = xmlcompiler_ptr(new pqrs::xmlcompiler(system_xml_directory, private_xml_directory));

  return 0;
}

void
pqrs_xmlcompiler_terminate(struct pqrs_xmlcompiler** out)
{
  if (out && *out) {
    delete *out;
    *out = NULL;
  }
}

void
pqrs_xmlcompiler_reload(struct pqrs_xmlcompiler* p)
{
  if (! p) return;
  if (! p->xmlcompiler_ptr) return;

  p->xmlcompiler_ptr->reload();
}

const char*
pqrs_xmlcompiler_get_error_message(const struct pqrs_xmlcompiler* p)
{
  if (! p) return NULL;
  if (! p->xmlcompiler_ptr) return NULL;

  return (p->xmlcompiler_ptr->get_error_message()).c_str();
}

int
pqrs_xmlcompiler_get_error_count(const struct pqrs_xmlcompiler* p)
{
  if (! p) return NULL;
  if (! p->xmlcompiler_ptr) return NULL;

  return p->xmlcompiler_ptr->get_error_count();
}
