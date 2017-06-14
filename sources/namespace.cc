/* @file namespace.cc
 *
 * Namespace routines.
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
/*
  	#[ Includes :
*/

extern "C" {
#include "form3.h"
#include "namespace.h"
}

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

using std::out_of_range;
using std::size_t;
using std::string;
using std::vector;

#ifdef HAVE_UNORDERED_MAP
	#include <unordered_map>
	using std::unordered_map;
#elif !defined(HAVE_TR1_UNORDERED_MAP) && defined(HAVE_BOOST_UNORDERED_MAP_HPP)
	#include <boost/unordered_map.hpp>
	using boost::unordered_map;
#else
	#include <tr1/unordered_map>
	using std::tr1::unordered_map;
#endif

/*
  	#] Includes : 
  	#[ Declarations :
*/

class Namespace {
public:
	Namespace(const string &fullname, Namespace *outer = NULL) : outer_(outer) {}

private:
	Namespace *outer_;
	unordered_map<string, string> aliases_;
};

class NamespaceList : public NAMESPACE_DATA {
public:
	void push(const string &name) {
		current_namespace_.push_back(name);
		// TODO
		MesPrint("push namespace <%s>",name.c_str());
	}

	void pop() {
		assert(current_namespace_.size() > 0);
		current_namespace_.pop_back();
		// TODO
		MesPrint("pop namespace");
	}

private:
	string current_scope_;
	vector<string> current_namespace_;
	unordered_map<string, Namespace> namespaces_;
};

/*
  	#] Declarations : 
  	#[ C interface :
*/

void InitNamespace(NAMESPACE *names) {
	names->p = new NamespaceList();
	names->level = 0;
}

void ClearNamespace(NAMESPACE *names) {
	delete names->p;
}

void PushNamespaceImpl(NAMESPACE *names, const UBYTE *name, UWORD len) {
	NamespaceList &list = *static_cast<NamespaceList *>(names->p);
	list.push(string((const char *)name, (size_t)len));
}

void PopNamespaceImpl(NAMESPACE *names) {
	NamespaceList &list = *static_cast<NamespaceList *>(names->p);
	list.pop();
}

/*
  	#] C interface : 
*/
