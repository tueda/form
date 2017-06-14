/** @file namespace.h
 *
 *  C interface to the namespace routines.
 */
/* #[ License : */
/*
 *   Copyright (C) 1984-2013 J.A.M. Vermaseren
 *   When using this file you are requested to refer to the publication
 *   J.A.M.Vermaseren "New features of FORM" math-ph/0010025
 *   This is considered a matter of courtesy as the development was paid
 *   for by FOM the Dutch physics granting agency and we would like to
 *   be able to track its scientific use to convince FOM of its value
 *   for the community.
 *
 *   This file is part of FORM.
 *
 *   FORM is free software: you can redistribute it and/or modify it under the
 *   terms of the GNU General Public License as published by the Free Software
 *   Foundation, either version 3 of the License, or (at your option) any later
 *   version.
 *
 *   FORM is distributed in the hope that it will be useful, but WITHOUT ANY
 *   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *   details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with FORM.  If not, see <http://www.gnu.org/licenses/>.
 */
/* #] License : */ 
#ifndef FORM_NAMESPACE_H_
#define FORM_NAMESPACE_H_
/*
  	#[ Includes :
*/

#include "form3.h"

/* Forward declaration required for the inline functions. */
int MesPrint(const char *,...);  /* declare.h, message.c */

/*
  	#] Includes : 
  	#[ Declarations :
*/

typedef struct NAMESPACE_DATA_tag {
} NAMESPACE_DATA;  /* extended in C++ implementation */

typedef struct NAMESPACE_tag {
	NAMESPACE_DATA *p;
	WORD level;  /* cache the current level for C inline functions */
	PADPOINTER(0,0,1,0);
} NAMESPACE;

void InitNamespace(NAMESPACE *names);
void ClearNamespace(NAMESPACE *names);

void PushNamespaceImpl(NAMESPACE *names, const UBYTE *name, UWORD len);
void PopNamespaceImpl(NAMESPACE *names);

static inline int PrePushNamespace(NAMESPACE *names, const UBYTE *name, UWORD len) {
	if ( names->level > 1024 ) {
		MesPrint("@Too many nested %#namespace. Infinite loop?");
		return(-1);
	}
	PushNamespaceImpl(names, name, len);
	names->level++;
	return(0);
}

static inline int PrePopNamespace(NAMESPACE *names) {
	if ( names->level == 0 ) {
		MesPrint("@%#endnamespace without corresponding %#namespace");
		return(-1);
	}
	PopNamespaceImpl(names);
	names->level--;
	return(0);
}

/*
  	#] Declarations : 
*/
#endif  /* FORM_NAMESPACE_H_ */
