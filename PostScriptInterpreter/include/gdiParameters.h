#pragma once

    class gdiParameters {
    public:

        HRESULT SaveState();
        HRESULT RestoreState();

        colorSpace *pColorSpace{NULL};

        void setLineCap(long v);
        void setLineJoin(long v);
        void setLineWidth(FLOAT v);
        void setLineDash(FLOAT *pValues,long countValues,FLOAT offset);

        void setColorSpace(colorSpace *pColorSpace);
        colorSpace *getColorSpace();

        void setColor(colorSpace *pColorSpace);
        void setRGBColor(COLORREF rgb);
        void setRGBColor(FLOAT r,FLOAT g,FLOAT b);

    };
