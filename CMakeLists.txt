cmake_minimum_required(VERSION 3.18)
project(AITrack)

include(cmake/GitUtils.cmake)

SET(ONNX_PKG_NAME "onnxruntime-win-x64-1.4.0")

SET(DEPS_DIR "Dependencies/")

SET(ONNXRUNTIME_URL "https://github.com/microsoft/onnxruntime/releases/download/v1.4.0/onnxruntime-win-x64-1.4.0.zip")
SET(ONNXRUNTIME_PATH "Dependencies/${ONNX_PKG_NAME}.zip")
SET(ONNXRUNTIME_EXTRACT_PATH "Dependencies/")


#SET(PS3Driver_URL "")
#SET(PS3Driver_PATH "")
SET(OPENCV_URL "https://github.com/opencv/opencv/releases/download/4.6.0/opencv-4.6.0-vc14_vc15.exe")
SET(OPENCV_PATH "${DEPS_DIR}opencv-4.6.0-vc14_vc15.exe")

SET(LIBUSB_URL "https://github.com/libusb/libusb/releases/download/v1.0.23/libusb-1.0.23.7z")
SET(LIBUSB_NAME "libusb-1.0.23")



if (NOT EXISTS "${ONNXRUNTIME_PATH}")
    file(DOWNLOAD "${ONNXRUNTIME_URL}" "${ONNXRUNTIME_PATH}")
	file(ARCHIVE_EXTRACT 
		INPUT "${ONNXRUNTIME_PATH}"
		DESTINATION "${ONNXRUNTIME_EXTRACT_PATH}")	
	file(RENAME "${ONNXRUNTIME_EXTRACT_PATH}/${ONNX_PKG_NAME}" "${ONNXRUNTIME_EXTRACT_PATH}/onnxruntime")
endif()

if (NOT EXISTS "${DEPS_DIR}${LIBUSB_NAME}.zip")
    file(DOWNLOAD "${LIBUSB_URL}" "${DEPS_DIR}${LIBUSB_NAME}.zip")
	file(ARCHIVE_EXTRACT 
		INPUT "${DEPS_DIR}${LIBUSB_NAME}.zip"
		DESTINATION "${DEPS_DIR}/libusb")
endif()

if (NOT EXISTS "Dependencies/PS3EyeDriver")
	git_clone(
			PROJECT_NAME    PS3EYEDriver
			GIT_URL         https://github.com/inspirit/PS3EYEDriver.git
			GIT_BRANCH      master)
			
	file(RENAME "PS3EyeDriver" "Dependencies/PS3EyeDriver")
endif()


if (NOT EXISTS "Dependencies/OpenCV")
	file(DOWNLOAD "${OPENCV_URL}" "${OPENCV_PATH}")
	FILE(MAKE_DIRECTORY "Dependencies/OpenCV")
	file(ARCHIVE_EXTRACT 
		INPUT "${OPENCV_PATH}"
		DESTINATION "${DEPS_DIR}OpenCVextracted")
	file(COPY "${DEPS_DIR}OpenCVextracted/opencv/build/x64/vc15/lib" DESTINATION "${DEPS_DIR}OpenCV/")
	file(COPY "${DEPS_DIR}OpenCVextracted/opencv/build/include" DESTINATION "${DEPS_DIR}OpenCV/")
	file(COPY "${DEPS_DIR}OpenCVextracted/opencv/build/include/opencv2/." DESTINATION "${DEPS_DIR}OpenCV/include/" FILES_MATCHING PATTERN "*.hpp")
	#file(COPY "${DEPS_DIR}OpenCVextracted/opencv/build/bin" DESTINATION "${DEPS_DIR}OpenCV/")
endif()


