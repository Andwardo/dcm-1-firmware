#
# Register the board_manager component with its source, include path,
# and required dependencies.
#
idf_component_register(
    SRCS
        "board_manager.c"
    INCLUDE_DIRS
        "include"
    # The 'driver' component (for GPIOs) is only used by board_manager.c,
    # making it a private requirement.
    PRIV_REQUIRES
        driver
)
