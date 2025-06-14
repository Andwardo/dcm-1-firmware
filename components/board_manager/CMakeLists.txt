# /…/components/board_manager/CMakeLists.txt

idf_component_register(
    SRCS "board_manager.c"
    INCLUDE_DIRS "."            # only the local folder
    PRIV_REQUIRES driver        # <-- add 'driver' so driver/gpio.h is found
)
