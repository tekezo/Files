#ifndef PQRS_XMLCOMPILER_BINDINGS_CLANG_H
#define PQRS_XMLCOMPILER_BINDINGS_CLANG_H

#ifdef __cplusplus
extern "C" {
#endif

struct pqrs_xmlcompiler;

int pqrs_xmlcompiler_initialize(struct pqrs_xmlcompiler** out,
                                const char* system_xml_directory,
                                const char* private_xml_directory);
void pqrs_xmlcompiler_terminate(struct pqrs_xmlcompiler** out);

void pqrs_xmlcompiler_reload(struct pqrs_xmlcompiler* p);
const char* pqrs_xmlcompiler_get_error_message(const struct pqrs_xmlcompiler* p);
int pqrs_xmlcompiler_get_error_count(const struct pqrs_xmlcompiler* p);

#ifdef __cplusplus
}
#endif

#endif
