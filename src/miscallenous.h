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

#pragma once
#include <QVector>
#include <functional>
#include "configuration.h"
#include "main.h"
#include "basics.h"

#define NUM_PRIMES 500

class LDDocument;
class QColor;
class QAction;

// Prime numbers
extern const int g_primes[NUM_PRIMES];

// Simplifies the given fraction.
void simplify (int& numer, int& denom);

using ApplyToMatrixFunction = std::function<void (int, double&)>;
using ApplyToMatrixConstFunction = std::function<void (int, double)>;

void roundToDecimals (double& a, int decimals);
void applyToMatrix (Matrix& a, ApplyToMatrixFunction func);
void applyToMatrix (const Matrix& a, ApplyToMatrixConstFunction func);

double getCoordinate (const Vertex& a, Axis ax);
QString join (QList< StringFormatArg > vals, QString delim = " ");

// Grid stuff
struct gridinfo
{
	const char*				name;
	ConfigEntry::FloatType*	coordsnap;
	ConfigEntry::FloatType*	anglesnap;
};

EXTERN_CFGENTRY (Int, grid);
static const int g_numGrids = 3;
extern const gridinfo g_gridInfo[3];

inline const gridinfo& currentGrid()
{
	return g_gridInfo[cfg::grid];
}

// =============================================================================
enum ERotationPoint
{
	EObjectOrigin,
	EWorldOrigin,
	ECustomPoint
};

Vertex rotPoint (const LDObjectList& objs);
void configRotationPoint();

// =============================================================================
namespace Grid
{
	enum Type
	{
		Coarse,
		Medium,
		Fine
	};

	enum Config
	{
		Coordinate,
		Angle
	};

	double snap (double value, const Grid::Config type);
}

// =============================================================================
// Plural expression
template<class T> static inline const char* plural (T n)
{
	return (n != 1) ? "s" : "";
}

// =============================================================================
// Templated clamp
template<class T> static inline T clamp (T a, T min, T max)
{
	return (a > max) ? max : (a < min) ? min : a;
}

// Templated minimum
template<class T> static inline T min (T a, T b)
{
	return (a < b) ? a : b;
}

// Templated maximum
template<class T> static inline T max (T a, T b)
{
	return (a > b) ? a : b;
}

// Templated absolute value
template<class T> static inline T abs (T a)
{
	return (a >= 0) ? a : -a;
}

template<class T> inline bool isZero (T a)
{
	return abs<T> (a) < 0.0001;
}

template<class T> inline bool isInteger (T a)
{
	return isZero (a - (int) a);
}

template<class T> void removeDuplicates (QList<T>& a)
{
	std::sort (a.begin(), a.end());
	a.erase (std::unique (a.begin(), a.end()), a.end());
}

inline QString utf16 (const char16_t* a)
{
	if (Q_LIKELY (sizeof(char16_t) == sizeof(unsigned short)))
		return QString::fromUtf16 (reinterpret_cast<const unsigned short*> (a));

	QVector<unsigned short> data;

	for (const char16_t* ap = &a[0]; *ap != '\u0000'; ++ap)
		data << *ap;

	data << '\u0000';
	return QString::fromUtf16 (data.constData());
}
