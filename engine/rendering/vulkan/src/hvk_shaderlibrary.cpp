#include "hvk_shaderlibrary.h"
#include "hvk_graphics.h"
#include "hyperflow.h"

namespace hf
{
    static VkShaderModule AddShaderStage(const void* code, uint32_t codeSize, VkShaderStageFlagBits stage, std::vector<VkPipelineShaderStageCreateInfo>& res);

    VkShaderLibrary::VkShaderLibrary(const inter::rendering::ShaderLibraryCreationInfo_i& info) : outputFormats(info.outputFormats)
    {
        uint32_t moduleCount = info.vertexInputModuleCount + info.preRasterModuleCount + info.fragmentModuleCount + info.fragmentOutputModuleCount;
        uint32_t maxShadingModules = info.preRasterModuleCount * 4 + info.fragmentModuleCount;

        std::vector<VkShaderModule> shaderModules{};
        shaderModules.reserve(maxShadingModules);
        std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfos{};
        stageCreateInfos.reserve(maxShadingModules);

        std::vector<VkVertexInputBindingDescription> vertexInputDescriptions{};
        vertexInputDescriptions.reserve(info.vertexInputModuleCount * MAX_VERTEX_INPUT_BUFFER_ATTRIBUTES);
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes{};
        vertexInputAttributes.reserve(info.vertexInputModuleCount * MAX_VERTEX_INPUT_BUFFER_ATTRIBUTES);

        std::vector<VkGraphicsPipelineCreateInfo> pipelineCreateInfos(moduleCount);

        std::vector<VkPipelineVertexInputStateCreateInfo> vertexInputInfos(info.vertexInputModuleCount);
        std::vector<VkPipelineInputAssemblyStateCreateInfo> inputAssemblies(info.vertexInputModuleCount);

        std::vector<VkPipelineRasterizationStateCreateInfo> rasterizers(info.preRasterModuleCount);

        std::vector<VkPipelineDepthStencilStateCreateInfo> depthStencilStates(info.fragmentModuleCount);

        uint32_t colorAttachmentCount = 0;
        for (uint32_t i = 0; i < info.fragmentOutputModuleCount; i++)
            colorAttachmentCount += info.pFragmentOutputModules[i].colorAttachmentCount;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(colorAttachmentCount);
        std::vector<VkPipelineColorBlendStateCreateInfo> colorBlendStateInfos(info.fragmentOutputModuleCount);

        modules = std::vector<VkPipeline>(moduleCount);

        uint32_t moduleIndex = 0;
        colorAttachmentCount = 0;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = (VkSampleCountFlagBits)outputFormats.sampleMode;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        if (outputFormats.sampleMode != MultisampleMode::MSAA_1X &&
            GRAPHICS_DATA.device.features.features.sampleRateShading)
        {
            multisampling.sampleShadingEnable = VK_TRUE;
            multisampling.minSampleShading = VK_MSAA_MIN_SAMPLE_SHADING;
        }
        else
        {
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.minSampleShading = 1.0f;
        }

        VkPipelineRenderingCreateInfo renderingInfo{};
        GetRenderingInfo(outputFormats, renderingInfo);

        VkGraphicsPipelineLibraryCreateInfoEXT vertexInputLibInfo{};
        vertexInputLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        vertexInputLibInfo.pNext = &renderingInfo;
        vertexInputLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT;

        VkGraphicsPipelineLibraryCreateInfoEXT preRasterLibInfo{};
        preRasterLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        preRasterLibInfo.pNext = &renderingInfo;
        preRasterLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkGraphicsPipelineLibraryCreateInfoEXT fragmentLibInfo{};
        fragmentLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        fragmentLibInfo.pNext = &renderingInfo;
        fragmentLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT;

        VkGraphicsPipelineLibraryCreateInfoEXT fragmentOutputLibInfo{};
        fragmentOutputLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        fragmentOutputLibInfo.pNext = &renderingInfo;
        fragmentOutputLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT;

        for (uint32_t i = 0; i < info.vertexInputModuleCount; i++)
        {
            uint32_t descFrom = vertexInputDescriptions.size(),
                     attribFrom = vertexInputAttributes.size();

            uint32_t attribCount = 0;
            auto& moduleInfo = info.pVertexInputModules[i];

            VkPipelineVertexInputStateCreateInfo vInfo{};
            vInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vInfo.vertexBindingDescriptionCount = moduleInfo.attributeCount;

            for (uint32_t k = 0; k < moduleInfo.attributeCount; k++)
            {
                const auto& attribute = GetAttrib(moduleInfo.pAttributes[k]);
                vertexInputDescriptions.push_back(attribute->bindingDescription);

                for (auto attribDescription : attribute->attribDescriptions)
                    vertexInputAttributes.push_back(attribDescription);
                attribCount += attribute->attribDescriptions.size();
            }

            vInfo.pVertexBindingDescriptions = &vertexInputDescriptions[descFrom];
            vInfo.vertexAttributeDescriptionCount = attribCount;
            vInfo.pVertexAttributeDescriptions = &vertexInputAttributes[attribFrom];
            vertexInputInfos[i] = vInfo;

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = (VkPrimitiveTopology)moduleInfo.topology;
            inputAssembly.primitiveRestartEnable = moduleInfo.enablePrimitiveRestart;
            inputAssemblies[i] = inputAssembly;

            VkGraphicsPipelineCreateInfo vertexInputPipelineInfo{};
            vertexInputPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            vertexInputPipelineInfo.pNext = &vertexInputLibInfo;
            vertexInputPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            vertexInputPipelineInfo.pVertexInputState = &vertexInputInfos[i];
            vertexInputPipelineInfo.pInputAssemblyState = &inputAssemblies[i];

            pipelineCreateInfos[moduleIndex] = vertexInputPipelineInfo;
            moduleIndex++;
        }

#define ADD_MODULE(x)\
        {\
            auto module = x;\
            if (module)\
            {\
                shaderModules.push_back(module);\
                stageCount++;\
            }\
        }

        for (uint32_t i = 0; i < info.preRasterModuleCount; i++)
        {
            const auto& moduleInfo = info.pPreRasterModules[i];

            uint32_t from = stageCreateInfos.size();
            uint32_t stageCount = 0;
            ADD_MODULE(AddShaderStage(moduleInfo.vertexShaderCode, moduleInfo.vertexShaderCodeSize, VK_SHADER_STAGE_VERTEX_BIT, stageCreateInfos))
            ADD_MODULE(AddShaderStage(moduleInfo.tessellationControlShaderCode, moduleInfo.tessellationControlShaderCodeSize, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, stageCreateInfos));
            ADD_MODULE(AddShaderStage(moduleInfo.tessellationEvaluationShaderCode, moduleInfo.tessellationEvaluationShaderCodeSize, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, stageCreateInfos));
            ADD_MODULE(AddShaderStage(moduleInfo.geometryShaderCode, moduleInfo.geometryShaderCodeSize, VK_SHADER_STAGE_GEOMETRY_BIT, stageCreateInfos));

            VkPipelineRasterizationStateCreateInfo rasterInfo{};
            rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterInfo.depthClampEnable = moduleInfo.options.enableDepthClamping;
            rasterInfo.rasterizerDiscardEnable = moduleInfo.options.enableRasterizerDiscard;
            rasterInfo.polygonMode = (VkPolygonMode)moduleInfo.options.polygonMode;
            rasterInfo.cullMode = (VkCullModeFlags)moduleInfo.options.cullMode;
            rasterInfo.frontFace = (VkFrontFace)moduleInfo.options.faceDirection;
            rasterInfo.lineWidth = moduleInfo.options.lineWidth;

            if (moduleInfo.options.biasOptions.has_value())
            {
                auto options = moduleInfo.options.biasOptions.value();
                rasterInfo.depthBiasEnable = VK_TRUE;
                rasterInfo.depthBiasConstantFactor = options.constantFactor;
                rasterInfo.depthBiasClamp = options.clamp;
                rasterInfo.depthBiasSlopeFactor = options.slopeFactor;
            }
            else rasterInfo.depthBiasEnable = VK_FALSE;

            rasterizers[i] = rasterInfo;

            auto& layout = GetShaderLayout(moduleInfo.layout);

            VkGraphicsPipelineCreateInfo preRasterPipelineInfo{};
            preRasterPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            preRasterPipelineInfo.pNext = &preRasterLibInfo;
            preRasterPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            preRasterPipelineInfo.stageCount = stageCount;
            preRasterPipelineInfo.pStages = &stageCreateInfos[from];
            preRasterPipelineInfo.pViewportState = &viewportState;
            preRasterPipelineInfo.pRasterizationState = &rasterizers[i];
            preRasterPipelineInfo.pDynamicState = &SHADER_DYNAMIC;
            preRasterPipelineInfo.layout = layout->layout;
            pipelineCreateInfos[moduleIndex] = preRasterPipelineInfo;

            moduleIndex++;
        }

        for (uint32_t i = 0; i < info.fragmentModuleCount; i++)
        {
            const auto& moduleInfo = info.pFragmentModules[i];

            uint32_t from = stageCreateInfos.size();
            uint32_t stageCount = 0;
            ADD_MODULE(AddShaderStage(moduleInfo.fragmentShaderCode, moduleInfo.fragmentShaderCodeSize, VK_SHADER_STAGE_FRAGMENT_BIT, stageCreateInfos));

            VkPipelineDepthStencilStateCreateInfo depthStencilState{};
            depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilState.depthTestEnable = moduleInfo.depthStencilOptions.enableDepth;
            depthStencilState.depthWriteEnable = moduleInfo.depthStencilOptions.writeDepth;
            depthStencilState.depthCompareOp = (VkCompareOp)moduleInfo.depthStencilOptions.comparisonFunc;
            depthStencilState.depthBoundsTestEnable = moduleInfo.depthStencilOptions.enableDepthBounds;
            depthStencilState.stencilTestEnable = moduleInfo.depthStencilOptions.enableStencil;
            depthStencilState.front = (VkStencilOpState)(VkStencilOp)moduleInfo.depthStencilOptions.frontStencil;
            depthStencilState.back = (VkStencilOpState)(VkStencilOp)moduleInfo.depthStencilOptions.backStencil;
            depthStencilState.minDepthBounds = moduleInfo.depthStencilOptions.depthBounds.x;
            depthStencilState.maxDepthBounds = moduleInfo.depthStencilOptions.depthBounds.y;
            depthStencilStates[i] = depthStencilState;

            VkGraphicsPipelineCreateInfo fragmentPipelineInfo{};
            fragmentPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            fragmentPipelineInfo.pNext = &fragmentLibInfo;
            fragmentPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            fragmentPipelineInfo.stageCount = stageCount;
            fragmentPipelineInfo.pStages = &stageCreateInfos[from];
            fragmentPipelineInfo.pMultisampleState = &multisampling;
            fragmentPipelineInfo.pDepthStencilState = &depthStencilStates[i];
            fragmentPipelineInfo.layout = GetShaderLayout(moduleInfo.layout)->layout;
            pipelineCreateInfos[moduleIndex] = fragmentPipelineInfo;

            moduleIndex++;
        }

#undef ADD_MODULE

        for (uint32_t i = 0; i < info.fragmentOutputModuleCount; i++)
        {
            const auto& moduleInfo = info.pFragmentOutputModules[i];

            uint32_t colorAttachmentStart = colorAttachmentCount;
            for (uint32_t j = 0; j < moduleInfo.colorAttachmentCount; j++)
            {
                auto& cInfo = moduleInfo.pColorAttachmentsSettings[j];
                VkPipelineColorBlendAttachmentState colorBlendAttachment{};
                colorBlendAttachment.colorWriteMask = (VkColorComponentFlags)cInfo.colorWriteMask;

                if (cInfo.blendingOptions.has_value())
                {
                    auto blending = cInfo.blendingOptions.value();
                    colorBlendAttachment.blendEnable = VK_TRUE;
                    colorBlendAttachment.srcColorBlendFactor = (VkBlendFactor)blending.color.srcFactor;
                    colorBlendAttachment.dstColorBlendFactor = (VkBlendFactor)blending.color.dstFactor;
                    colorBlendAttachment.colorBlendOp = (VkBlendOp)blending.color.op;
                    colorBlendAttachment.srcAlphaBlendFactor = (VkBlendFactor)blending.alpha.srcFactor;
                    colorBlendAttachment.dstAlphaBlendFactor = (VkBlendFactor)blending.alpha.dstFactor;
                    colorBlendAttachment.alphaBlendOp = (VkBlendOp)blending.alpha.op;
                }
                else colorBlendAttachment.blendEnable = VK_FALSE;
                colorBlendAttachments[colorAttachmentCount] = colorBlendAttachment;
                colorAttachmentCount++;
            }

            VkPipelineColorBlendStateCreateInfo blendAttachmentInfo{};
            blendAttachmentInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

            if (moduleInfo.blendOp.has_value())
            {
                blendAttachmentInfo.logicOpEnable = VK_TRUE;
                blendAttachmentInfo.logicOp = (VkLogicOp)moduleInfo.blendOp.value();
            }
            else blendAttachmentInfo.logicOpEnable = VK_FALSE;

            blendAttachmentInfo.attachmentCount = moduleInfo.colorAttachmentCount;
            blendAttachmentInfo.pAttachments = &colorBlendAttachments[colorAttachmentStart];
            colorBlendStateInfos[i] = blendAttachmentInfo;

            VkGraphicsPipelineCreateInfo fragmentOutputPipelineInfo{};
            fragmentOutputPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            fragmentOutputPipelineInfo.pNext = &fragmentOutputLibInfo;
            fragmentOutputPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            fragmentOutputPipelineInfo.pMultisampleState = &multisampling;
            fragmentOutputPipelineInfo.pColorBlendState = &colorBlendStateInfos[i];

            pipelineCreateInfos[moduleIndex] = fragmentOutputPipelineInfo;
            moduleIndex++;
        }

        VkPipelineCache cache{};

#if defined(VK_ENABLE_PIPELINE_CACHES)
        std::string cachePath = TO_RES_PATH(std::string(info.uniqueLibraryName) + ".cache");
        std::vector<char> cacheData{};

        {
            VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
            pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

            if (GRAPHICS_DATA.platform.functions.fileExistsFunc(cachePath.c_str()) &&
                GRAPHICS_DATA.platform.functions.readFileFunc(cachePath, cacheData) &&
                !cacheData.empty())
            {
                pipelineCacheCreateInfo.initialDataSize = cacheData.size();
                pipelineCacheCreateInfo.pInitialData = cacheData.data();
            }
            else
            {
                pipelineCacheCreateInfo.initialDataSize = 0;
                pipelineCacheCreateInfo.pInitialData = nullptr;
            }

            auto result = vkCreatePipelineCache(GRAPHICS_DATA.device.logicalDevice.device,
                                                &pipelineCacheCreateInfo, &GRAPHICS_DATA.platform.allocator, &cache);
            if (result != VK_SUCCESS)
            {
                LOG_WARN("[Hyperflow] Failed to load pipeline cache, probably file was corrupted %s", info.uniqueLibraryName);
                cache = VK_NULL_HANDLE;
            }

            pipelineCacheCreateInfo.pInitialData = nullptr;
            pipelineCacheCreateInfo.initialDataSize = 0;
        }
#endif

        VK_HANDLE_EXCEPT(vkCreateGraphicsPipelines(GRAPHICS_DATA.device.logicalDevice.device,
                             cache, (uint32_t)pipelineCreateInfos.size(), pipelineCreateInfos.data(),
                             &GRAPHICS_DATA.platform.allocator, modules.data()));

#if defined(VK_ENABLE_PIPELINE_CACHES)
        {
            size_t cacheSize = 0;
            {
                auto result = vkGetPipelineCacheData(GRAPHICS_DATA.device.logicalDevice.device,
                                                     cache, &cacheSize, nullptr);
                if (result != VK_SUCCESS)
                {
                    LOG_WARN("[Hyperflow] Failed to request cache size to store for future use! %s", info.uniqueLibraryName);
                    cacheSize = 0;
                }
            }

            cacheData.resize(cacheSize);
            auto result = vkGetPipelineCacheData(GRAPHICS_DATA.device.logicalDevice.device,
                                                 cache, &cacheSize, cacheData.data());
            if (result != VK_SUCCESS)
                LOG_WARN("[Hyperflow] Failed to read cache to store for future use! %s", info.uniqueLibraryName);
            else
            {
                if(!GRAPHICS_DATA.platform.functions.writeFileFunc(cachePath, cacheData))
                    LOG_WARN("[Hyperflow] Failed to write cache to store for future use! %s", info.uniqueLibraryName);
                else LOG_LOG("[Hyperflow] Successfully wrote cache which will speed up the next pipeline read. %s", info.uniqueLibraryName);
            }
        }

        if (cache != VK_NULL_HANDLE)
        {
            vkDestroyPipelineCache(GRAPHICS_DATA.device.logicalDevice.device, cache, &GRAPHICS_DATA.platform.allocator);
            cache = VK_NULL_HANDLE;
        }
#endif

        for (auto& shaderModule : shaderModules)
            vkDestroyShaderModule(GRAPHICS_DATA.device.logicalDevice.device, shaderModule, &GRAPHICS_DATA.platform.allocator);
    }

    VkShaderLibrary::~VkShaderLibrary()
    {
        for (const auto& module : modules)
            vkDestroyPipeline(GRAPHICS_DATA.device.logicalDevice.device, module, &GRAPHICS_DATA.platform.allocator);
        modules.clear();
    }

    VkShaderModule AddShaderStage(const void* code, uint32_t codeSize, VkShaderStageFlagBits stage, std::vector<VkPipelineShaderStageCreateInfo>& res)
    {
        if (codeSize > 0 && code != nullptr)
        {
            VkShaderModule module{};
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = codeSize;
            createInfo.pCode = (uint32_t*)code;
            VK_HANDLE_EXCEPT(vkCreateShaderModule(GRAPHICS_DATA.device.logicalDevice.device, &createInfo, &GRAPHICS_DATA.platform.allocator, &module));

            VkPipelineShaderStageCreateInfo stageInfo{};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = stage;
            stageInfo.module = module;
            stageInfo.pName = "main";

            res.push_back(stageInfo);
            return module;
        }

        return nullptr;
    }

    VkPipeline GetShaderLibraryModule(const VkShaderLibrary* lib, uint32_t moduleId)
    {
        if (lib->modules.size() > moduleId) return lib->modules[moduleId];
        throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access out of bounds shader module! ID: %i", moduleId);
    }
}
