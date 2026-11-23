#define BUILDING_GRANNY_STATIC 1

#include "granny.h"

#include "granny_types.h"


#include "granny_cpp_settings.h"

#include "granny_version.h"

#include "granny_transform.h"

#include "granny_data_type_definition.h"

#include "granny_model.h"

#include "granny_model_control_binding.h"

#include "granny_model_instance.h"

#include "granny_curve.h"

#include "granny_material.h"

#include "granny_pixel_layout.h"

#include "granny_animation.h"

#include "granny_animation_binding.h"

#include "granny_art_tool_info.h"

#include "granny_back_compat.h"

#include "granny_basis_conversion.h"

#include "granny_bink.h"

#include "granny_bspline.h"

#include "granny_bspline_buffers.h"

#include "granny_bspline_solver.h"

#include "granny_camera.h"

#include "granny_compress_curve.h"

#include "granny_control.h"

#include "granny_controlled_animation.h"

#include "granny_controlled_pose.h"

#include "granny_crc.h"

#include "granny_data_type_conversion.h"

#include "granny_data_type_io.h"

#include "granny_exporter_info.h"

#include "granny_file.h"

#include "granny_file_builder.h"

#include "granny_file_compressor.h"

#include "granny_file_format.h"

#include "granny_file_info.h"

#include "granny_file_operations.h"

#include "granny_file_reader.h"

#include "granny_file_writer.h"

#include "granny_find_knot.h"

#include "granny_float16.h"

#include "granny_ik.h"

#include "granny_image_operations.h"

#include "granny_intersection.h"

#include "granny_local_pose.h"

#include "granny_log.h"

#include "granny_lookat.h"

#include "granny_math.h"

#include "granny_matrix_operations.h"

#include "granny_memory.h"

#include "granny_memory_arena.h"

#include "granny_memory_file_reader.h"

#include "granny_memory_file_writer.h"

#include "granny_mesh.h"

#include "granny_mesh_binding.h"

#include "granny_mesh_builder.h"

#include "granny_mesh_deformer.h"

#include "granny_mirror_specification.h"

#include "granny_mod_support.h"

#include "granny_oodle1_compression.h"

#include "granny_periodic_loop.h"

#include "granny_pointer_hash.h"

#include "granny_pose_cache.h"

#include "granny_retargeter.h"

#include "granny_s3tc.h"

#include "granny_skeleton.h"

#include "granny_skeleton_builder.h"

#include "granny_spu_animation.h"

#include "granny_spu_animation_binding.h"

#include "granny_spu_animation_info.h"

#include "granny_spu_commands_ppuside.h"

#include "granny_spu_controlled_animation.h"

#include "granny_spu_track_group.h"

#include "granny_stack_allocator.h"

#include "granny_string.h"

#include "granny_string_comparison_callback.h"

#include "granny_string_database.h"

#include "granny_string_table.h"

#include "granny_system_clock.h"

#include "granny_tangent_frame.h"

#include "granny_telemetry.h"

#include "granny_texture.h"

#include "granny_texture_builder.h"

#include "granny_thread_safety.h"

#include "granny_track_group.h"

#include "granny_track_group_builder.h"

#include "granny_track_group_sampler.h"

#include "granny_track_mask.h"

#include "granny_track_sampler.h"

#include "granny_tri_topology.h"

#include "granny_type_listing.h"

#include "granny_variant_builder.h"

#include "granny_version_checking.h"

#include "granny_vertex_data.h"

#include "granny_world_pose.h"

BEGIN_GRANNY_NAMESPACE; END_GRANNY_NAMESPACE; USING_GRANNY_NAMESPACE;

GRANNY_DYNLINK(void) GrannyCompleteControlAt(granny_control * Control,
                                             granny_real32 AtSeconds)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    CompleteControlAt((control &)*Control,
                      (real32)AtSeconds);
}

GRANNY_DYNLINK(bool) GrannyControlIsComplete(granny_control const * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((bool)ControlIsComplete((control const &)*Control));
}

GRANNY_DYNLINK(void) GrannyConvertSingleObject(granny_data_type_definition const * SourceType,
                                               void const * SourceObject,
                                               granny_data_type_definition const * DestType,
                                               void * DestObject,
                                               granny_conversion_handler *  OverrideHandler)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    ConvertSingleObject((data_type_definition const *)SourceType,
                        (void const *)SourceObject,
                        (data_type_definition const *)DestType,
                        (void *)DestObject,
                        (conversion_handler * )OverrideHandler);
}

GRANNY_DYNLINK(void) GrannyCopyMeshIndices(granny_mesh const * Mesh,
                                           granny_int32x BytesPerIndex,
                                           void * DestIndices)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    CopyMeshIndices((mesh const &)*Mesh,
                    (int32x)BytesPerIndex,
                    (void *)DestIndices);
}

GRANNY_DYNLINK(void) GrannyCopyMeshVertices(granny_mesh const * Mesh,
                                            granny_data_type_definition const * VertexType,
                                            void * DestVertices)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    CopyMeshVertices((mesh const &)*Mesh,
                     (data_type_definition const *)VertexType,
                     (void *)DestVertices);
}

GRANNY_DYNLINK(void) GrannyDeformVertices(granny_mesh_deformer const * Deformer,
                                          granny_int32x const * MatrixIndices,
                                          granny_real32 const * MatrixBuffer4x4,
                                          granny_int32x VertexCount,
                                          void const * SourceVertices,
                                          void * DestVertices)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    DeformVertices((mesh_deformer const &)*Deformer,
                   (int32x const *)MatrixIndices,
                   (real32 const *)MatrixBuffer4x4,
                   (int32x)VertexCount,
                   (void const *)SourceVertices,
                   (void *)DestVertices);
}

GRANNY_DYNLINK(bool) GrannyFindBoneByName(granny_skeleton const * Skeleton,
                                          char const * BoneName,
                                          granny_int32x * BoneIndex)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((bool)FindBoneByName((skeleton const *)Skeleton,
                                 (char const *)BoneName,
                                 (int32x &)*BoneIndex));
}

GRANNY_DYNLINK(bool) GrannyFindMatchingMember(granny_data_type_definition const * SourceType,
                                              void const * SourceObject,
                                              char const * DestMemberName,
                                              granny_variant *  Result)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((bool)FindMatchingMember((data_type_definition const *)SourceType,
                                     (void const *)SourceObject,
                                     (char const *)DestMemberName,
                                     (variant * )Result));
}

GRANNY_DYNLINK(void) GrannyFreeCompletedModelControls(granny_model_instance const * ModelInstance)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeCompletedModelControls((model_instance const &)*ModelInstance);
}

GRANNY_DYNLINK(void) GrannyFreeControl(granny_control * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeControl((control *)Control);
}

GRANNY_DYNLINK(bool) GrannyFreeControlIfComplete(granny_control * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((bool)FreeControlIfComplete((control *)Control));
}

GRANNY_DYNLINK(void) GrannyFreeControlOnceUnused(granny_control * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeControlOnceUnused((control &)*Control);
}

GRANNY_DYNLINK(void) GrannyFreeFile(granny_file * File)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeFile((file *)File);
}

GRANNY_DYNLINK(void) GrannyFreeFileSection(granny_file * File,
                                           granny_int32x SectionIndex)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeFileSection((file &)*File,
                    (int32x)SectionIndex);
}

GRANNY_DYNLINK(void) GrannyFreeLocalPose(granny_local_pose * LocalPose)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeLocalPose((local_pose *)LocalPose);
}

GRANNY_DYNLINK(void) GrannyFreeMeshBinding(granny_mesh_binding * Binding)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeMeshBinding((mesh_binding *)Binding);
}

GRANNY_DYNLINK(void) GrannyFreeMeshDeformer(granny_mesh_deformer * Deformer)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeMeshDeformer((mesh_deformer *)Deformer);
}

GRANNY_DYNLINK(void) GrannyFreeModelInstance(granny_model_instance * ModelInstance)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeModelInstance((model_instance *)ModelInstance);
}

GRANNY_DYNLINK(void) GrannyFreeWorldPose(granny_world_pose * WorldPose)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    FreeWorldPose((world_pose *)WorldPose);
}

GRANNY_DYNLINK(granny_real32) GrannyGetControlLocalDuration(granny_control const * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    real32 ReturnValue;
    return *(granny_real32 *)&(ReturnValue = GetControlLocalDuration((control const &)*Control));
}

GRANNY_DYNLINK(granny_int32x) GrannyGetControlLoopCount(granny_control const * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    int32x ReturnValue;
    return *(granny_int32x *)&(ReturnValue = GetControlLoopCount((control const &)*Control));
}

GRANNY_DYNLINK(granny_real32) GrannyGetControlRawLocalClock(granny_control * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    real32 ReturnValue;
    return *(granny_real32 *)&(ReturnValue = GetControlRawLocalClock((control &)*Control));
}

GRANNY_DYNLINK(granny_real32) GrannyGetControlSpeed(granny_control const * Control)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    real32 ReturnValue;
    return *(granny_real32 *)&(ReturnValue = GetControlSpeed((control const &)*Control));
}

GRANNY_DYNLINK(granny_file_info *) GrannyGetFileInfo(granny_file * File)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_file_info *)GetFileInfo((file &)*File));
}

GRANNY_DYNLINK(char const* ) GrannyGetLogMessageOriginString(granny_log_message_origin Origin)
{
    return ((char const* )GetLogMessageOriginString((log_message_origin)Origin));
}

GRANNY_DYNLINK(char const* ) GrannyGetLogMessageTypeString(granny_log_message_type Type)
{
    return ((char const* )GetLogMessageTypeString((log_message_type)Type));
}

GRANNY_DYNLINK(granny_texture *) GrannyGetMaterialTextureByType(granny_material const * Material,
                                                                granny_material_texture_type Type)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_texture *)GetMaterialTextureByType((material const *)Material,
                                                       (material_texture_type)Type));
}

GRANNY_DYNLINK(granny_int32x const *) GrannyGetMeshBindingToBoneIndices(granny_mesh_binding const * Binding)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_int32x const *)GetMeshBindingToBoneIndices((mesh_binding const &)*Binding));
}

GRANNY_DYNLINK(granny_int32x) GrannyGetMeshIndexCount(granny_mesh const * Mesh)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    int32x ReturnValue;
    return *(granny_int32x *)&(ReturnValue = GetMeshIndexCount((mesh const &)*Mesh));
}

GRANNY_DYNLINK(granny_int32x) GrannyGetMeshTriangleGroupCount(granny_mesh const * Mesh)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    int32x ReturnValue;
    return *(granny_int32x *)&(ReturnValue = GetMeshTriangleGroupCount((mesh const &)*Mesh));
}

GRANNY_DYNLINK(granny_tri_material_group *) GrannyGetMeshTriangleGroups(granny_mesh const * Mesh)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_tri_material_group *)GetMeshTriangleGroups((mesh const &)*Mesh));
}

GRANNY_DYNLINK(granny_int32x) GrannyGetMeshVertexCount(granny_mesh const * Mesh)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    int32x ReturnValue;
    return *(granny_int32x *)&(ReturnValue = GetMeshVertexCount((mesh const &)*Mesh));
}

GRANNY_DYNLINK(granny_data_type_definition *) GrannyGetMeshVertexType(granny_mesh const * Mesh)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_data_type_definition *)GetMeshVertexType((mesh const &)*Mesh));
}

GRANNY_DYNLINK(void *) GrannyGetMeshVertices(granny_mesh const * Mesh)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((void *)GetMeshVertices((mesh const &)*Mesh));
}

GRANNY_DYNLINK(granny_skeleton *) GrannyGetSourceSkeleton(granny_model_instance const * Model)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_skeleton *)GetSourceSkeleton((model_instance const &)*Model));
}

GRANNY_DYNLINK(granny_int32x) GrannyGetTotalTypeSize(granny_data_type_definition const * TypeDefinition)
{
    int32x ReturnValue;
    return *(granny_int32x *)&(ReturnValue = GetTotalTypeSize((data_type_definition const *)TypeDefinition));
}

GRANNY_DYNLINK(granny_real32 *) GrannyGetWorldPose4x4(granny_world_pose const * WorldPose,
                                                      granny_int32x BoneIndex)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_real32 *)GetWorldPose4x4((world_pose const &)*WorldPose,
                                             (int32x)BoneIndex));
}

GRANNY_DYNLINK(granny_real32 *) GrannyGetWorldPoseComposite4x4(granny_world_pose const * WorldPose,
                                                               granny_int32x BoneIndex)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_real32 *)GetWorldPoseComposite4x4((world_pose const &)*WorldPose,
                                                      (int32x)BoneIndex));
}

GRANNY_DYNLINK(granny_matrix_4x4 *) GrannyGetWorldPoseComposite4x4Array(granny_world_pose const * WorldPose)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_matrix_4x4 *)GetWorldPoseComposite4x4Array((world_pose const &)*WorldPose));
}

GRANNY_DYNLINK(granny_model_instance *) GrannyInstantiateModel(granny_model const * Model)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_model_instance *)InstantiateModel((model const &)*Model));
}

GRANNY_DYNLINK(bool) GrannyMeshIsRigid(granny_mesh const * Mesh)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((bool)MeshIsRigid((mesh const &)*Mesh));
}

GRANNY_DYNLINK(granny_local_pose *) GrannyNewLocalPose(granny_int32x BoneCount)
{
    return ((granny_local_pose *)NewLocalPose((int32x)BoneCount));
}

GRANNY_DYNLINK(granny_mesh_binding *) GrannyNewMeshBinding(granny_mesh const * Mesh,
                                                           granny_skeleton const * FromSkeleton,
                                                           granny_skeleton const * ToSkeleton)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_mesh_binding *)NewMeshBinding((mesh const &)*Mesh,
                                                  (skeleton const &)*FromSkeleton,
                                                  (skeleton const &)*ToSkeleton));
}

GRANNY_DYNLINK(granny_mesh_deformer *) GrannyNewMeshDeformer(granny_data_type_definition const * InputVertexLayout,
                                                             granny_data_type_definition const * OutputVertexLayout,
                                                             granny_deformation_type DeformationType,
                                                             granny_deformer_tail_flags TailFlag)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_mesh_deformer *)NewMeshDeformer((data_type_definition const *)InputVertexLayout,
                                                    (data_type_definition const *)OutputVertexLayout,
                                                    (deformation_type)DeformationType,
                                                    (deformer_tail_flags)TailFlag));
}

GRANNY_DYNLINK(granny_world_pose *) GrannyNewWorldPose(granny_int32x BoneCount)
{
    return ((granny_world_pose *)NewWorldPose((int32x)BoneCount));
}

GRANNY_DYNLINK(granny_control *) GrannyPlayControlledAnimation(granny_real32 StartTime,
                                                               granny_animation const * Animation,
                                                               granny_model_instance * Model)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    return ((granny_control *)PlayControlledAnimation((real32)StartTime,
                                                      (animation const &)*Animation,
                                                      (model_instance &)*Model));
}

GRANNY_DYNLINK(granny_file *) GrannyReadEntireFileFromMemory(granny_int32x MemorySize,
                                                             void const*  Memory)
{
    return ((granny_file *)ReadEntireFileFromMemory((int32x)MemorySize,
                                                    (void const* )Memory));
}

GRANNY_DYNLINK(void) GrannySampleModelAnimationsAccelerated(granny_model_instance const * ModelInstance,
                                                            granny_int32x BoneCount,
                                                            granny_real32 const * Offset4x4,
                                                            granny_local_pose * Scratch,
                                                            granny_world_pose * Result)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SampleModelAnimationsAccelerated((model_instance const &)*ModelInstance,
                                     (int32x)BoneCount,
                                     (real32 const *)Offset4x4,
                                     (local_pose &)*Scratch,
                                     (world_pose &)*Result);
}

GRANNY_DYNLINK(void) GrannySetControlEaseIn(granny_control * Control,
                                            bool EaseIn)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetControlEaseIn((control &)*Control,
                     (bool)EaseIn);
}

GRANNY_DYNLINK(void) GrannySetControlEaseInCurve(granny_control * Control,
                                                 granny_real32 StartSeconds,
                                                 granny_real32 EndSeconds,
                                                 granny_real32 StartValue,
                                                 granny_real32 StartTangent,
                                                 granny_real32 EndTangent,
                                                 granny_real32 EndValue)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetControlEaseInCurve((control &)*Control,
                          (real32)StartSeconds,
                          (real32)EndSeconds,
                          (real32)StartValue,
                          (real32)StartTangent,
                          (real32)EndTangent,
                          (real32)EndValue);
}

GRANNY_DYNLINK(void) GrannySetControlEaseOut(granny_control * Control,
                                             bool EaseOut)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetControlEaseOut((control &)*Control,
                      (bool)EaseOut);
}

GRANNY_DYNLINK(void) GrannySetControlEaseOutCurve(granny_control * Control,
                                                  granny_real32 StartSeconds,
                                                  granny_real32 EndSeconds,
                                                  granny_real32 StartValue,
                                                  granny_real32 StartTangent,
                                                  granny_real32 EndTangent,
                                                  granny_real32 EndValue)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetControlEaseOutCurve((control &)*Control,
                           (real32)StartSeconds,
                           (real32)EndSeconds,
                           (real32)StartValue,
                           (real32)StartTangent,
                           (real32)EndTangent,
                           (real32)EndValue);
}

GRANNY_DYNLINK(void) GrannySetControlLoopCount(granny_control * Control,
                                               granny_int32x LoopCount)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetControlLoopCount((control &)*Control,
                        (int32x)LoopCount);
}

GRANNY_DYNLINK(void) GrannySetControlRawLocalClock(granny_control * Control,
                                                   granny_real32 LocalClock)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetControlRawLocalClock((control &)*Control,
                            (real32)LocalClock);
}

GRANNY_DYNLINK(void) GrannySetControlSpeed(granny_control * Control,
                                           granny_real32 Speed)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetControlSpeed((control &)*Control,
                    (real32)Speed);
}

GRANNY_DYNLINK(void) GrannySetLogCallback(granny_log_callback const * LogCallback)
{
    SetLogCallback((log_callback const &)*LogCallback);
}

GRANNY_DYNLINK(void) GrannySetModelClock(granny_model_instance const * ModelInstance,
                                         granny_real32 NewClock)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyGetAllowGlobalStateChanges())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: global state modifications not enabled",
                                    LogCallback.UserData);
        }
    }
    else if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    SetModelClock((model_instance const &)*ModelInstance,
                  (real32)NewClock);
}

GRANNY_DYNLINK(void) GrannyUpdateModelMatrix(granny_model_instance const * ModelInstance,
                                             granny_real32 SecondsElapsed,
                                             granny_real32 const * ModelMatrix4x4,
                                             granny_real32 * DestMatrix4x4,
                                             bool Inverse)
{
#ifdef GRANNY_THREAD_CHECKS
    if (!GrannyThreadAllowedToCallGranny())
    {
        granny_log_callback LogCallback;
        GrannyGetLogCallback(&LogCallback);
        if (LogCallback.Function)
        {
            (*LogCallback.Function)(GrannyErrorLogMessage, GrannyThreadSafetyLogMessage,
                                    __FILE__, __LINE__,
                                    "ThreadingError: attempted to access granny from the wrong thread",
                                    LogCallback.UserData);
        }
    }
#endif // GRANNY_THREAD_CHECKS

    UpdateModelMatrix((model_instance const &)*ModelInstance,
                      (real32)SecondsElapsed,
                      (real32 const *)ModelMatrix4x4,
                      (real32 *)DestMatrix4x4,
                      (bool)Inverse);
}

GRANNY_DYNLINKDATA(granny_data_type_definition *) GrannyPNT332VertexType = (granny_data_type_definition *)(void*)(PNT332VertexType);