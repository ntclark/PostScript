#pragma once

#include <limits>

struct POINTD;

typedef double POINT_TYPE;

    struct GS_POINT {
        GS_POINT() : x(0.0),y(0.0) {}
        GS_POINT(POINT_TYPE px,POINT_TYPE py) : x(px), y(py) {}
        GS_POINT(POINT pt) : x((POINT_TYPE)pt.x),y((POINT_TYPE)pt.y) {}
        GS_POINT(GS_POINT *pPointd);
        POINT_TYPE x;
        POINT_TYPE y;
    };

    static GS_POINT &operator-=(GS_POINT &left,GS_POINT right) { 
        left.x -= right.x;
        left.y -= right.y; 
        return left; 
    }

    static GS_POINT operator-(GS_POINT left,GS_POINT right) {
        return left -= right;
    }

    static GS_POINT &operator+=(GS_POINT &left,GS_POINT right) {
        left.x += right.x;
        left.y += right.y;
        return left;
    }

    static GS_POINT operator+(GS_POINT left,GS_POINT right) {
        return left += right;
    }

    static GS_POINT &operator*=(GS_POINT &left,POINT_TYPE right) {
        left.x *= right;
        left.y *= right;
        return left; 
    }

    static GS_POINT operator*(POINT_TYPE left, GS_POINT right ) {
        return right *= left;
    }

    static GS_POINT operator*(GS_POINT left, POINT_TYPE right ) {
        return left *= right;
    }

    static POINT_TYPE dot( GS_POINT *pLeft, GS_POINT *pRight ) {
    return pLeft -> x * pRight -> x + pLeft -> y * pRight -> y;
    }

    static POINT_TYPE dot( GS_POINT &left, GS_POINT &right ) {
    return left.x * right.x + left.y * right.y; 
    }

    static GS_POINT perpendicular( GS_POINT that ) {
    return GS_POINT( -that.y, that.x ); 
    }