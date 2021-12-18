


#include "Shape.h"

class Rect : public Shape {
public:
	Rect();
	~Rect();

	void Draw(QPainter& painter);
	void set_width() override;
	void set_heigth() override;
};
