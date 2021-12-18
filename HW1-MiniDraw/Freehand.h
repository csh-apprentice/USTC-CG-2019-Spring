#pragma once
#include "Shape.h"
#include <vector>
#include <QPoint>

class Freehand :public Shape
{
public:
    Freehand(std::vector<QPoint> toppoints);
    Freehand();            //recast initialize function
    ~Freehand();

    void set_width() override;
    void set_heigth() override;
    void Draw(QPainter& painter);
    void set_center_point() override;


private:
    std::vector<QPoint> temp_points;           //storage the information of the top points of the Polygon
    QPoint* PointArray();
    int size_points = 0;                    //the number of the top points of the Polygon
};