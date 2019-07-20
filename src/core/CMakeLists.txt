# src/core/CmakeLists.txt - Core DOS CMakeLists 

file(GLOB CORE_SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${CORE_SOURCES})

file(GLOB BOUNDING_SOURCES 
  "bounding/*.cpp"
  "bounding/*.h"
)
source_group("bounding" FILES ${BOUNDING_SOURCES})

file(GLOB CAMERA_SOURCES 
  "camera/*.cpp"
  "camera/*.h"
)
source_group("camera" FILES ${CAMERA_SOURCES})

file(GLOB DATASTRUCTURES_SOURCES 
  "datastructures/*.cpp"
  "datastructures/*.h"
)
source_group("datastructures" FILES ${DATASTRUCTURES_SOURCES})

file(GLOB DIMENSION_SOURCES 
  "dimension/*.cpp"
  "dimension/*.h"
)
source_group("dimension" FILES ${DIMENSION_SOURCES})

file(GLOB EHM_SOURCES 
  "ehm/*.cpp"
  "ehm/*.h"
)
source_group("ehm" FILES ${EHM_SOURCES})

file(GLOB HAND_SOURCES 
  "hand/*.cpp"
  "hand/*.h"
)
source_group("hand" FILES ${HAND_SOURCES})

file(GLOB HYSTERESIS_SOURCES 
  "hysteresis/*.cpp"
  "hysteresis/*.h"
)
source_group("hysteresis" FILES ${HYSTERESIS_SOURCES})

file(GLOB IMAGE_SOURCES 
  "image/*.cpp"
  "image/*.h"
)
source_group("image" FILES ${IMAGE_SOURCES})

file(GLOB MATERIAL_SOURCES 
  "material/*.cpp"
  "material/*.h"
)
source_group("material" FILES ${MATERIAL_SOURCES})

file(GLOB MATRIX_SOURCES 
  "matrix/*.cpp"
  "matrix/*.h"
)
source_group("matrix" FILES ${MATRIX_SOURCES})

file(GLOB MODEL_SOURCES 
  "model/*.cpp"
  "model/*.h"
)
source_group("model" FILES ${MODEL_SOURCES})

file(GLOB PRIMITIVES_SOURCES 
  "primitives/*.cpp"
  "primitives/*.h"
)
source_group("primitives" FILES ${PRIMITIVES_SOURCES})

file(GLOB TEXT_SOURCES 
  "text/*.cpp"
  "text/*.h"
)
source_group("text" FILES ${TEXT_SOURCES})

file(GLOB TYPES_SOURCES 
  "types/*.cpp"
  "types/*.h"
)
source_group("types" FILES ${TYPES_SOURCES})

file(GLOB USER_SOURCES 
  "user/*.cpp"
  "user/*.h"
)
source_group("user" FILES ${USER_SOURCES})

add_library(core 
    ${CORE_SOURCES} 
    ${BOUNDING_SOURCES}
    ${CAMERA_SOURCES}
    ${DATASTRUCTURES_SOURCES}
    ${DIMENSION_SOURCES}
    ${EHM_SOURCES}
    ${HAND_SOURCES}
    ${HYSTERESIS_SOURCES}
    ${IMAGE_SOURCES}
    ${MATERIAL_SOURCES}
    ${MATRIX_SOURCES}
    ${MODEL_SOURCES}
    ${PRIMITIVES_SOURCES}
    ${TEXT_SOURCES}
    ${TYPES_SOURCES}
    ${USER_SOURCES}
)


