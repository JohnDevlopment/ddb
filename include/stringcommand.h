#ifndef STRINGCOMMAND_H
#define STRINGCOMMAND_H

#include <tcl.h>

/* Implementation of "_string charcount" in Tcl. */
int StringSubcommand_Charcount(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

#endif /* STRINGCOMMAND_H */
