#include "hyperflow.h"
#include "hinternal.h"

namespace hf::gridlines
{
    void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info)
    {
        Start_ShaderSetup(rn, inter::HF.staticResources.axisLinesShaderSetup);
        {
            primitives::BindGlobalUniformBuffer(rn);

            const ShaderBindingInfo shaderInfo
            {
                .shader = inter::HF.staticResources.axisLinesShader,
                .attrib = inter::HF.staticResources.quadAttrib,
                .bindingPoint = RenderBindingType::Graphics
            };

            Start_Shader(rn, shaderInfo);
            {
                Start_Material(rn, inter::HF.staticResources.emptyMaterial);
                {
                    Start_Draw(rn);
                    {
                        DrawSet_PushConstant(rn, &info, sizeof(GridLinesInfo));

                        const DrawCallInfo drawCallInfo
                        {
                            .pVertBuffers = &inter::HF.staticResources.quadBuffer,
                            .bufferCount = 1,
                            .indexBuffer = nullptr,
                            .instanceCount = 1
                        };

                        DrawAdd_DrawCall(rn, drawCallInfo);
                    }
                    End_Draw(rn);
                }
                End_Material(rn);
            }
            End_Shader(rn);
        }
        End_ShaderSetup(rn);
    }
}