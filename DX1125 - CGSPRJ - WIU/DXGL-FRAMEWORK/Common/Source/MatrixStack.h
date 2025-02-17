#ifndef MXS_H
#define MXS_H

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <stack>

class MatrixStack
{
public:
	MatrixStack(void);
	~MatrixStack(void);
	const glm::mat4& Top(void) const;
	void PopMatrix(void);
	void PushMatrix(void);
	void Clear(void);
	void LoadIdentity(void);
	void LoadMatrix(const glm::mat4& matrix);
	void MultMatrix(const glm::mat4& matrix);
	void Rotate(float degrees, float axisX, float axisY, float axisZ);
	void Scale(float scaleX, float scaleY, float scaleZ);
	void Translate(float translateX, float translateY, float translateZ);
	void Frustum(double left, double right, double bottom, double top, double near, double
		far);
	void LookAt(glm::vec3 eye,
		glm::vec3 center ,
		glm::vec3 up);
private:
	std::stack<glm::mat4> ms;
};

#endif
