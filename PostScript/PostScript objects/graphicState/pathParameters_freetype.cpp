#include "job.h"

#include "pathParameters.h"

#include "ftraster.c"
#include <fstream>

//#include <ft2build.h>

//#include <freetype/freetype.h>
//#include <freetype/ftoutln.h>
//#include <freetype/ftglyph.h>

    HBITMAP createTemporaryBitmap(long cx,long cy,long bitsPerComponent,BYTE *pBits);

    void * MY_Alloc_Func(FT_Memory,long size) {
    return malloc((size_t)size);
    }

    void MY_Free_Func(FT_Memory,void *block) {
    free(block);
    }

    void *MY_Realloc_Func(FT_Memory,long,long new_size,void *block) {
    return realloc(block, (size_t)new_size);
    }

    //static FT_Library pFTLibrary = NULL;

    void pathParameters::rasterizePathFreetype() {

    int cntVertices = GetPath(pPStoPDF -> GetDC(),NULL,NULL,0);

    if ( 0 == cntVertices ) 
        return;

FT_Memory mem;

mem = new FT_MemoryRec;
mem -> alloc = MY_Alloc_Func;
mem -> free = MY_Free_Func;
mem -> realloc = MY_Realloc_Func;

FT_Outline_ outline;
outline.n_contours = 0;

    POINT *pThePoints = new POINT[cntVertices];
    BYTE *pTheVertexTypes = new BYTE[cntVertices];

    GetPath(pPStoPDF -> GetDC(),pThePoints,pTheVertexTypes,cntVertices);

outline.n_points = cntVertices;
outline.points = new FT_Vector[outline.n_points];
outline.tags = new unsigned char[outline.n_points];

    POINT *pPoint = pThePoints;
    POINT *pLastPoint = pThePoints + cntVertices;
    BYTE *pVertexType = pTheVertexTypes;

    boolean isFigureStarted = false;

int pointIndex = 0;
    do {

        uint16_t delta = 1;

        if ( *pVertexType & PT_CLOSEFIGURE ) {
            //pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            isFigureStarted = false;
        }

        if ( PT_MOVETO == *pVertexType ) {

            //if ( isFigureStarted )
            //    pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
outline.n_contours++;

            //pID2D1GeometrySink -> BeginFigure(D2D1::Point2F((FLOAT)pPoint -> x,(FLOAT)pPoint -> y),D2D1_FIGURE_BEGIN_FILLED);
outline.points[pointIndex].x = 16 * pPoint -> x;
outline.points[pointIndex].y = 16 * pPoint -> y;
outline.tags[pointIndex] = 0;
            isFigureStarted = true;

        } else if ( PT_LINETO == *pVertexType ) {

            //pID2D1GeometrySink -> AddLine(D2D1::Point2F((FLOAT)pPoint -> x,(FLOAT)pPoint -> y));
outline.points[pointIndex].x = 16 * pPoint -> x;
outline.points[pointIndex].y = 16 * pPoint -> y;
outline.tags[pointIndex] = 1;

        } else if ( PT_BEZIERTO == *pVertexType ) {

/*
            pID2D1GeometrySink -> AddBezier(D2D1::BezierSegment(
                D2D1::Point2F((FLOAT)pPoint -> x,(FLOAT)pPoint -> y),
                D2D1::Point2F((FLOAT)(pPoint + 1) -> x,(FLOAT)(pPoint + 1) -> y),
                D2D1::Point2F((FLOAT)(pPoint + 2) -> x,(FLOAT)(pPoint + 2) -> y)));
*/
            delta = 3;

        }

        pPoint += delta;
        pVertexType += delta;

pointIndex++;

    } while ( pPoint < pLastPoint);

outline.contours = new unsigned short[outline.n_contours];

outline.contours[0] = outline.n_points - 1;
outline.flags = 0;

const int width = 500;
const int rows = 400;

  // 1 bit per pixel.
  const int pitch_mono = (width + 7) >> 3;

  FT_Bitmap bmp;
  FT_Raster raster;
 
  // Allocate a chunk of memory for the render pool.
  const int kRenderPoolSize = 1024 * 1024;
  unsigned char *renderPool = new unsigned char[kRenderPoolSize];

bmp.buffer = new unsigned char[rows * pitch_mono];
memset(bmp.buffer, 0, rows * pitch_mono);

bmp.width = width;
bmp.rows = rows;
bmp.pitch = pitch_mono;
bmp.pixel_mode = FT_PIXEL_MODE_MONO;

FT_Raster_Params params;
memset(&params, 0, sizeof (params));

params.source = &outline;
params.target = &bmp;

ft_standard_raster.raster_new(mem, &raster);
ft_standard_raster.raster_reset(raster, renderPool, kRenderPoolSize);
ft_standard_raster.raster_render(raster, &params);

  std::ofstream out_mono("out-mono.pbm", std::ios::binary);
  out_mono << "P4 " << width << " " << rows << "\n";
  out_mono.write((const char *)bmp.buffer, rows * pitch_mono);


    return;
}
