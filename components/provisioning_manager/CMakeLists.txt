# /…/components/provisioning_manager/CMakeLists.txt

idf_component_register(
    SRCS "provisioning_manager.c"
    INCLUDE_DIRS "."          # include both root and the 'include/' subfolder
    PRIV_REQUIRES 
        wifi_provisioning               # for wifi_provisioning/scheme_http.h
        protocomm
        esp_event
)
