#ifndef DDBCOMMAND_H
#define DDBCOMMAND_H

#include <tcl.h>

/* Equivelent to "ddb hash" in Tcl. */
int DdbSubcommand_Hash(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

/* Tcl command "ddb init". */
int DdbSubcommand_Init(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

/* Tcl command "ddb column". */
int DdbSubcommand_Columns(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

/* Tcl command "ddb print". */
int DdbSubcommand_Print(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

#endif /* DDBCOMMAND_H */
