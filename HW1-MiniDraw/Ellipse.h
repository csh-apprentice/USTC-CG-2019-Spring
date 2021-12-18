

#include "Shape.h"

class MEllipse : public Shape {
public:
	MEllipse();
	~MEllipse();

	void set_width() override;
	void set_heigth() override;
	void Draw(QPainter& painter);
};
