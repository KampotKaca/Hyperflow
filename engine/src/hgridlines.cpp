#include "hyperflow.h"
#include "hinternal.h"

namespace hf::dp
{
    void Draw(const GridLinesInfo& info)
    {
        StartShaderLayout(ir::HF.staticResources.shaderLayouts.axisLines);
        {
            BindGlobalUniformBuffer();

            StartShader(ir::HF.staticResources.shaders.axisLines);
            {
                StartMat(nullptr);
                {
                    StartDrawGroup();
                    {
                        DrawGroupSetPushConstant(info);
                        DrawGroupAddDrawCall(nullptr, ir::HF.staticResources.quadBuffer);
                    }
                    EndDrawGroup();
                }
                EndMat();
            }
            EndShader();
        }
        EndShaderLayout();
    }
}