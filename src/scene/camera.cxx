//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "scene/camera.hxx"
#include "scene/document.hxx"

#include <glm/ext.hpp>

namespace com::scene
{
    Camera::Camera(uint32_t const width, uint32_t const height, float const fov, QObject* parent)
        : QObject(parent), m_viewport(static_cast<float>(width), static_cast<float>(height)), m_fov(glm::radians(fov))
    {
        update();
    }

    void Camera::fitToDocument(Document const* document)
    {
        auto const bounds      = document->bounds();
        auto const longestEdge = bounds.getLongestEdge();
        m_centre               = bounds.getCenter();
        m_eye                  = m_centre + longestEdge;
        m_direction            = m_eye - m_centre;

        m_viewMatrix       = glm::lookAt(m_eye, m_centre, { 0.0f, 1.0f, 0.0f });
        m_projectionMatrix = glm::perspective(m_fov, m_viewport.x / m_viewport.y, 0.001f, longestEdge * 100.0f);
        m_isDirty          = true;

        update();
    }

    void Camera::processMovement()
    {
        // auto const delta = m_lastPoint - m_initialPoint;
        update();
    }

    void Camera::processMovement(float const factor)
    {
        m_eye += m_direction * factor;
        m_isDirty = true;

        update();
    }

    void Camera::update()
    {
        if (m_isDirty)
        {
            m_viewMatrix           = glm::lookAt(m_eye, m_centre, { 0.0f, 1.0f, 0.0f });
            m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;

            m_isDirty = false;
        }
    }

} // namespace com::scene
