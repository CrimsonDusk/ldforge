/*
 *  LDForge: LDraw parts authoring CAD
 *  Copyright (C) 2013 Santeri Piippo
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LDCONFIG_H
#define LDCONFIG_H

#include "types.h"

// =============================================================================
// StringParser
//
// String parsing utility
// =============================================================================
class LDConfigParser {
public:
	LDConfigParser (str inText, char sep);
	
	bool atEnd ();
	bool atBeginning ();
	bool next (str& val);
	bool peekNext (str& val);
	bool getToken (str& val, const ushort pos);
	bool findToken (short& result, char const* needle, short args);
	size_t size ();
	void rewind ();
	void seek (short amount, bool rel);
	bool tokenCompare (short inPos, const char* sOther);
	
	str operator[] (const size_t idx) {
		return m_tokens[idx];
	}
	
private:
	List<str> m_tokens;
	short m_pos;
};

void parseLDConfig();

#endif // LDCONFIG_H