cmake_minimum_required(VERSION 3.9.0)

include(CMakeDependentOption)
include(CheckLanguage)

check_language(CSharp)

CMAKE_DEPENDENT_OPTION(ASAR_GEN_EXE_TEST "Build Asar standalone application test suite" ON "ASAR_GEN_EXE" OFF)
CMAKE_DEPENDENT_OPTION(ASAR_GEN_DLL_TEST "Build Asar shared library test suite" ON "ASAR_GEN_DLL" OFF)
CMAKE_DEPENDENT_OPTION(ASAR_APITESTS_C "Build Asar C API test suite" ON "ASAR_GEN_DLL" OFF)
CMAKE_DEPENDENT_OPTION(ASAR_APITESTS_CSHARP "Build Asar C# API test suite" ON "ASAR_GEN_DLL;CMAKE_CSharp_COMPILER" OFF)

# Shared settings for Asar test applications

macro(set_asar_test_shared_properties target)
    # Maximum warning level
    if(MSVC)
        target_compile_definitions(${target} PRIVATE "_CRT_SECURE_NO_WARNINGS")
        target_compile_options(${target} PRIVATE /Wall)

        # These certainly aren't worth a warning, though
        target_compile_options(${target} PRIVATE
            /wd4514 # unreferenced inline function removed
            /wd4710 # function not inlined
            /wd4711 # function selected for automatic inline expansion
            /wd4820 # 'bytes' bytes padding added after construct 'member_name'
        )
		
		if(MSVC_VERSION VERSION_LESS "1900")
			target_compile_features(${target} PRIVATE cxx_std_11)
		else()
			# MSVC throws errors in STL headers when building with MSVC 2017 without C++14...
			target_compile_features(${target} PRIVATE cxx_std_14)
		endif()
    else()
        target_link_libraries(${target} dl)
        target_compile_options(${target} PRIVATE -Wall -pedantic
			-Wno-varargs # passing an object that undergoes default argument promotion to 'va_start' has undefined behavior
			-Wno-unused-result # ignoring return value
		)
        # for some reason this isn't available on MSVC?
        target_compile_features(${target} PRIVATE c_std_99)
		target_compile_features(${target} PRIVATE cxx_std_11)
    endif()
endmacro()

#Macros for API Tests

macro(add_c_api_test testname)
	add_executable(
		asar-c-api-${testname}

		"test-${testname}.cpp"
		"util.cpp"

		"../asar-dll-bindings/asardll.c"
		"../asar-dll-bindings/asardll.h"
	)
	add_dependencies(asar-c-api-${testname} asar)
	set_asar_test_shared_properties(asar-c-api-${testname})

	target_include_directories(asar-c-api-${testname} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../asar-dll-bindings/")
	add_custom_command(TARGET asar-c-api-${testname} POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:asar>" "${CMAKE_CURRENT_BINARY_DIR}" VERBATIM)

	add_test(NAME test-asar-c-api-${testname}
				 COMMAND asar-c-api-${testname} "${CMAKE_SOURCE_DIR}/tests")
endmacro()

macro(add_csharp_api_test testname)
	add_executable(
		asar-csharp-api-${testname}

		"test-${testname}.cs"

		"../asar-dll-bindings/asar.cs"
	)
	target_compile_options(
		asar-csharp-api-${testname}
		PRIVATE /unsafe /langversion:6
	)
	add_dependencies(asar-csharp-api-${testname} asar)
	add_custom_command(TARGET asar-csharp-api-${testname} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"$<TARGET_FILE:asar>" "$<TARGET_FILE_DIR:asar-csharp-api-${testname}>" VERBATIM)

	add_test(NAME test-asar-csharp-api-${testname}
				 COMMAND asar-csharp-api-${testname} "${CMAKE_SOURCE_DIR}/tests")
endmacro()

set(CXX_EXTENSIONS OFF)

# The main test application's two build options

if(ASAR_GEN_EXE_TEST)
	add_executable(
		asar-app-test

		"test.cpp"
		"util.cpp"
	)

	set_asar_test_shared_properties(asar-app-test)

	if(ASAR_GEN_EXE)
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests_tmp_app")
		add_test(
			NAME EXE_TEST
			COMMAND asar-app-test "$<TARGET_FILE:asar-standalone>" "${CMAKE_SOURCE_DIR}/tests" "${CMAKE_SOURCE_DIR}/dummy_rom.sfc" "${CMAKE_CURRENT_BINARY_DIR}/tests_tmp_app")
	endif()
endif()

if(ASAR_GEN_DLL_TEST)
	add_executable(
		asar-dll-test

		"test.cpp"
		"util.cpp"

		"../asar-dll-bindings/asardll.c"
		"../asar-dll-bindings/asardll.h"
	)

	target_include_directories(asar-dll-test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../asar-dll-bindings/")
	target_compile_definitions(asar-dll-test PRIVATE ASAR_TEST_DLL)

	set_asar_test_shared_properties(asar-dll-test)
	if(ASAR_GEN_EXE)
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests_tmp_dll")
		add_test(
			NAME DLL_TEST
			COMMAND asar-dll-test "$<TARGET_FILE:asar>" "${CMAKE_SOURCE_DIR}/tests" "${CMAKE_SOURCE_DIR}/dummy_rom.sfc" "${CMAKE_CURRENT_BINARY_DIR}/tests_tmp_dll")
	endif()
endif()

# API test Applications

if(ASAR_APITESTS_C)
	add_c_api_test(init)
endif()

if(ASAR_APITESTS_CSHARP)
	enable_language(CSharp)
	add_csharp_api_test(init)
endif()
