/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * LCG - LEZIONI COMPUTER GRAFICA
 * (C) 2025 Copyright by Michele Iacobellis
 * A project for students...
 *
 * This file is part of LCG.
 *
 * LCG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCG. If not, see <http://www.gnu.org/licenses/>.
 *
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#ifndef lcg_camera_h
#define lcg_camera_h

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#include "lcg_priv.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DICHIARATIVE DI STRUTTURA/CLASSE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace lcg {
  class Camera {
  public:
    enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };
    glm::vec3 position;
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    
    Camera(glm::vec3 position)
    : position        {                  position }
    , yaw             {                    -90.0f }
    , pitch           {                      0.0f }
    , movementSpeed   {                      2.5f }
    , mouseSensitivity{                      0.1f }
    , m_vWorldUp      {glm::vec3(0.0f, 1.0f, 0.0f)}
    {
      m_UpdateVectors();
    }
    
    glm::mat4 viewMatrix() const {
      return glm::lookAt(position, position + m_vFront, m_vUp);
    }
    
    void processKeyboard(CameraMovement direction, float deltaTime) {
      float velocity = movementSpeed * deltaTime;
      if (direction == FORWARD)
        position += m_vFront * velocity;
      if (direction == BACKWARD)
        position -= m_vFront * velocity;
      if (direction == LEFT)
        position -= m_vRight * velocity;
      if (direction == RIGHT)
        position += m_vRight * velocity;
    }
    
    void processMouseMovement(float xoffset, float yoffset
      , GLboolean constrainPitch = true)
    {
      xoffset *= mouseSensitivity;
      yoffset *= mouseSensitivity;
      
      yaw   += xoffset;
      pitch += yoffset;
      
      if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
      }
      
      m_UpdateVectors();
    }
    
    glm::vec3 front() const { return m_vFront; }
    
  private:
    glm::vec3 m_vFront;
    glm::vec3 m_vUp;
    glm::vec3 m_vRight;
    glm::vec3 m_vWorldUp;
    
    void m_UpdateVectors() {
      glm::vec3 front;
      front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      front.y = sin(glm::radians(pitch));
      front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      m_vFront = glm::normalize(front);
      m_vRight = glm::normalize(glm::cross(m_vFront, m_vWorldUp));
      m_vUp    = glm::normalize(glm::cross(m_vRight, m_vFront));
    }
  };
}

#endif // lcg_camera_h

