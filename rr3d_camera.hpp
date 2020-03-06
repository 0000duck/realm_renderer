#ifndef __RR3D_CAMERA__
#define __RR3D_CAMERA__

#include <glm.hpp>

class Camera {
public:
	Camera() {};
	Camera(float, float, float, float);
	~Camera();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();
	float* getTranslationX();
	float* getTranslationY();
	float* getTranslationZ();
	void setPitch(float);
	void setYaw(float);
	void setRoll(float);
	void addPitch(float);
	void addYaw(float);
	void addRoll(float);
	void setTranslation(float, float, float);
	glm::vec3 getDirection();
	float getPitch();
	float getYaw();
	float getRoll();
	void move(float);
	void strafe(float);
	void lift(float);
private:
	float pitch;
	float yaw;
	float roll;
	glm::vec3 translation;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat4 P;
	glm::mat4 V;
};

#endif
