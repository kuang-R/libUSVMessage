add_library(libUSVMessage
	crc16.c message.c message_type.c)

target_include_directories(libUSVMessage
	PUBLIC "${PROJECT_BINARY_DIR}"
	)
