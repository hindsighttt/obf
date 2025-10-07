
#include "../../math/math.hpp"
#include "../../schema/schema.hpp"

#include "../types/cnetworkutlvectorbase.hpp"
#include "../types/chandle.hpp"
class C_PostProcessingVolume {
   public:
    SCHEMA_FIELD(m_bExposureControl, "C_PostProcessingVolume", "m_bExposureControl",bool);
    SCHEMA_FIELD(m_flMinExposure, "C_PostProcessingVolume", "m_flMinExposure",float);
    SCHEMA_FIELD(m_flMaxExposure, "C_PostProcessingVolume", "m_flMaxExposure",
                 float);
};
class CPlayer_CameraServices {
   public:
    SCHEMA_FIELD(m_hActivePostProcessingVolumes, "CPlayer_CameraServices",
                 "m_hActivePostProcessingVolumes", CHandle);
};
