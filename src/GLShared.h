/*
 *  LDForge: LDraw parts authoring CAD
 *  Copyright (C) 2013, 2014 Santeri Piippo
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

#ifndef LDFORGE_GLSHARED_H
#define LDFORGE_GLSHARED_H
#include <QString>

class LDObject;

struct LDPolygon
{
	char		num;
	Vertex		vertices[4];
	int			id;
	int			color;
	QString		origin;

	inline int numVertices() const
	{
		return (num == 5) ? 4 : num;
	}
};

enum EVBOSurface
{
	vboLines,
	vboTriangles,
	vboQuads,
	vboCondLines,
	vboNumSurfaces
};

enum EVBOComplement
{
	vboSurfaces,
	vboNormalColors,
	vboPickColors,
	vboBFCFrontColors,
	vboBFCBackColors,
	vboNumComplements
};

// KDevelop doesn't seem to understand some VBO stuff
#ifdef IN_IDE_PARSER
using GLenum = unsigned int;
using GLuint = unsigned int;
void glBindBuffer (GLenum, GLuint);
void glGenBuffers (GLuint, GLuint*);
void glDeleteBuffers (GLuint, GLuint*);
void glBufferData (GLuint, GLuint, void*, GLuint);
#endif

static const int gNumVBOs = vboNumSurfaces * vboNumComplements;

#endif // LDFORGE_GLSHARED_H