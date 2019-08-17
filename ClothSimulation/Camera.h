#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define PI 3.14159265354

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	float follow_r;  // radius
	float follow_a;  // angle
	float follow_h;  // height

	float follow_yaw;
	float follow_pitch;

	glm::vec3 dir1;
	glm::vec3 local_front, local_up, local_right;

	glm::vec3 look;  // look at of follow 

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		//Position = position;
		//Front = glm::normalize(glm::vec3(0.0, 0.0, 0.0) - Position);
		WorldUp = up;
		//Yaw = yaw;
		//Pitch = pitch;
		//updateCameraVectors();
		initFollow();
		setFollowPos();
	}

	void initFollow()
	{
		follow_r = 7.0;
		follow_a = 0.0;
		follow_h = 2.0;
		follow_yaw = 90.0;
		follow_pitch = 0.0;
	}

	void setFollowPos()
	{
		Position[0] = follow_r * sin(follow_a / 180.0*PI);
		Position[1] = follow_h;
		Position[2] = follow_r * cos(follow_a / 180.0*PI);

		look[0] = 0.0;
		look[1] = 0.0;
		look[2] = 0.0;

		local_front = glm::normalize(look - Position);
		local_right = glm::normalize(glm::cross(local_front, WorldUp));
		local_up = glm::normalize(glm::cross(local_right, local_front));

		dir1.x = cos(glm::radians(follow_yaw)) * cos(glm::radians(follow_pitch));
		dir1.y = sin(glm::radians(follow_pitch));
		dir1.z = sin(glm::radians(follow_yaw)) * cos(glm::radians(follow_pitch));

		Front = glm::normalize(dir1.x*local_right + dir1.y*local_up + dir1.z*local_front);
		updateCameraVectorsFollow();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			follow_r -= velocity;
		if (direction == BACKWARD)
			follow_r += velocity;
		if (direction == LEFT)
			follow_a -= 5.0*velocity;
		if (direction == RIGHT)
			follow_a += 5.0*velocity;
		if (direction == TOP)
			follow_h += velocity;
		if (direction == BOTTOM)
			follow_h -= velocity;
		//if (direction == FORWARD)
		//	Position += Front * velocity;
		//if (direction == BACKWARD)
		//	Position -= Front * velocity;
		//if (direction == LEFT)
		//	Position -= Right * velocity;
		//if (direction == RIGHT)
		//	Position += Right * velocity;
		//if (direction == TOP)
		//	Position += Up * velocity;
		//if (direction == BOTTOM)
		//	Position -= Up * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity * tanh(Zoom / 45.0*5.0);
		yoffset *= MouseSensitivity * tanh(Zoom / 45.0*5.0);

		Yaw += xoffset;
		Pitch += yoffset;

		follow_yaw += xoffset;
		follow_pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}

	void updateCameraVectorsFollow()
	{
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif