//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "scene/document.hxx"
#include "base/preferences.hxx"
#include "rhi/per-frame-data.hxx"
#include "rhi/primitive.hxx"
#include "rhi/shaders/uniforms.hxx"
#include "rhi/utilities.hxx"

#include <QFileInfo>

namespace com::scene
{
    enum ShaderKind : uint32_t
    {
        ShaderCursorVertex,
        ShaderCursorFragment,
        ShaderModelVertex,
        ShaderModelFragment,
        ShaderHitTestVertex,
        ShaderHitTestFragment,
        ShaderKindCount
    };

    Document::Document(rhi::Context* context, vk::Extent2D const& extent, QObject* parent) : QObject(parent), m_context(context), m_extent(extent)
    {
        auto const radius      = base::Preferences::read(base::PreferenceType::PrimitiveRadius).toFloat();
        auto const minPolygons = base::Preferences::read(base::PreferenceType::MinimumPrimitivePolygonCount).toUInt();
        m_models.emplace_back(std::make_unique<Model>(rhi::makeSphere(m_context, { 0.0f, 0.0f, 0.0f }, radius, minPolygons)));

        auto const cursorVertexCount = base::Preferences::read(base::PreferenceType::CursorVertexCount).toUInt();
        m_cursor                     = std::make_unique<Model>(rhi::makeCursor(m_context, cursorVertexCount));

        m_descriptorSetLayouts.resize(PipelineIndexCount);
        m_descriptorPools.resize(PipelineIndexCount);
        m_descriptorSets.resize(PipelineIndexCount);
        m_pipelineLayouts.resize(PipelineIndexCount);
        m_pipelines.resize(PipelineIndexCount);
        m_shaders.resize(ShaderKindCount);

        resize(extent);
    }

    Document::~Document()
    {
        destroyHitTestPipeline();
        destroyModelPipeline();
        destroyCursorPipeline();
    }

    auto Document::bounds() const -> AABB
    {
        AABB result;

        for (auto const& mesh : m_models)
            result.extend(mesh->bounds());

        return result;
    }

    auto Document::name() -> QString
    {
        if (!m_path.isEmpty())
        {
            QFileInfo info(m_path);
            return info.baseName();
        }

        return tr("Untitled");
    }

    void Document::render(Camera const* camera, vk::CommandBuffer const& commandBuffer)
    {
        auto index = PipelineIndexModel;

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[index]);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayouts[index], 0, { m_descriptorSets[index] }, nullptr);

        for (auto const& model : m_models)
        {
            uploadUniforms(camera, model->transform(), index);
            model->render(commandBuffer);
        }

        if (m_hit)
        {
            index = PipelineIndexCursor;

            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[index]);
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayouts[index], 0, { m_descriptorSets[index] }, nullptr);

            uploadUniforms(camera, { 1.0f }, index);
            m_cursor->render(commandBuffer);
        }
    }

    auto Document::save(QString const) -> bool
    {
        return true;
    }

    void Document::updateHitTestQuery(Camera const* camera, vk::Rect2D const& rect)
    {
        auto*       frameData     = m_context->frameData();
        auto const& commandBuffer = frameData->commandBuffer();

        if (m_shouldUpdateHitBuffer)
        {
            m_hitDepth->transition(rhi::Image::Usage::eAttachmentReadWrite, commandBuffer);
            m_hitNormal->transition(rhi::Image::Usage::eAttachmentWriteOnly, commandBuffer);

            renderHitTesting(rect, camera, commandBuffer);

            m_hitDepth->transition(rhi::Image::Usage::eTransferSrc, commandBuffer);
            m_hitNormal->transition(rhi::Image::Usage::eTransferSrc, commandBuffer);

            m_shouldUpdateHitBuffer = false;

            auto const point = camera->lastPoint();
            m_hitDepth->copyPixel(point.x(), point.y(), 0, commandBuffer, frameData->mouseBuffer());
            m_hitNormal->copyPixel(point.x(), point.y(), 4, commandBuffer, frameData->mouseBuffer());
            m_hit = rhi::createMouseHit(point.x(), point.y(), m_extent.width, m_extent.height, frameData->mouseBuffer());
        }
    }

    void Document::uploadUniforms(Camera const* camera, glm::mat4 const& modelTransform, PipelineIndex const index)
    {
        auto const& device    = m_context->device()->logicalDevice();
        auto*       frameData = m_context->frameData();

        CameraUniform cameraParams = { camera->viewProjection(), camera->eye() };
        auto*         cameraBuffer = frameData->cameraUniformBuffer();
        cameraBuffer->upload(cameraParams);

        auto* modelBuffer = frameData->modelUniformBuffer();
        modelBuffer->upload(modelTransform);

        std::vector<rhi::DescriptorUpdate> updateSet;
        updateSet.emplace_back(vk::DescriptorType::eUniformBuffer, cameraBuffer->buffer(), VK_WHOLE_SIZE, vk::BufferView());
        updateSet.emplace_back(vk::DescriptorType::eUniformBuffer, modelBuffer->buffer(), VK_WHOLE_SIZE, vk::BufferView());
        rhi::updateDescriptorSets(device, m_descriptorSets[index], updateSet);
    }

    void Document::createCursorPipeline()
    {
        destroyCursorPipeline();

        auto const& device = m_context->device()->logicalDevice();
        auto const  i      = PipelineIndexCursor;

        std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = { { vk::DescriptorType::eUniformBuffer, 1 }, { vk::DescriptorType::eUniformBuffer, 1 } };
        m_descriptorPools[i]                                    = rhi::createDescriptorPool(device, descriptorPoolSizes);

        std::vector<rhi::DescriptorSetDescription> descriptorSetDescription = {
            { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment },
            { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment }
        };
        m_descriptorSetLayouts[i] = rhi::createDescriptorSetLayout(device, descriptorSetDescription);

        m_descriptorSets[i]  = device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(m_descriptorPools[i], m_descriptorSetLayouts[i])).front();
        m_pipelineLayouts[i] = device.createPipelineLayout(vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), m_descriptorSetLayouts[i]));

        m_shaders[ShaderCursorVertex]   = rhi::createShader(device, "cursor.vert");
        m_shaders[ShaderCursorFragment] = rhi::createShader(device, "cursor.frag");

        std::vector<vk::Format> colorformats        = { m_context->colorFormat() };
        auto const              renderingCreateInfo = vk::PipelineRenderingCreateInfo({}, colorformats, m_context->depthFormat());
        auto const vertexAttributes = std::vector<std::pair<vk::Format, size_t>>{ { std::make_pair(vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3)) } };
        m_pipelines[i]              = rhi::createGraphicsPipeline(m_context,
                                                     vertexAttributes,
                                                     m_shaders[ShaderCursorVertex],
                                                     m_shaders[ShaderCursorFragment],
                                                     vk::FrontFace::eClockwise,
                                                     vk::PrimitiveTopology::eLineList,
                                                     true,
                                                     renderingCreateInfo,
                                                     m_pipelineLayouts[i]);
    }

    void Document::createHitTestPipeline()
    {
        destroyHitTestPipeline();

        auto const& device = m_context->device()->logicalDevice();
        auto const  i      = PipelineIndexHitTest;

        m_hitDepth = std::make_unique<rhi::Image>(m_context->device(),
                                                  m_extent,
                                                  vk::Format::eD32Sfloat,
                                                  vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc);

        m_hitNormal = std::make_unique<rhi::Image>(m_context->device(),
                                                   m_extent,
                                                   vk::Format::eR32Uint,
                                                   vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc);

        std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = { { vk::DescriptorType::eUniformBuffer, 1 }, { vk::DescriptorType::eUniformBuffer, 1 } };
        m_descriptorPools[i]                                    = rhi::createDescriptorPool(device, descriptorPoolSizes);

        std::vector<rhi::DescriptorSetDescription> descriptorSetDescription = {
            { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment },
            { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment }
        };
        m_descriptorSetLayouts[i] = rhi::createDescriptorSetLayout(device, descriptorSetDescription);

        m_descriptorSets[i]  = device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(m_descriptorPools[i], m_descriptorSetLayouts[i])).front();
        m_pipelineLayouts[i] = device.createPipelineLayout(vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), m_descriptorSetLayouts[i]));

        m_shaders[ShaderHitTestVertex]   = rhi::createShader(device, "hit-test.vert");
        m_shaders[ShaderHitTestFragment] = rhi::createShader(device, "hit-test.frag");

        std::vector<vk::Format> colorformats        = { vk::Format::eR32Uint };
        auto const              renderingCreateInfo = vk::PipelineRenderingCreateInfo({}, colorformats, m_context->depthFormat());
        auto const              vertexAttributes    = std::vector<std::pair<vk::Format, size_t>>{ //
                                                                                  { std::make_pair(vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3)) },
                                                                                  { std::make_pair(vk::Format::eR8G8B8A8Unorm, sizeof(uint32_t)) }
        };
        m_pipelines[i] = rhi::createGraphicsPipeline(m_context,
                                                     vertexAttributes,
                                                     m_shaders[ShaderHitTestVertex],
                                                     m_shaders[ShaderHitTestFragment],
                                                     vk::FrontFace::eClockwise,
                                                     vk::PrimitiveTopology::eTriangleList,
                                                     true,
                                                     renderingCreateInfo,
                                                     m_pipelineLayouts[i]);
    }

    void Document::createModelPipeline()
    {
        destroyModelPipeline();

        auto const& device = m_context->device()->logicalDevice();
        auto const  i      = PipelineIndexModel;

        std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = { { vk::DescriptorType::eUniformBuffer, 1 }, { vk::DescriptorType::eUniformBuffer, 1 } };
        m_descriptorPools[i]                                    = rhi::createDescriptorPool(device, descriptorPoolSizes);

        std::vector<rhi::DescriptorSetDescription> descriptorSetDescription = {
            { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment },
            { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment }
        };
        m_descriptorSetLayouts[i] = rhi::createDescriptorSetLayout(device, descriptorSetDescription);

        m_descriptorSets[i]  = device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(m_descriptorPools[i], m_descriptorSetLayouts[i])).front();
        m_pipelineLayouts[i] = device.createPipelineLayout(vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), m_descriptorSetLayouts[i]));

        m_shaders[ShaderModelVertex]   = rhi::createShader(device, "model.vert");
        m_shaders[ShaderModelFragment] = rhi::createShader(device, "model.frag");

        std::vector<vk::Format> colorformats        = { m_context->colorFormat() };
        auto const              renderingCreateInfo = vk::PipelineRenderingCreateInfo({}, colorformats, m_context->depthFormat());
        auto const              vertexAttributes    = std::vector<std::pair<vk::Format, size_t>>{ //
                                                                                  { std::make_pair(vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3)) },
                                                                                  { std::make_pair(vk::Format::eR8G8B8A8Unorm, sizeof(uint32_t)) }
        };
        m_pipelines[i] = rhi::createGraphicsPipeline(m_context,
                                                     vertexAttributes,
                                                     m_shaders[ShaderModelVertex],
                                                     m_shaders[ShaderModelFragment],
                                                     vk::FrontFace::eClockwise,
                                                     vk::PrimitiveTopology::eTriangleList,
                                                     true,
                                                     renderingCreateInfo,
                                                     m_pipelineLayouts[i]);
    }

    void Document::destroyCursorPipeline()
    {
        destroyPipeline(PipelineIndexCursor, m_shaders[ShaderCursorVertex], m_shaders[ShaderCursorFragment]);
    }

    void Document::destroyHitTestPipeline()
    {
        destroyPipeline(PipelineIndexHitTest, m_shaders[ShaderHitTestVertex], m_shaders[ShaderHitTestFragment]);

        m_hitDepth.reset();
        m_hitNormal.reset();
    }

    void Document::destroyModelPipeline()
    {
        destroyPipeline(PipelineIndexModel, m_shaders[ShaderModelVertex], m_shaders[ShaderModelFragment]);
    }

    void Document::destroyPipeline(PipelineIndex const index, vk::ShaderModule& vertex, vk::ShaderModule& fragment)
    {
        auto const& device = m_context->device()->logicalDevice();

        device.destroyShaderModule(vertex);
        device.destroyShaderModule(fragment);

        device.destroyPipelineLayout(m_pipelineLayouts[index]);
        device.destroyDescriptorPool(m_descriptorPools[index]);
        device.destroyDescriptorSetLayout(m_descriptorSetLayouts[index]);

        device.destroyPipeline(m_pipelines[index]);
    }

    void Document::renderHitTesting(vk::Rect2D const& rect, Camera const* camera, vk::CommandBuffer const& commandBuffer)
    {
        std::vector<vk::RenderingAttachmentInfo> attachments;

        // Color attachment.
        auto colourAttachment = m_hitNormal->asRenderingAttachmentInfo();
        colourAttachment.setLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachments.emplace_back(colourAttachment);

        // Depth attachment.
        auto depthAttachment = m_hitDepth->asRenderingAttachmentInfo();
        depthAttachment.setClearValue(vk::ClearDepthStencilValue(0.0f));

        vk::RenderingInfo renderInfo({}, rect, 1u, 0, attachments, &depthAttachment);
        commandBuffer.beginRendering(renderInfo);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[PipelineIndexHitTest]);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                         m_pipelineLayouts[PipelineIndexHitTest],
                                         0, //
                                         { m_descriptorSets[PipelineIndexHitTest] },
                                         nullptr);

        for (auto const& model : m_models)
        {
            uploadUniforms(camera, model->transform(), PipelineIndexHitTest);
            model->render(commandBuffer);
        }

        commandBuffer.endRendering();
    }

} // namespace com::scene
