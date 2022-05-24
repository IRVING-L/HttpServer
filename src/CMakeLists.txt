aux_source_directory(. SRC_LIST)
aux_source_directory(db/. DB_LIST)
aux_source_directory(model/. MODEL_LIST)

aux_source_directory(log/. LOG_LIST)

add_executable(server ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${LOG_LIST})


target_link_libraries(server mysqlclient pthread)