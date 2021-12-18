#pragma once
#include "Shape.h"
#include <vector>
#include <QPoint>

class MPolygon :public Shape
{public:
    MPolygon(std::vector<QPoint> toppoints);
    MPolygon();            //recast initialize function
    ~MPolygon();

    void set_width() override;
    void set_heigth() override;
    void set_center_point() override;
   // QPoint get_center_point() override;
    void Draw(QPainter& painter);
     
private:
    std::vector<QPoint> top_points;           //storage the information of the top points of the Polygon
    QPoint* PointArray();
    int size_points=0;                    //the number of the top points of the Polygon
 };