#include "hyperflow.h"
#include "hinternal.h"

namespace hf::gridlines
{
    void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info)
    {
        rn->Start_ShaderSetup(inter::HF.staticResources.axisLinesShaderSetup);
        {
            Camera3DCore::BindCurrentToUniform(rn);

            const ShaderBindingInfo shaderInfo
            {
                .shader = inter::HF.staticResources.axisLinesShader,
                .attrib = inter::HF.staticResources.quadAttrib,
                .bindingPoint = RenderBindingType::Graphics
            };

            rn->Start_Shader(shaderInfo);
            {
                rn->Start_Material(inter::HF.staticResources.emptyMaterial);
                {
                    rn->Start_Draw();
                    {
                        rn->DrawSet_PushConstant(&info, sizeof(GridLinesInfo));

                        const DrawCallInfo drawCallInfo
                        {
                            .pVertBuffers = &inter::HF.staticResources.quadBuffer,
                            .bufferCount = 1,
                            .indexBuffer = nullptr,
                            .instanceCount = 1
                        };

                        rn->DrawAdd_DrawCall(drawCallInfo);
                    }
                    rn->End_Draw();
                }
                rn->End_Material();
            }
            rn->End_Shader();
        }
        rn->End_ShaderSetup();
    }
}