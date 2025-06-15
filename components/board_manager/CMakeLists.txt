# components/board_manager/CMakeLists.txt
#
# Register the board_manager component with its source, include path,
# and required dependencies.

idf_component_register(
    SRCS
        "board_manager.c"

    INCLUDE_DIRS
        "include"

    PRIV_REQUIRES
        driver
        nvs_flash
    REQUIRES
	driver
)
