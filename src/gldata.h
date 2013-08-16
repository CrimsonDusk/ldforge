#ifndef LDFORGE_GLDATA_H
#define LDFORGE_GLDATA_H

#include "types.h"
#include <QMap>

class QColor;
class LDFile;

/* =============================================================================
 * -----------------------------------------------------------------------------
 * VertexCompiler
 *
 * This class manages vertex arrays for the GL renderer, compiling vertices into
 * VAO-readable triangles which can be requested with getMergedBuffer.
 *
 * There are 5 main array types:
 * - the normal polygon array, for triangles
 * - edge line array, for lines
 * - BFC array, this is the same as the normal polygon array except that the
 * -     polygons are listed twice, once normally and green and once reversed
 * -     and red, this allows BFC red/green view.
 * - Picking array, this is the samea s the normal polygon array except the
 * -     polygons are compiled with their index color, this way the picking
 * -     method is capable of determining which object was selected by pixel
 * -     color.
 * - Edge line picking array, the pick array version of the edge line array.
 *
 * There are also these same 5 arrays for every LDObject compiled. The main
 * arrays are generated on demand from the ones in the current file's
 * LDObjects and stored in cache for faster rendering.
 *
 * The nested Array class contains a vector-like buffer of the Vertex structs,
 * these structs are the VAOs that get passed to the renderer.
 */

class VertexCompiler {
public:
	enum ColorType {
		Normal,
		BFCFront,
		BFCBack,
		PickColor,
	};
	
	enum ArrayType {
		MainArray,
		EdgeArray,
		BFCArray,
		PickArray,
		EdgePickArray,
		NumArrays
	};
	
	struct Vertex {
		float x, y, z;
		uint32 color;
		float pad[4];
	};
	
	class Array {
	public:
		typedef int32 Size;
		
		Array();
		Array (const Array& other) = delete;
		~Array();
		
		void clear();
		void merge (Array* other);
		void resizeToFit (Size newSize);
		const Size& allocatedSize() const;
		Size writtenSize() const;
		const Vertex* data() const;
		void write (const VertexCompiler::Vertex& f);
		Array& operator= (const Array& other) = delete;
		
	private:
		Vertex* m_data;
		Vertex* m_ptr;
		Size m_size;
	};
	
	VertexCompiler();
	~VertexCompiler();
	void setFile (LDFile* file);
	void compileFile();
	void compileObject (LDObject* obj, LDObject* topobj);
	void forgetObject (LDObject* obj);
	Array* getMergedBuffer (ArrayType type);
	QColor getObjectColor (LDObject* obj, ColorType list) const;
	
private:
	void compilePolygon (LDObject* drawobj, LDObject* trueobj);
	void compileVertex (vertex v, QColor col, VertexCompiler::Array* array);
	
	QMap<LDObject*, Array**> m_objArrays;
	QMap<LDFile*, Array*> m_fileCache;
	Array* m_mainArrays[NumArrays];
	LDFile* m_file;
	bool m_changed[NumArrays];
};

#endif // LDFORGE_GLDATA_H