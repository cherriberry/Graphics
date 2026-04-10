#pragma once

#include <image.hpp>
#include <algorithm>
#include <queue>
#include <stack>
#include <cstdio>

class Element {
public:
    virtual void draw(Image &img) = 0;
    virtual ~Element() = default;
};

class Line : public Element {

public:
    int xA, yA;
    int xB, yB;
    Vector3f color;
    void draw(Image &img) override {
        // TODO: Implement Bresenham Algorithm
        int x1 = xA, y1 = yA, x2 = xB, y2 = yB;
        int dx = std::abs(xB - xA);
        int dy = std::abs(yB - yA);
        float e = 0.5;
        
        if( dx == 0 && dy != 0){
            printf("Draw a line from (%d, %d) to (%d, %d) using color (%f, %f, %f)\n", xA, yA, xB, yB,
                    color.x(), color.y(), color.z());
            int ys = std::min(yA, yB);
            int ye = std::max(yA, yB);
            for (int y = ys; y <= ye; ++y) {
                img.SetPixel(xA, y, color);
            }
            return;
        }
        
        else if (dx != 0 && dy == 0) {
            int xs = std::min(xA, xB);
            int xe = std::max(xA, xB);
            for (int x = xs; x <= xe; ++x) {
                img.SetPixel(x, yA, color);
            }
            printf("Draw a line from (%d, %d) to (%d, %d) using color (%f, %f, %f)\n", xA, yA, xB, yB,
                    color.x(), color.y(), color.z());
            return;
        }



        int x , y, origin_dir;
        bool swapped = false;
        if (dy > dx) {
            swapped = true;
            std::swap(x1, y1);
            std::swap(x2, y2);
            std::swap(dx, dy);
            e = 0.5 * dx / dy;
        }

        if ( x1 < x2 ) {
            x = x1;
            y = y1;
            origin_dir = (y2 - y1) > 0 ? 1 : -1;
            if (y2 == y1)
                origin_dir = 0;
        }
        else {
            x = x2;
            y = y2;
            origin_dir = (y1 - y2) > 0 ? 1 : -1;
            if (y1 == y2)
                origin_dir = 0;
        }

        for(int i = 0; i <= dx; ++i ) {
            if (swapped && y >= 0 && y < img.Width() && x >= 0 && x < img.Height())
                img.SetPixel(y, x, color);
            else if(!swapped && x >= 0 && x < img.Width() && y >= 0 && y < img.Height())
                img.SetPixel(x, y, color);
            else
                break;
            x++;
            e += 2 * dy;
            if (e >= 0){
                y += origin_dir;
                e -= 2 * dx;
            }
        }


        printf("Draw a line from (%d, %d) to (%d, %d) using color (%f, %f, %f)\n", xA, yA, xB, yB,
                color.x(), color.y(), color.z());
    }
};

class Circle : public Element {

public:
    int cx, cy;
    int radius;
    Vector3f color;

    void circlePoints(Image &img, int x, int y) {
        img.SetPixel(cx + x, cy + y, color);
        img.SetPixel(cx - x, cy + y, color);
        img.SetPixel(cx + x, cy - y, color);
        img.SetPixel(cx - x, cy - y, color);
        img.SetPixel(cx + y, cy + x, color);
        img.SetPixel(cx - y, cy + x, color);
        img.SetPixel(cx + y, cy - x, color);
        img.SetPixel(cx - y, cy - x, color);
    }

    void draw(Image &img) override {
        // TODO: Implement Algorithm to draw a Circle
        int x = 0, y = radius;
        int d = 3 - 2 * radius;
        while (x <= y) {
            circlePoints(img, x, y);
            if (d < 0) {
                d += 2 * x + 3;
            }
            else {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }

        printf("Draw a circle with center (%d, %d) and radius %d using color (%f, %f, %f)\n", cx, cy, radius,
               color.x(), color.y(), color.z());
    }
};

class Fill : public Element {

public:
    int cx, cy;
    Vector3f color;

    struct Point {
        int x, y;
    };

    

    void draw(Image &img) override {
        // TODO: Flood fill
        printf("Fill with source point (%d, %d) using color (%f, %f, %f)\n", cx, cy, color.x(), color.y(), color.z());

        if (cx < 0 || cx >= img.Width() || cy < 0 || cy >= img.Height()) {
            printf("Flood fill source point is out of image boundary\n", cx, cy);
            return;
        }
        
        if (img.GetPixel(cx, cy) == color) {
            printf("Flood fill source point already has the target color\n", cx, cy);
            return;
        }

        bool spanAbove, spanBelow;
        Point p{cx, cy};
        std::stack<Point> s;
        Vector3f oldcolor = img.GetPixel(cx, cy);
        s.push(p);

        while( !s.empty() ) {
            int xl = 0, xr = img.Width() - 1;
            p = s.top();
            s.pop();

            if (p.x < 0 || p.x >= img.Width() || p.y < 0 || p.y >= img.Height()) {
                continue;
            }

            if (img.GetPixel(p.x, p.y) != oldcolor) {
                continue;
            }

            int x = p.x;
            int y = p.y;
            // 向右填充
            while (x <= xr && img.GetPixel(x, p.y) == oldcolor) {
                img.SetPixel(x, p.y, color);
                x++;
            }
            xr = x - 1;

            // 向左填充
            x = p.x - 1;
            while (x >= xl && img.GetPixel(x, p.y) == oldcolor) 
            {
                img.SetPixel(x, p.y, color);
                x--;
            }
            xl = x + 1;

            if (cx == 400)
                printf("Fill line from (%d, %d) to (%d, %d)\n", xl, p.y, xr, p.y);

            // 上一行是否需要填充
            spanAbove = false;
            y = p.y + 1;
            x = xl;
            while(x <= xr && img.GetPixel(x, y) != oldcolor ) x++;//退出或找下一段
            while(x <= xr && x >= 0 ) {
                spanAbove = false;
                while(x < img.Width() && x >= 0 && img.GetPixel(x, y) == oldcolor) {
                    spanAbove = true;
                    x++; // 找到最右边界
                }
                if (spanAbove) {
                    s.push({x - 1, y});
                    spanAbove = false;
                }
                while(x <= xr && img.GetPixel(x, y) != oldcolor ) x++;//退出或找下一段
            }

            // 向下检查
            spanBelow = false;
            y = p.y - 1; 
            if (y < 0) {
                continue;
            }
            x = xl;
            while(x <= xr && img.GetPixel(x, y) != oldcolor ) x++;//退出或找下一段
            while(x <= xr && x >= 0) {
                spanBelow = false;
                while( x < img.Width() && img.GetPixel(x, y) == oldcolor ) {
                    spanBelow = true;
                    x++; // 找到最右边界
                }
                if (spanBelow) {
                    s.push({x - 1, y});
                    spanBelow = false;
                }
                while(x <= xr && img.GetPixel(x, y) != oldcolor ) x++;//退出或找下一段
            }
        }

        printf("Flood fill source point = (%d, %d) using color (%f, %f, %f)\n", cx, cy,
                color.x(), color.y(), color.z());
    }
};