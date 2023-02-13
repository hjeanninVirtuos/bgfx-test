# vts_bgfx_compile_shader_to_header(
# 	TYPE VERTEX|FRAGMENT|COMPUTE
# 	SHADERS filenames
# 	VARYING_DEF filename
# 	OUTPUT_DIR directory
# )
#
# VTS-GSL (qbazin):
# I needed to change this function because it couldn't find "bgfx::shaderc", so I put "$<TARGET_FILE:shaderc>" instead
# Also, after the copy, the include path for the shaders was wrong so I added it manually
#
function(vts_bgfx_compile_shader_to_header)
	set(options "")
	set(oneValueArgs TYPE VARYING_DEF OUTPUT_DIR)
	set(multiValueArgs SHADERS)
	cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

	set(PROFILES 120 300_es spirv) # pssl
	if(UNIX AND NOT APPLE)
		set(PLATFORM LINUX)
	elseif(EMSCRIPTEN)
		set(PLATFORM ASM_JS)
	elseif(APPLE)
		set(PLATFORM OSX)
		list(APPEND PROFILES metal)
	elseif(
		WIN32
		OR MINGW
		OR MSYS
		OR CYGWIN
	)
		set(PLATFORM WINDOWS)
		if(ARGS_TYPE STREQUAL "VERTEX" OR ARGS_TYPE STREQUAL "FRAGMENT")
			list(APPEND PROFILES s_3_0)
			list(APPEND PROFILES s_4_0)
			list(APPEND PROFILES s_5_0)
		elseif(ARGS_TYPE STREQUAL "COMPUTE")
			list(APPEND PROFILES s_4_0)
			list(APPEND PROFILES s_5_0)
		else()
			message(error "shaderc: Unsupported type")
		endif()
	else()
		message(error "shaderc: Unsupported platform")
	endif()

	foreach(SHADER_FILE ${ARGS_SHADERS})
		source_group("Shaders" FILES "${SHADER}")
		get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
		get_filename_component(SHADER_FILE_NAME_WE ${SHADER_FILE} NAME_WE)
		get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)

		# Build output targets and their commands
		set(OUTPUTS "")
		set(COMMANDS "")
		foreach(PROFILE ${PROFILES})
			_bgfx_get_profile_ext(${PROFILE} PROFILE_EXT)
			set(OUTPUT ${ARGS_OUTPUT_DIR}/${SHADER_FILE_BASENAME}.${PROFILE_EXT}.bin.h)
			set(PLATFORM_I ${PLATFORM})
			if(PROFILE STREQUAL "spirv")
				set(PLATFORM_I LINUX)
			endif()
			_bgfx_shaderc_parse(
				CLI #
				${ARGS_TYPE} ${PLATFORM_I} WERROR "$<$<CONFIG:debug>:DEBUG>$<$<CONFIG:relwithdebinfo>:DEBUG>"
				FILE ${SHADER_FILE_ABSOLUTE}
				OUTPUT ${OUTPUT}
				PROFILE ${PROFILE}
				O "$<$<CONFIG:debug>:0>$<$<CONFIG:release>:3>$<$<CONFIG:relwithdebinfo>:3>$<$<CONFIG:minsizerel>:3>"
				VARYINGDEF ${ARGS_VARYING_DEF}
				INCLUDES ${CMAKE_SOURCE_DIR}/modules/bgfx/bgfx/src
				BIN2C BIN2C ${SHADER_FILE_NAME_WE}_${PROFILE_EXT}
			)
			list(APPEND OUTPUTS ${OUTPUT})
			list(APPEND COMMANDS COMMAND $<TARGET_FILE:shaderc> ${CLI})
		endforeach()

		add_custom_command(
			OUTPUT ${OUTPUTS}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${ARGS_OUTPUT_DIR} ${COMMANDS}
			MAIN_DEPENDENCY ${SHADER_FILE_ABSOLUTE}
			DEPENDS ${ARGS_VARYING_DEF}
		)
	endforeach()
endfunction()
