set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(UI_SRCS
    UI/menu.cpp
    UI/main.cpp
    UI/rotating_textbox.cpp
)

set(OLED_SRCS
    OLED/DEV_Config.c
    OLED/OLED_Driver.c
)

set(UGUI_SRCS
    UGUI/ugui.c
)

file(GLOB UGUICPP_SRCS
    UGUICPP/*.cpp
)

set(COMPONENT_SRCS
    main.cpp
    rn52.cpp
    mediainfo.cpp
    display.cpp
    input.cpp
    loop.cpp
    ${UI_SRCS}
    ${OLED_SRCS}
    ${UGUI_SRCS}
    ${UGUICPP_SRCS}
)

set(COMPONENT_ADD_INCLUDEDIRS .)


register_component()
