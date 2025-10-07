#pragma once
// used: stronghandle
#include "../../sdk/datatypes/stronghandle.h"

static constexpr char szVMatBufferFlatVisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
        {
            Shader = "csgo_unlitgeneric.vfx"
        
            F_IGNOREZ = 0
             F_DISABLE_Z_WRITE = 0
            F_DISABLE_Z_BUFFERING = 0
            F_BLEND_MODE = 1
            F_TRANSLUCENT = 1
            F_RENDER_BACKFACES = 0

            g_vColorTint = [1.000000, 1.000000, 1.000000, 1.000000]
            g_vGlossinessRange = [0.000000, 1.000000, 0.000000, 0.000000]
            g_vNormalTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
            g_vTexCoordOffset = [0.000000, 0.000000, 0.000000, 0.000000]
            g_vTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
            g_tColor = resource:"materials/dev/primary_white_color_tga_f7b257f6.vtex"
            g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
        })";

static constexpr char szVMatBufferFlatInvisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
        {
            Shader = "csgo_unlitgeneric.vfx"
            F_IGNOREZ = 1
            F_DISABLE_Z_WRITE = 1
            F_DISABLE_Z_BUFFERING = 1
            F_BLEND_MODE = 1
            F_TRANSLUCENT = 1
            g_vColorTint = [1.000000, 1.000000, 1.000000, 0.000000]
            g_vGlossinessRange = [0.000000, 1.000000, 0.000000, 0.000000]
            g_vNormalTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
            g_vTexCoordOffset = [0.000000, 0.000000, 0.000000, 0.000000]
            g_vTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
            g_tColor = resource:"materials/dev/primary_white_color_tga_f7b257f6.vtex"
            g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
        })";

static constexpr char szVMatBufferDefaultVisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
        {
            Shader = "csgo_complex.vfx"
            F_IGNOREZ = 0
            F_DISABLE_Z_WRITE = 0
            F_DISABLE_Z_BUFFERING = 0
            F_RENDER_BACKFACES = 0
            F_TRANSLUCENT = 0
            F_PAINT_VERTEX_COLORS = 1
            g_vColorTint = [1.000000, 1.000000, 1.000000, 1.000000]
            TextureNormal = resource:"materials/default/default_normal.tga"
            g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
            g_tColor = resource:"materials/default/default_color_tga_72dcfbfd.vtex"
            g_tNormal = resource:"materials/default/default_normal_tga_7be61377.vtex"
        })";

static constexpr char szVMatBufferDefaultInvisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
        {
            Shader = "csgo_complex.vfx"
            F_IGNOREZ = 1
            F_DISABLE_Z_WRITE = 1
            F_DISABLE_Z_BUFFERING = 1
            F_RENDER_BACKFACES = 1
            F_TRANSLUCENT = 1
            F_PAINT_VERTEX_COLORS = 1
            g_vColorTint = [1.000000, 1.000000, 1.000000, 1.000000]
            TextureNormal = resource:"materials/default/default_normal.tga"
            g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
            g_tColor = resource:"materials/default/default_color_tga_72dcfbfd.vtex"
            g_tNormal = resource:"materials/default/default_normal_tga_7be61377.vtex"
        })";

static constexpr char szVMatBufferGlowVisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
		     shader = "csgo_effects.vfx"
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_flOpacityScale = 0.70
                g_flFresnelExponent = 1
                g_flFresnelFalloff = 1
                g_flFresnelMax = 0.0
                g_flFresnelMin = 1.0
                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 0
                g_vColorTint = [1.0, 1.0, 1.0, 0.0]
})";

static constexpr char szVMatBufferGlowInvisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
				Shader = "csgo_effects.vfx"
                g_flFresnelExponent = 7.0
                g_flFresnelFalloff = 10.0
                g_flFresnelMax = 0.1
                g_flFresnelMin = 1.0
				g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tSceneDepth = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_flToolsVisCubemapReflectionRoughness = 1.0
                g_flBeginMixingRoughness = 1.0
                g_vColorTint = [ 1.000000, 1.000000, 1.000000, 0 ]
                F_IGNOREZ = 1
                F_DISABLE_Z_WRITE = 1
                F_DISABLE_Z_BUFFERING = 1
                F_RENDER_BACKFACES = 0
})";

static constexpr char szVMatBufferGlassVisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
 {
                shader = "csgo_effects.vfx"
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_flOpacityScale = 0.45
                g_flFresnelExponent = 0.75
                g_flFresnelFalloff = 1
                g_flFresnelMax = 0.0
                g_flFresnelMin = 1
                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 0
                g_vColorTint = [1.0, 1.0, 1.0, 0.0]
            })";

static constexpr char szVMatBufferGlassInvisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
 {
                shader = "csgo_effects.vfx"
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_flOpacityScale = 0.45
                g_flFresnelExponent = 0.75
                g_flFresnelFalloff = 1
                g_flFresnelMax = 0.0
                g_flFresnelMin = 1
                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 1
                F_DISABLE_Z_WRITE = 1
                F_DISABLE_Z_BUFFERING = 1
                F_RENDER_BACKFACES = 0
                g_vColorTint = [1.0, 1.0, 1.0, 0.0]
            })";

static constexpr char szVMatBufferIlluminationVisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
                Shader = "solidcolor.vfx"
                F_BLEND_MODE = 1
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 0
                g_vColorTint = [50.0, 50.0, 50.0, 50.0]
})";

static constexpr char szVMatBufferIlluminationInvisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
				Shader = "solidcolor.vfx"
				g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
				g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
				g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
				g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
				g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
				F_IGNOREZ = 1
				F_DISABLE_Z_WRITE = 1
				F_DISABLE_Z_BUFFERING = 1
				F_RENDER_BACKFACES = 0
                F_BLEND_MODE = 1
				g_vColorTint = [50.0, 50.0, 50.0, 50.0]
})";

static constexpr char szVMatBufferMetalVisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_character.vfx" // Switched to match Character
                F_BLEND_MODE = 1
                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_bFogEnabled = 0
                g_flMetalness = 0.000
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
            })";

static constexpr char szVMatBufferMetalInvisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_character.vfx"
                F_DISABLE_Z_BUFFERING = 1
                F_DISABLE_Z_PREPASS = 1
                F_DISABLE_Z_WRITE = 1
                F_BLEND_MODE = 1
                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_bFogEnabled = 0
                g_flMetalness = 0.000
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
            })";

class CMaterial2;
class CMeshData;

namespace F::VISUALS::CHAMS
{
	bool Initialize();
	void Destroy();

	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);

	// @note: bDisableZBuffering == true to create invisible material
	CStrongHandle<CMaterial2> CreateMaterial(const char* szMaterialName, const char szVmatBuffer[]);

	bool OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2, bool bEnemy, bool bLifeState, bool bLocalPlayer, bool bWeapon);

	inline bool bInitialized = false;
}
