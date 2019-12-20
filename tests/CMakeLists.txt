
add_executable(frp_test
    test_main.cpp
    stream_test.cpp
    detail_signal_test.cpp
    )

target_link_libraries(frp_test
    frp_external::Catch2
    frp
    )
