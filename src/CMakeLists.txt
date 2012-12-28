add_executable(CarDetect CarDetect.cpp haardetect.cpp)
target_link_libraries(CarDetect ${OpenCV_LIBS})

add_executable(NPR NPRMain.cpp)
target_link_libraries(NPR ${OpenCV_LIBS})

install(TARGETS CarDetect NPR DESTINATION bin)
