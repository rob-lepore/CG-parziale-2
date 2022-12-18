#ifndef __CAMERA__
#define __CAMERA__
#include "Lib.h"

class Camera {
public:
	vec4 position;
	vec4 target;
	vec4 upVector;
	vec4 direction;

	float cameraSpeed = 0.1;


	void moveForward()
	{

		vec4 direction = target - position;    //Direzione lungo cui si sposta la telecamera in coordinate del mondo
		position += direction * cameraSpeed;
	}

	void moveBack()
	{

		vec4 direction = target - position; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
		position -= direction * cameraSpeed;
	}

	void moveLeft()
	{
		vec3 direction = target - position; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
		vec3 slide_vector = cross(direction, vec3(upVector)) * cameraSpeed;
		position -= vec4(slide_vector, .0);
		target -= vec4(slide_vector, .0);
	}

	void moveRight()
	{

		vec3 direction = target - position;  //Direzione lungo cui si sposta la telecamera in coordinate del mondo
		vec3 slide_vector = cross(direction, vec3(upVector)) * cameraSpeed;
		position += vec4(slide_vector, .0);
		target += vec4(slide_vector, .0);
	}

	void moveUp()
	{

		vec3 direction = target - position;  //Direzione lungo cui si sposta la telecamera in coordinate del mondo
		vec3 slide_vector = normalize(glm::cross(direction, vec3(upVector)));
		vec3 upDirection = cross(direction, slide_vector) * cameraSpeed;
		position -= vec4(upDirection, 0.0);
		target -= vec4(upDirection, 0.0);
	}

	void moveDown()
	{

		vec4 direction = target - position;
		vec3 slide_vector = normalize(cross(vec3(direction), vec3(upVector)));
		vec3 upDirection = cross(vec3(direction), slide_vector) * cameraSpeed;
		position += vec4(upDirection, 0.0);
		target += vec4(upDirection, 0.0);
	}
};

#endif