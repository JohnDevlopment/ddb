#ifndef DDBCOMMAND_H
#define DDBCOMMAND_H

#include <tcl.h>

/* Equivelent to "ddb hash" in Tcl. */
int DdbSubcommand_Hash(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

#endif /* DDBCOMMAND_H */
