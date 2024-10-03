//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/hit-testing.hxx"
#include "rhi/image.hxx"
#include "scene/camera.hxx"
#include "scene/model.hxx"

#include <QObject>

namespace com::scene
{
    // Forward declaration.
    enum PipelineIndex : uint32_t;

    /// A document is a container for scene state.
    class Document final : public QObject
    {
        Q_OBJECT

    public:
        /// Denotes the pipeline index.
        enum PipelineIndex : uint32_t
        {
            PipelineIndexModel,   ///< Model.
            PipelineIndexCursor,  ///< The cursor.
            PipelineIndexHitTest, ///< Hit-test.
            PipelineIndexCount    ///< Count.
        };

    public:
        /// Constructor.
        /// \param context The RHI context.
        /// \param extent The physical extent of the document.
        /// \param parent The parent object, if any.
        explicit Document(rhi::Context* context, vk::Extent2D const& extent, QObject* parent = nullptr);

        /// Destructor.
        ~Document();

        /// Compute the bounds of the document.
        /// \return A valid bounding box.
        [[nodiscard]] auto bounds() const -> AABB;

        /// Determines if the document has been modified.
        /// \return true if the document has been modified; false otherwise.
        [[nodiscard]] auto isModified()
        {
            return m_isModified;
        }

        /// Get the models in the current file.
        /// \return A valid string.
        [[nodiscard]] auto models() const -> std::vector<std::unique_ptr<Model>> const&
        {
            return m_models;
        }

        /// Get the name of the document, without extension.
        /// \return A valid string.
        [[nodiscard]] auto name() -> QString;

        /// Get the path of the current file.
        /// \return A valid string.
        [[nodiscard]] auto path()
        {
            return m_path;
        }

        /// Render the document.
        /// \param camera The camera.
        /// \param commandBuffer The command buffer.
        void render(Camera const* camera, vk::CommandBuffer const& commandBuffer);

        /// Update the hit-test data on next-frame.
        void requestHitUpdate()
        {
            m_hit.reset();
            m_shouldUpdateHitBuffer = true;
        }

        /// Resize the document.
        /// \param extent The physical extent of the document.
        void resize(vk::Extent2D const& extent)
        {
            m_extent = extent;
            createHitTestPipeline();
            createModelPipeline();
            createCursorPipeline();
        }

        /// Save the document if it has been modified.
        /// \param path A new path to save file.
        /// \return true if the document has not been modified or if it was saved successfully; false otherwise.
        [[nodiscard]] auto save(QString const path = {}) -> bool;

        /// Read the mouse data to see if a hit occured.
        /// \param camera The camera.
        /// \param rect The swap chain rect.
        void updateHitTestQuery(Camera const* camera, vk::Rect2D const& rect);

        /// Upload data to the GPU.
        /// \param camera The camera.
        /// \param modelTransform The model's transform.
        /// \param index The pipeline index.
        void uploadUniforms(Camera const* camera, glm::mat4 const& modelTransform, PipelineIndex const index);

    private:
        void createCursorPipeline();
        void createHitTestPipeline();
        void createModelPipeline();
        void destroyCursorPipeline();
        void destroyHitTestPipeline();
        void destroyModelPipeline();
        void destroyPipeline(PipelineIndex const index, vk::ShaderModule& vertex, vk::ShaderModule& fragment);
        void renderHitTesting(vk::Rect2D const& rect, Camera const* camera, vk::CommandBuffer const& commandBuffer);

    private:
        rhi::Context*                        m_context = nullptr;
        vk::Extent2D                         m_extent;
        std::unique_ptr<Model>               m_cursor;
        bool                                 m_isModified = false;
        std::vector<std::unique_ptr<Model>>  m_models;
        QString                              m_path;
        bool                                 m_shouldUpdateHitBuffer = false;
        std::unique_ptr<rhi::Image>          m_hitDepth;
        std::unique_ptr<rhi::Image>          m_hitNormal;
        std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
        std::vector<vk::DescriptorPool>      m_descriptorPools;
        std::vector<vk::DescriptorSet>       m_descriptorSets;
        std::vector<vk::PipelineLayout>      m_pipelineLayouts;
        std::vector<vk::Pipeline>            m_pipelines;
        std::vector<vk::ShaderModule>        m_shaders;
        std::unique_ptr<rhi::MouseHit>       m_hit;
    };
} // namespace com::scene
