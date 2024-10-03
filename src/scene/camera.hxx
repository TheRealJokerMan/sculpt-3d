//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QObject>
#include <QPoint>
#include <glm/glm.hpp>

namespace com::scene
{
    /// Defines the current camera mode.
    enum class CameraMode
    {
        None,  ///< None.
        Pick,  ///< Picking.
        Dolly, ///< Zooming in-and-out.
        Orbit, ///< Rotating.
        Truck, ///< Moving left-to-right.
    };

    /// Provides and controls a camera to view the scene.
    class Camera final : public QObject
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param width The width of the viewport.
        /// \param height The height of the viewport.
        /// \param fov The vertical field-of-view (in degrees).
        /// \param parent The parent object, if any.
        explicit Camera(uint32_t const width, uint32_t const height, float const fov = 60.0f, QObject* parent = nullptr);

        /// Get the eye position.
        /// \return A valid vector.
        [[nodiscard]] auto eye() const
        {
            return glm::vec4(m_eye, 1.0f);
        }

        /// Bound the camera to the document's contents.
        void fitToDocument(class Document const* document);

        /// Get the current mode.
        /// \return A valid mode.
        [[nodiscard]] auto mode() const
        {
            return m_mode;
        }

        /// Get the last tracked position.
        /// \return A valid point.
        [[nodiscard]] auto lastPoint() const
        {
            return m_lastPoint;
        }

        /// Process any mouse movement.
        void processMovement();

        /// Process any mouse movement.
        /// \param factor A scalar value.
        void processMovement(float const factor);

        /// Set the mode.
        /// \param mode The mode to set.
        /// \param point The position of the mouse cursor.
        void setMode(CameraMode const mode, QPoint const& point = {})
        {
            m_mode         = mode;
            m_initialPoint = point;
        }

        /// Track the mouse cursor.
        /// \param point The position of the mouse cursor.
        void track(QPoint const& point)
        {
            m_lastPoint = point;
        }

        /// Get the view-projection matrix.
        /// \return A valid matrix.
        [[nodiscard]] auto viewProjection() const -> glm::mat4 const&
        {
            return m_viewProjectionMatrix;
        }

    private:
        void update();

    private:
        CameraMode m_mode = CameraMode::None;
        QPoint     m_initialPoint;
        QPoint     m_lastPoint;
        glm::vec2  m_viewport;
        float      m_fov = 0.0f;
        glm::mat4  m_viewMatrix;
        glm::mat4  m_projectionMatrix;
        glm::mat4  m_viewProjectionMatrix;
        glm::vec3  m_eye;
        glm::vec3  m_centre;
        glm::vec3  m_direction;
        bool       m_isDirty = false;
    };
} // namespace com::scene
