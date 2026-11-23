cmake_minimum_required(VERSION 3.10.2)
project("Metin2Mobile")

option(LUPUS2_BOOTSTRAP "Build the Android/JNI bootstrap before enabling the legacy engine" OFF)

set(LUPUS2_IOS OFF)
if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(LUPUS2_IOS ON)
    enable_language(OBJC OBJCXX)
endif()

option(LUPUS2_BUILD_IOS_APP "Build the native iOS host application" ON)
set(LUPUS2_IOS_DEVELOPMENT_TEAM "" CACHE STRING "Apple Development Team identifier")

set(LUPUS2_MOBILE OFF)
if(ANDROID OR LUPUS2_IOS)
    set(LUPUS2_MOBILE ON)
endif()

if(ANDROID AND LUPUS2_BOOTSTRAP)
    add_library(metin2_mobile SHARED
        userInterface/AndroidBootstrap.cpp
    )

    find_library(log-lib log)
    find_library(gles3-lib GLESv3)
    target_link_libraries(metin2_mobile
        ${log-lib}
        ${gles3-lib}
    )
    return()
endif()

if(ANDROID)
    add_definitions(-D__ANDROID__)
elseif(LUPUS2_IOS)
    add_definitions(-D__IOS__ -DLUPUS2_IOS=1)
endif()
add_definitions(-DENABLE_OPENGL)
add_definitions(-DENABLE_MOBILE_CONTROLS)
add_definitions(-DLOCALE_SERVICE_EUROPE)

if(LUPUS2_MOBILE)
    add_definitions(-DLUPUS2_MOBILE=1)
    # The desktop client is shipped with the compiler's release optimizer.
    # Gradle's installable debug variant otherwise builds this entire legacy
    # engine at -O0, making CPU skinning, Python UI and text rendering miss
    # frames even though the phone GPU is mostly idle.  Keep symbols and the
    # debuggable APK, but compile native code with the PC release-equivalent
    # optimization level.
    # Most of this codebase was written for MSVC, whose aliasing/overflow
    # behaviour is relied upon by several old runtime paths.  Preserve those
    # semantics while enabling the release optimizer on Android.
    add_compile_options(-O2 -fno-strict-aliasing -fwrapv)

    # Some older Granny versions use these for 64-bit
    add_definitions(-DGRANNY_ONLY_TYPES)
    if(ANDROID AND ANDROID_ABI STREQUAL "arm64-v8a")
        add_definitions(-DANDROID_ARM64)
    endif()
    if(LUPUS2_IOS)
        add_definitions(-DIOS_ARM64)
    endif()
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/android_compat
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/include/granny2
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/include/Python2
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_CURRENT_SOURCE_DIR}/eterBase
    ${CMAKE_CURRENT_SOURCE_DIR}/eterLib
    ${CMAKE_CURRENT_SOURCE_DIR}/eterPythonLib
    ${CMAKE_CURRENT_SOURCE_DIR}/gameLib
    ${CMAKE_CURRENT_SOURCE_DIR}/milesLib
    ${CMAKE_CURRENT_SOURCE_DIR}/effectLib
    ${CMAKE_CURRENT_SOURCE_DIR}/sphereLib
    ${CMAKE_CURRENT_SOURCE_DIR}/eterPack
    ${CMAKE_CURRENT_SOURCE_DIR}/eterGrnLib
    ${CMAKE_CURRENT_SOURCE_DIR}/eterImageLib
    ${CMAKE_CURRENT_SOURCE_DIR}/speedTreeLib
    ${CMAKE_CURRENT_SOURCE_DIR}/prTerrainLib
    ${CMAKE_CURRENT_SOURCE_DIR}/scriptLib
    ${CMAKE_CURRENT_SOURCE_DIR}/eterLocale
    ${CMAKE_CURRENT_SOURCE_DIR}/cWebBrowser
)

if(LUPUS2_MOBILE)
    # Keep every mobile dependency relative to the checkout.  The old local
    # Windows build used D:/... paths, which made a clean clone impossible to
    # configure on another machine.
    set(LUPUS2_GRANNY_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/granny_sdk/Granny-3D-SDK-main")
    set(LUPUS2_PYTHON27_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/android-python2.7.3")
    set(LUPUS2_LZO_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lzo-2.10")
    set(LUPUS2_DEVIL_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/DevIL/DevIL")

    set(GRANNY_SDK_ROOT "${LUPUS2_GRANNY_DEFAULT}" CACHE PATH "Granny SDK source root")
    include_directories(BEFORE
        "${GRANNY_SDK_ROOT}/include"
    )
endif()

set(ETER_BASE_SOURCES
	eterBase/Poly/Poly.cpp
	eterBase/Poly/Base.cpp
	eterBase/Poly/Symbol.cpp
	eterBase/Poly/SymTable.cpp
    eterBase/CRC32.cpp
    eterBase/Debug.cpp
    eterBase/FileBase.cpp
    eterBase/FileDir.cpp
    eterBase/FileLoader.cpp
    eterBase/MappedFile.cpp
    eterBase/Random.cpp
    eterBase/StdAfx.cpp
    eterBase/Stl.cpp
    eterBase/TempFile.cpp
    eterBase/Timer.cpp
    eterBase/Utils.cpp
    eterBase/cipher.cpp
    eterBase/error.cpp
    eterBase/lzo.cpp
    eterBase/tea.cpp
)

if(NOT LUPUS2_MOBILE)
    list(APPEND ETER_BASE_SOURCES eterBase/CPostIt.cpp)
endif()

if(LUPUS2_MOBILE)
    set(GRANNY_RUNTIME_SOURCES
        ${GRANNY_SDK_ROOT}/source/granny_animation_binding.cpp
        ${GRANNY_SDK_ROOT}/source/granny_control.cpp
        ${GRANNY_SDK_ROOT}/source/granny_controlled_animation.cpp
        ${GRANNY_SDK_ROOT}/source/granny_controlled_pose.cpp
        ${GRANNY_SDK_ROOT}/source/granny_curve.cpp
        ${GRANNY_SDK_ROOT}/source/granny_data_type_conversion.cpp
        ${GRANNY_SDK_ROOT}/source/granny_data_type_definition.cpp
        ${GRANNY_SDK_ROOT}/source/granny_data_type_io.cpp
        ${GRANNY_SDK_ROOT}/source/granny_file.cpp
        ${GRANNY_SDK_ROOT}/source/granny_file_info.cpp
        ${GRANNY_SDK_ROOT}/source/granny_ik.cpp
        ${GRANNY_SDK_ROOT}/source/granny_local_pose.cpp
        ${GRANNY_SDK_ROOT}/source/granny_log.cpp
        ${GRANNY_SDK_ROOT}/source/granny_lookat.cpp
        ${GRANNY_SDK_ROOT}/source/granny_material.cpp
        ${GRANNY_SDK_ROOT}/source/granny_mesh.cpp
        ${GRANNY_SDK_ROOT}/source/granny_mesh_binding.cpp
        ${GRANNY_SDK_ROOT}/source/granny_mesh_builder.cpp
        ${GRANNY_SDK_ROOT}/source/granny_mesh_deformer.cpp
        ${GRANNY_SDK_ROOT}/source/granny_model_instance.cpp
        ${GRANNY_SDK_ROOT}/source/granny_retargeter.cpp
        ${GRANNY_SDK_ROOT}/source/granny_skeleton.cpp
        ${GRANNY_SDK_ROOT}/source/granny_spu_animation_binding.cpp
        ${GRANNY_SDK_ROOT}/source/granny_spu_controlled_animation.cpp
        ${GRANNY_SDK_ROOT}/source/granny_track_mask.cpp
        ${GRANNY_SDK_ROOT}/source/granny_vertex_data.cpp
        ${GRANNY_SDK_ROOT}/source/granny_world_pose.cpp
        third_party/granny_android_selected.cpp
        ${GRANNY_SDK_ROOT}/source/winxx/win64/ansi_granny_accelerated_deformers.cpp
        ${GRANNY_SDK_ROOT}/source/winxx/win64/ansi_granny_bone_operations.cpp
        ${GRANNY_SDK_ROOT}/source/winxx/win64/ansi_granny_matrix_operations.cpp
    )
    include(${CMAKE_CURRENT_SOURCE_DIR}/third_party/granny_runtime_sources.cmake)

    # The legacy RAD/Oodle1 decoder is used while opening original PC .gr2
    # assets.  It contains old optimizer-sensitive C code and corrupts its
    # arithmetic model when Clang compiles it at -O2.  Keep only that loading
    # path at the known-good optimization level; animation, skinning and the
    # render loop remain optimized.
    set_source_files_properties(
        ${GRANNY_SDK_ROOT}/source/arithbit.c
        ${GRANNY_SDK_ROOT}/source/radarith.c
        ${GRANNY_SDK_ROOT}/source/radlz.c
        ${GRANNY_SDK_ROOT}/source/granny_oodle0_compression.cpp
        ${GRANNY_SDK_ROOT}/source/granny_oodle1_compression.cpp
        PROPERTIES COMPILE_OPTIONS "-O0"
    )

    # Mobile builds use Granny's portable scalar skinning routine where
    # the PC build uses an SSE accelerated deformer.  Let Clang reassociate
    # and vectorize that hot numeric loop for ARM64; vertex formats, matrices
    # and animation timing remain identical to the PC data path.
    set_source_files_properties(
        ${GRANNY_SDK_ROOT}/source/granny_generic_deformers.cpp
        PROPERTIES COMPILE_OPTIONS "-O3;-ffast-math;-funroll-loops"
    )

    add_library(granny_mobile STATIC ${GRANNY_RUNTIME_SOURCES})
    set_target_properties(granny_mobile PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_options(granny_mobile PRIVATE -ffunction-sections -fdata-sections)
    target_compile_definitions(granny_mobile PRIVATE BUILDING_GRANNY_STATIC=1)
    if(LUPUS2_IOS)
        target_compile_definitions(granny_mobile PRIVATE GRANNY_IPHONE=1)
    endif()
    target_include_directories(granny_mobile PRIVATE
        ${GRANNY_SDK_ROOT}/source
        ${GRANNY_SDK_ROOT}/source/winxx/win64
        ${GRANNY_SDK_ROOT}/include
    )

    set(PYTHON27_ROOT
        "${LUPUS2_PYTHON27_DEFAULT}" CACHE PATH "Python 2.7 source root used by the original client")
    file(GLOB PYTHON27_OBJECT_SOURCES ${PYTHON27_ROOT}/Objects/*.c)
    set(PYTHON27_RUNTIME_SOURCES
        ${PYTHON27_ROOT}/Parser/acceler.c
        ${PYTHON27_ROOT}/Parser/bitset.c
        ${PYTHON27_ROOT}/Parser/firstsets.c
        ${PYTHON27_ROOT}/Parser/grammar.c
        ${PYTHON27_ROOT}/Parser/grammar1.c
        ${PYTHON27_ROOT}/Parser/listnode.c
        ${PYTHON27_ROOT}/Parser/metagrammar.c
        ${PYTHON27_ROOT}/Parser/myreadline.c
        ${PYTHON27_ROOT}/Parser/node.c
        ${PYTHON27_ROOT}/Parser/parser.c
        ${PYTHON27_ROOT}/Parser/parsetok.c
        ${PYTHON27_ROOT}/Parser/pgen.c
        ${PYTHON27_ROOT}/Parser/tokenizer.c
        ${PYTHON27_ROOT}/Python/_warnings.c
        ${PYTHON27_ROOT}/Python/Python-ast.c
        ${PYTHON27_ROOT}/Python/asdl.c
        ${PYTHON27_ROOT}/Python/ast.c
        ${PYTHON27_ROOT}/Python/bltinmodule.c
        ${PYTHON27_ROOT}/Python/ceval.c
        ${PYTHON27_ROOT}/Python/codecs.c
        ${PYTHON27_ROOT}/Python/compile.c
        ${PYTHON27_ROOT}/Python/dtoa.c
        ${PYTHON27_ROOT}/Python/dynload_shlib.c
        ${PYTHON27_ROOT}/Python/errors.c
        ${PYTHON27_ROOT}/Python/formatter_string.c
        ${PYTHON27_ROOT}/Python/formatter_unicode.c
        ${PYTHON27_ROOT}/Python/frozen.c
        ${PYTHON27_ROOT}/Python/future.c
        ${PYTHON27_ROOT}/Python/getargs.c
        ${PYTHON27_ROOT}/Python/getcompiler.c
        ${PYTHON27_ROOT}/Python/getcopyright.c
        ${PYTHON27_ROOT}/Python/getopt.c
        ${PYTHON27_ROOT}/Python/getplatform.c
        ${PYTHON27_ROOT}/Python/getversion.c
        ${PYTHON27_ROOT}/Python/graminit.c
        ${PYTHON27_ROOT}/Python/import.c
        ${PYTHON27_ROOT}/Python/importdl.c
        ${PYTHON27_ROOT}/Python/marshal.c
        ${PYTHON27_ROOT}/Python/modsupport.c
        ${PYTHON27_ROOT}/Python/mysnprintf.c
        ${PYTHON27_ROOT}/Python/mystrtoul.c
        ${PYTHON27_ROOT}/Python/peephole.c
        ${PYTHON27_ROOT}/Python/pyarena.c
        ${PYTHON27_ROOT}/Python/pyctype.c
        ${PYTHON27_ROOT}/Python/pyfpe.c
        ${PYTHON27_ROOT}/Python/pymath.c
        ${PYTHON27_ROOT}/Python/pystate.c
        ${PYTHON27_ROOT}/Python/pystrcmp.c
        ${PYTHON27_ROOT}/Python/pystrtod.c
        ${PYTHON27_ROOT}/Python/pythonrun.c
        ${PYTHON27_ROOT}/Python/random.c
        ${PYTHON27_ROOT}/Python/structmember.c
        ${PYTHON27_ROOT}/Python/symtable.c
        ${PYTHON27_ROOT}/Python/sysmodule.c
        ${PYTHON27_ROOT}/Python/thread.c
        ${PYTHON27_ROOT}/Python/traceback.c
        ${PYTHON27_ROOT}/Modules/gcmodule.c
        ${PYTHON27_ROOT}/Modules/getbuildinfo.c
        ${PYTHON27_ROOT}/Modules/getpath.c
        ${PYTHON27_ROOT}/Modules/signalmodule.c
        ${PYTHON27_ROOT}/Modules/_weakref.c
        ${PYTHON27_ROOT}/Modules/mathmodule.c
        ${PYTHON27_ROOT}/Modules/_sre.c
        ${PYTHON27_ROOT}/Modules/_codecsmodule.c
        ${PYTHON27_ROOT}/Modules/_struct.c
		${PYTHON27_ROOT}/Modules/cStringIO.c
		${PYTHON27_ROOT}/Modules/binascii.c
		${PYTHON27_ROOT}/Modules/md5module.c
		${PYTHON27_ROOT}/Modules/md5.c
		${PYTHON27_ROOT}/Modules/shamodule.c
		${PYTHON27_ROOT}/Modules/sha256module.c
		${PYTHON27_ROOT}/Modules/sha512module.c
        ${PYTHON27_ROOT}/Modules/operator.c
        ${PYTHON27_ROOT}/Modules/_collectionsmodule.c
        ${PYTHON27_ROOT}/Modules/itertoolsmodule.c
		${PYTHON27_ROOT}/Modules/_functoolsmodule.c
		${PYTHON27_ROOT}/Modules/_localemodule.c
        ${PYTHON27_ROOT}/Modules/errnomodule.c
        ${PYTHON27_ROOT}/Modules/posixmodule.c
        ${PYTHON27_ROOT}/Modules/timemodule.c
        ${PYTHON27_ROOT}/Modules/datetimemodule.c
        ${PYTHON27_ROOT}/Modules/_randommodule.c
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/python_android_config.c
        ${PYTHON27_OBJECT_SOURCES}
    )
    add_library(python27_mobile STATIC ${PYTHON27_RUNTIME_SOURCES})
    set_target_properties(python27_mobile PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(python27_mobile PRIVATE Py_BUILD_CORE=1 _GNU_SOURCE=1)
    target_compile_options(python27_mobile PRIVATE -Wno-deprecated-non-prototype)
    target_include_directories(python27_mobile BEFORE PUBLIC
        ${PYTHON27_ROOT}
        ${PYTHON27_ROOT}/Include
        ${PYTHON27_ROOT}/Parser
    )

    set(CRYPTOPP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/include/cryptopp)
    file(GLOB CRYPTOPP_RUNTIME_SOURCES ${CRYPTOPP_ROOT}/*.cpp)
    list(FILTER CRYPTOPP_RUNTIME_SOURCES EXCLUDE REGEX
        "/(adhoc|test|bench[123]|datatest|dlltest|fipsalgt|validat[0-9]+|regtest[1-4]|pch|simple|winpipes|cryptlib_bds)\\.cpp$")
    if(ANDROID)
        list(APPEND CRYPTOPP_RUNTIME_SOURCES
            ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
    endif()
    add_library(cryptopp_mobile STATIC ${CRYPTOPP_RUNTIME_SOURCES})
    set_target_properties(cryptopp_mobile PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(cryptopp_mobile PRIVATE CRYPTOPP_DISABLE_ASM=1)
    target_compile_options(cryptopp_mobile PRIVATE -Wno-deprecated-declarations)
    target_include_directories(cryptopp_mobile PUBLIC ${CRYPTOPP_ROOT})
    if(ANDROID)
        target_include_directories(cryptopp_mobile PUBLIC
            ${ANDROID_NDK}/sources/android/cpufeatures)
    endif()

    set(LZO_ROOT "${LUPUS2_LZO_DEFAULT}" CACHE PATH "LZO source root")
    file(GLOB LZO_RUNTIME_SOURCES ${LZO_ROOT}/src/*.c)
    add_library(lzo2_mobile STATIC ${LZO_RUNTIME_SOURCES})
    set_target_properties(lzo2_mobile PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_include_directories(lzo2_mobile PUBLIC ${LZO_ROOT}/include)

    set(DEVIL_ROOT "${LUPUS2_DEVIL_DEFAULT}" CACHE PATH "DevIL source root")
    file(GLOB DEVIL_RUNTIME_SOURCES ${DEVIL_ROOT}/src-IL/src/*.cpp)
    list(FILTER DEVIL_RUNTIME_SOURCES EXCLUDE REGEX "/altivec_.*\\.cpp$")
    add_library(devil_mobile STATIC ${DEVIL_RUNTIME_SOURCES})
    set_target_properties(devil_mobile PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(devil_mobile PRIVATE IL_STATIC_LIB=1 IL_NO_EXTLIBS=1)
    target_include_directories(devil_mobile BEFORE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/devil_android
        ${DEVIL_ROOT}/include
        ${DEVIL_ROOT}/src-IL/include
    )
endif()

set(ETER_LIB_SOURCES
	eterLib/TextBar.cpp
    eterLib/BlockTexture.cpp
    eterLib/ColorTransitionHelper.cpp
    eterLib/Decal.cpp
    eterLib/DibBar.cpp
    eterLib/EnvironmentMap.cpp
    eterLib/AndroidFileLoaderThread.cpp
    eterLib/GrpCollisionObject.cpp
    eterLib/GrpD3DXBuffer.cpp
    eterLib/GrpDetector.cpp
    eterLib/GrpDIB.cpp
    eterLib/GrpExpandedImageInstance.cpp
    eterLib/GrpFontTexture.cpp
    eterLib/GrpMarkInstance.cpp
    eterLib/GrpPixelShader.cpp
    eterLib/GrpRatioInstance.cpp
    eterLib/GrpShadowTexture.cpp
    eterLib/GrpSubImage.cpp
    eterLib/GrpVertexBufferDynamic.cpp
    eterLib/GrpVertexBufferStatic.cpp
    eterLib/GrpVertexShader.cpp
    eterLib/AndroidIME.cpp
    eterLib/LensFlare.cpp
    eterLib/lineintersect_utils.cpp
    eterLib/Mutex.cpp
    eterLib/NetDatagram.cpp
    eterLib/NetPacketHeaderMap.cpp
    eterLib/parser.cpp
    eterLib/PathStack.cpp
    eterLib/ScreenFilter.cpp
    eterLib/SkyBox.cpp
    eterLib/TextTag.cpp
    eterLib/Thread.cpp
    eterLib/Camera.cpp
    eterLib/CullingManager.cpp
    eterLib/GrpDevice.cpp
    eterLib/GrpObjectInstance.cpp
    eterLib/GrpScreen.cpp
    eterLib/GrpTexture.cpp
    eterLib/GrpImageTexture.cpp
    eterLib/GrpBase.cpp
    eterLib/GrpColor.cpp
    eterLib/GrpColorInstance.cpp
    eterLib/GrpImage.cpp
    eterLib/GrpImageInstance.cpp
    eterLib/GrpIndexBuffer.cpp
    eterLib/GrpLightManager.cpp
    eterLib/GrpMath.cpp
    eterLib/GrpVertexBuffer.cpp
    eterLib/GrpText.cpp
    eterLib/GrpTextInstance.cpp
    eterLib/MSApplication.cpp
    eterLib/MSWindow.cpp
    eterLib/Input.cpp
    eterLib/NetDevice.cpp
    eterLib/ResourceManager.cpp
    eterLib/ReferenceObject.cpp
    eterLib/NetAddress.cpp
    eterLib/NetDatagramReceiver.cpp
    eterLib/NetDatagramSender.cpp
    eterLib/NetStream.cpp
    eterLib/StdAfx.cpp
    eterLib/StateManager.cpp
    eterLib/AttributeData.cpp
    eterLib/AttributeInstance.cpp
    eterLib/CollisionData.cpp
    eterLib/Resource.cpp
    eterLib/TargaResource.cpp
    eterLib/TextFileLoader.cpp
    eterLib/Util.cpp
)

set(ETER_IMAGE_LIB_SOURCES
    eterImageLib/DXTCImage.cpp
    eterImageLib/Image.cpp
    eterImageLib/StdAfx.cpp
    eterImageLib/TGAImage.cpp
    eterImageLib/stb_image.cpp
)

set(GAME_LIB_SOURCES
    gameLib/AreaTerrain.cpp
    gameLib/DungeonBlock.cpp
    gameLib/FlyingData.cpp
    gameLib/FlyTarget.cpp
    gameLib/FlyTrace.cpp
    gameLib/GameType.cpp
    gameLib/GameUtil.cpp
    gameLib/MapBase.cpp
    gameLib/MapOutdoorCharacterShadow.cpp
    gameLib/MapOutdoorIndexBuffer.cpp
    gameLib/MapOutdoorLoad.cpp
    gameLib/MapOutdoorQuadtree.cpp
    gameLib/MapOutdoorRender.cpp
    gameLib/MapOutdoorRenderHTP.cpp
    gameLib/MapOutdoorRenderSTP.cpp
    gameLib/MapOutdoorUpdate.cpp
    gameLib/MapOutdoorWater.cpp
    gameLib/MapType.cpp
    gameLib/MapUtil.cpp
    gameLib/MonsterAreaInfo.cpp
    gameLib/PhysicsObject.cpp
    gameLib/Property.cpp
    gameLib/PropertyLoader.cpp
    gameLib/RaceDataFile.cpp
    gameLib/RaceMotionData.cpp
    gameLib/SnowEnvironment.cpp
    gameLib/SnowParticle.cpp
    gameLib/TerrainDecal.cpp
    gameLib/TerrainPatch.cpp
    gameLib/TerrainQuadtree.cpp
    gameLib/WeaponTrace.cpp
    gameLib/Area.cpp
    gameLib/AreaLoaderThread.cpp
    gameLib/ActorInstance.cpp
    gameLib/ActorInstanceAttach.cpp
    gameLib/ActorInstanceBattle.cpp
    gameLib/ActorInstanceBlend.cpp
    gameLib/ActorInstanceCollisionDetection.cpp
    gameLib/ActorInstanceData.cpp
    gameLib/ActorInstanceEvent.cpp
    gameLib/ActorInstanceFly.cpp
    gameLib/ActorInstanceMotion.cpp
    gameLib/ActorInstanceMotionEvent.cpp
    gameLib/ActorInstancePosition.cpp
    gameLib/ActorInstanceRender.cpp
    gameLib/ActorInstanceRotation.cpp
    gameLib/ActorInstanceSync.cpp
    gameLib/ActorInstanceWeaponTrace.cpp
    gameLib/FlyingInstance.cpp
    gameLib/FlyingObjectManager.cpp
    gameLib/GameEventManager.cpp
    gameLib/ItemData.cpp
    gameLib/ItemManager.cpp
    gameLib/MapManager.cpp
    gameLib/MapOutdoor.cpp
    gameLib/PropertyManager.cpp
    gameLib/RaceData.cpp
    gameLib/RaceManager.cpp
    gameLib/StdAfx.cpp
)

set(USER_INTERFACE_SOURCES
	userInterface/WiseLogicXTrap.cpp
	userInterface/AndroidWebBrowser.cpp
    userInterface/AffectFlagContainer.cpp
    userInterface/CameraProcedure.cpp
    userInterface/InstanceBase.cpp
    userInterface/InstanceBaseBattle.cpp
    userInterface/InstanceBaseEffect.cpp
    userInterface/InstanceBaseEvent.cpp
    userInterface/InstanceBaseMotion.cpp
    userInterface/InstanceBaseMovement.cpp
    userInterface/InstanceBaseTransform.cpp
    userInterface/InsultChecker.cpp
    userInterface/AndroidMovieMan.cpp
    userInterface/NetworkActorManager.cpp
    userInterface/ProcessCRC.cpp
    userInterface/PythonApplicationCursor.cpp
    userInterface/PythonApplicationLogo.cpp
    userInterface/AndroidPythonApplicationProcedure.cpp
    userInterface/PythonBackgroundModule.cpp
    userInterface/PythonCharacterManagerModule.cpp
    userInterface/PythonCharacterModule.cpp
    userInterface/PythonChatModule.cpp
    userInterface/PythonCaptchaStub.cpp
    userInterface/PythonEffectModule.cpp
    userInterface/PythonEventManagerMoudle.cpp
    userInterface/PythonExceptionSender.cpp
    userInterface/PythonExchangeModule.cpp
    userInterface/PythonFlyModule.cpp
    userInterface/PythonGameEventManagerModule.cpp
    userInterface/PythonIMEModule.cpp
    userInterface/PythonItemModule.cpp
    userInterface/PythonMiniMapModule.cpp
    userInterface/PythonNetworkDatagram.cpp
    userInterface/PythonNetworkDatagramModule.cpp
    userInterface/PythonNetworkStreamCommand.cpp
    userInterface/PythonNetworkStreamEvent.cpp
    userInterface/PythonNetworkStreamPhaseGameActor.cpp
    userInterface/PythonNetworkStreamPhaseGameItem.cpp
    userInterface/PythonNetworkStreamPhaseHandShake.cpp
    userInterface/PythonNetworkStreamPhaseOffline.cpp
    userInterface/PythonNonPlayerModule.cpp
    userInterface/PythonPlayerEventHandler.cpp
    userInterface/PythonPlayerInput.cpp
    userInterface/PythonPlayerInputKeyboard.cpp
    userInterface/PythonPlayerInputMouse.cpp
    userInterface/PythonPlayerSkill.cpp
    userInterface/PythonProfilerModule.cpp
    userInterface/PythonSoundManagerModule.cpp
    userInterface/PythonSystemModule.cpp
    userInterface/PythonTextTailModule.cpp
    userInterface/ServerStateCheckerModule.cpp
    userInterface/AccountConnector.cpp
    userInterface/AndroidMain.cpp
    userInterface/NyxSentinelAndroid.cpp
    userInterface/GuildMarkDownloader.cpp
    userInterface/GuildMarkUploader.cpp
    userInterface/GameType.cpp
    userInterface/Locale.cpp
    userInterface/MarkImage.cpp
    userInterface/MarkManager.cpp
    userInterface/PythonApplication.cpp
    userInterface/PythonApplicationCamera.cpp
    userInterface/PythonApplicationEvent.cpp
    userInterface/PythonApplicationModule.cpp
    userInterface/PythonAcce.cpp
    userInterface/PythonBackground.cpp
    userInterface/PythonCharacterManager.cpp
    userInterface/PythonChat.cpp
    userInterface/PythonEventManager.cpp
    userInterface/PythonExchange.cpp
    userInterface/PythonGuild.cpp
    userInterface/PythonIME.cpp
    userInterface/PythonItem.cpp
    userInterface/PythonMessenger.cpp
    userInterface/PythonMiniMap.cpp
    userInterface/PythonNonPlayer.cpp
    userInterface/PythonQuest.cpp
    userInterface/PythonSafeBox.cpp
    userInterface/PythonShop.cpp
    userInterface/PythonIkarusShopStub.cpp
    userInterface/PythonSkill.cpp
    userInterface/PythonSystem.cpp
    userInterface/PythonTextTail.cpp
    userInterface/PythonPlayerModule.cpp
    userInterface/PythonPlayer.cpp
    userInterface/PythonPlayerSettingsModule.cpp
    userInterface/PythonPackModule.cpp
    userInterface/PythonNetworkStream.cpp
    userInterface/PythonNetworkStreamModule.cpp
    userInterface/PythonNetworkStreamPhaseGame.cpp
    userInterface/PythonNetworkStreamPhaseLoading.cpp
    userInterface/PythonNetworkStreamPhaseLogin.cpp
    userInterface/PythonNetworkStreamPhaseSelect.cpp
    userInterface/ServerStateChecker.cpp
    userInterface/UserInterface.cpp
    userInterface/StdAfx.cpp
)

set(ETER_PACK_SOURCES
    eterPack/EterPack.cpp
    eterPack/EterPackCursor.cpp
    eterPack/EterPackManager.cpp
    eterPack/EterPackPolicy_CSHybridCrypt.cpp
    eterPack/md5.c
    eterPack/StdAfx.cpp
)

set(ETER_GRN_LIB_SOURCES
    eterGrnLib/LODController.cpp
    eterGrnLib/ModelInstanceMotion.cpp
    eterGrnLib/Motion.cpp
    eterGrnLib/Util.cpp
    eterGrnLib/Material.cpp
    eterGrnLib/Mesh.cpp
    eterGrnLib/Model.cpp
    eterGrnLib/ModelInstance.cpp
    eterGrnLib/ModelInstanceCollisionDetection.cpp
    eterGrnLib/ModelInstanceModel.cpp
    eterGrnLib/ModelInstanceRender.cpp
    eterGrnLib/ModelInstanceUpdate.cpp
    eterGrnLib/StdAfx.cpp
    eterGrnLib/Thing.cpp
    eterGrnLib/ThingInstance.cpp
)

set(EFFECT_LIB_SOURCES
    effectLib/EffectData.cpp
    effectLib/EffectElementBase.cpp
    effectLib/EffectElementBaseInstance.cpp
    effectLib/EffectInstance.cpp
    effectLib/EffectManager.cpp
    effectLib/EffectMesh.cpp
    effectLib/EffectMeshInstance.cpp
    effectLib/EffectUpdateDecorator.cpp
    effectLib/EmitterProperty.cpp
    effectLib/FrameController.cpp
    effectLib/ParticleInstance.cpp
    effectLib/ParticleProperty.cpp
    effectLib/ParticleSystemData.cpp
    effectLib/ParticleSystemInstance.cpp
    effectLib/SimpleLightData.cpp
    effectLib/SimpleLightInstance.cpp
    effectLib/StdAfx.cpp
    effectLib/Type.cpp
)

set(ETER_PYTHON_LIB_SOURCES
    eterPythonLib/PythonGraphic.cpp
    eterPythonLib/PythonGraphicImageModule.cpp
    eterPythonLib/PythonGraphicModule.cpp
    eterPythonLib/PythonGraphicTextModule.cpp
    eterPythonLib/PythonGraphicThingModule.cpp
    eterPythonLib/PythonGridSlotWindow.cpp
    eterPythonLib/PythonSlotWindow.cpp
    eterPythonLib/PythonWindow.cpp
    eterPythonLib/PythonWindowManager.cpp
    eterPythonLib/PythonWindowManagerModule.cpp
    eterPythonLib/StdAfx.cpp
)

set(SCRIPT_LIB_SOURCES
    scriptLib/PythonDebugModule.cpp
    scriptLib/PythonLauncher.cpp
    scriptLib/PythonMarshal.cpp
    scriptLib/PythonUtils.cpp
    scriptLib/Resource.cpp
    scriptLib/StdAfx.cpp
)

set(SPHERE_LIB_SOURCES
    sphereLib/frustum.cpp
    sphereLib/sphere.cpp
    sphereLib/spherepack.cpp
    sphereLib/StdAfx.cpp
)

set(MILES_LIB_SOURCES
    milesLib/SoundBase.cpp
    milesLib/SoundData.cpp
    milesLib/SoundInstance2D.cpp
    milesLib/SoundInstance3D.cpp
    milesLib/SoundInstanceStream.cpp
    milesLib/SoundManager.cpp
    milesLib/SoundManager2D.cpp
    milesLib/SoundManager3D.cpp
    milesLib/SoundManagerStream.cpp
    milesLib/Stdafx.cpp
    milesLib/Type.cpp
)

set(SPEED_TREE_LIB_SOURCES
    speedTreeLib/BoundaryShapeManager.cpp
    speedTreeLib/SpeedGrassRT.cpp
    speedTreeLib/SpeedGrassWrapper.cpp
    speedTreeLib/SpeedTreeForest.cpp
    speedTreeLib/SpeedTreeForestDirectX8.cpp
    speedTreeLib/SpeedTreeWrapper.cpp
    speedTreeLib/StdAfx.cpp
)

set(PR_TERRAIN_LIB_SOURCES
    prTerrainLib/Terrain.cpp
    prTerrainLib/TextureSet.cpp
    prTerrainLib/StdAfx.cpp
)

set(ETER_LOCALE_SOURCES
    eterLocale/Arabic.cpp
    eterLocale/Japanese.cpp
    eterLocale/StringCodec.cpp
    eterLocale/StringCodec_Vietnamese.cpp
    eterLocale/StdAfx.cpp
)

if(NOT LUPUS2_MOBILE)
    set(C_WEB_BROWSER_SOURCES
        cWebBrowser/CWebBrowser.c
    )
    add_definitions(-DENABLE_WEB_BROWSER) # Optional flag if the code needs it
else()
    set(C_WEB_BROWSER_SOURCES "")
endif()

if(LUPUS2_IOS)
    set(LUPUS2_ENGINE_LIBRARY_TYPE STATIC)
else()
    set(LUPUS2_ENGINE_LIBRARY_TYPE SHARED)
endif()

add_library(metin2_mobile ${LUPUS2_ENGINE_LIBRARY_TYPE}
    ${USER_INTERFACE_SOURCES}
    ${ETER_BASE_SOURCES}
    ${ETER_LIB_SOURCES}
    ${ETER_IMAGE_LIB_SOURCES}
    ${GAME_LIB_SOURCES}
    ${ETER_PACK_SOURCES}
    ${ETER_GRN_LIB_SOURCES}
    ${EFFECT_LIB_SOURCES}
    ${ETER_PYTHON_LIB_SOURCES}
    ${SCRIPT_LIB_SOURCES}
    ${SPHERE_LIB_SOURCES}
    ${MILES_LIB_SOURCES}
    ${SPEED_TREE_LIB_SOURCES}
    ${PR_TERRAIN_LIB_SOURCES}
    ${ETER_LOCALE_SOURCES}
    ${C_WEB_BROWSER_SOURCES}
)

if(ANDROID)
    # The legacy MSVC code frequently takes the address of a temporary D3DX
    # vector. Clang supports it as an extension; retain that source behavior
    # while the renderer is being ported.
    target_compile_options(metin2_mobile PRIVATE
        -Wno-address-of-temporary
        -fno-autolink
    )
    target_link_options(metin2_mobile PRIVATE
        -Wl,--error-limit=0
        -Wl,-z,max-page-size=16384
        -Wl,-z,common-page-size=16384
    )
endif()

# Extern Library linking
set(EXTERN_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/lib/android/${ANDROID_ABI})
link_directories(${EXTERN_LIB_PATH})

if(ANDROID)
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(gles3-lib GLESv3)
elseif(LUPUS2_IOS)
    find_library(opengles-lib OpenGLES REQUIRED)
    find_library(foundation-lib Foundation REQUIRED)
endif()

# Common Metin2 Libraries (Static)
if(LUPUS2_MOBILE)
    # Android dependencies are added back as native targets as they are
    # ported. Windows import libraries must never be searched by the ARM64
    # linker. Miles, DevIL and JPEG are currently isolated behind Android
    # compatibility paths so they intentionally have no link dependency.
    set(METIN2_EXTERN_LIBS)
else()
    set(METIN2_EXTERN_LIBS
        python2.7 cryptopp lzo2 il ilu ilut jpeg png speedtree granny2 mss
    )
endif()

target_link_libraries(metin2_mobile
    ${log-lib}
    ${android-lib}
    ${gles3-lib}
    ${opengles-lib}
    ${foundation-lib}
    ${METIN2_EXTERN_LIBS}
    $<$<BOOL:${LUPUS2_MOBILE}>:granny_mobile>
    $<$<BOOL:${LUPUS2_MOBILE}>:python27_mobile>
    $<$<BOOL:${LUPUS2_MOBILE}>:cryptopp_mobile>
    $<$<BOOL:${LUPUS2_MOBILE}>:lzo2_mobile>
    $<$<BOOL:${LUPUS2_MOBILE}>:devil_mobile>
    z
    m
)

if(ANDROID)
    target_link_options(metin2_mobile PRIVATE -Wl,--gc-sections)
endif()

if(LUPUS2_IOS AND LUPUS2_BUILD_IOS_APP)
    set(LUPUS2_IOS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../ios")
    set(LUPUS2_IOS_ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/../android/app/src/lite/assets")

    add_executable(Lupus2IOS MACOSX_BUNDLE
        ${LUPUS2_IOS_ROOT}/Lupus2/main.m
        ${LUPUS2_IOS_ROOT}/Lupus2/Lupus2AppDelegate.mm
        ${LUPUS2_IOS_ROOT}/Lupus2/Lupus2GameViewController.mm
        ${LUPUS2_IOS_ASSETS}
    )
    set_source_files_properties(${LUPUS2_IOS_ASSETS} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
    target_include_directories(Lupus2IOS PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mobile
        ${LUPUS2_IOS_ROOT}/Lupus2)
    target_link_libraries(Lupus2IOS PRIVATE
        metin2_mobile
        ${opengles-lib}
        ${foundation-lib}
        "-framework GLKit"
        "-framework UIKit")
    set_target_properties(Lupus2IOS PROPERTIES
        OUTPUT_NAME Lupus2
        MACOSX_BUNDLE_INFO_PLIST ${LUPUS2_IOS_ROOT}/Lupus2/Info.plist
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER online.lupus2.client
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 13.0
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_SUPPORTS_MACCATALYST NO
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE Automatic
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${LUPUS2_IOS_DEVELOPMENT_TEAM}"
    )
endif()
