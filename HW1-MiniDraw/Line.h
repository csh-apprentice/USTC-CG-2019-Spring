

#include "Shape.h"

class Line : public Shape {
public:
	Line();
	~Line();

	void set_width() override;
	void set_heigth() override;
	void Draw(QPainter& painter);
};
