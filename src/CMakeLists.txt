add_library(libUSVMessage
	crc16.c message.c message_type.c)

# 设置c语言版本
set_property(TARGET libUSVMessage PROPERTY C_STANDARD 99)
set_property(TARGET libUSVMessage PROPERTY C_STANDARD_REQUIRED True)

target_include_directories(libUSVMessage
	PUBLIC "${PROJECT_BINARY_DIR}"
	)
