find_ros_package(genmsg_cpp)

# Message-generation support.
macro(genmsg_cpp)
  get_msgs(_msglist)
  set(_autogen "")
  foreach(_msg ${_msglist})
    # Construct the path to the .msg file
    set(_input ${PROJECT_SOURCE_DIR}/msg/${_msg})
  
    gendeps(${PROJECT_NAME} ${_msg})
  
    set(genmsg_cpp_exe ${genmsg_cpp_PACKAGE_PATH}/genmsg)

    set(_output_cpp ${PROJECT_SOURCE_DIR}/msg/cpp/${PROJECT_NAME}/${_msg})
    string(REPLACE ".msg" ".h" _output_cpp ${_output_cpp})
  
    # Add the rule to build the .h the .msg
    add_custom_command(OUTPUT ${_output_cpp} 
                       COMMAND ${genmsg_cpp_exe} ${_input}
                       DEPENDS ${_input} ${genmsg_cpp_exe} ${gendeps_exe} ${${PROJECT_NAME}_${_msg}_GENDEPS} ${ROS_MANIFEST_LIST})
    list(APPEND _autogen ${_output_cpp})
  endforeach(_msg)
  # Create a target that depends on the union of all the autogenerated
  # files
  add_custom_target(ROSBUILD_genmsg_cpp DEPENDS ${_autogen})
  # Add our target to the top-level genmsg target, which will be fired if
  # the user calls genmsg()
  add_dependencies(rospack_genmsg ROSBUILD_genmsg_cpp)
endmacro(genmsg_cpp)

# Call the macro we just defined.
genmsg_cpp()

# Service-generation support.
macro(gensrv_cpp)
  get_srvs(_srvlist)
  set(_autogen "")
  foreach(_srv ${_srvlist})
    # Construct the path to the .srv file
    set(_input ${PROJECT_SOURCE_DIR}/srv/${_srv})
  
    gendeps(${PROJECT_NAME} ${_srv})
  
    set(gensrv_cpp_exe ${genmsg_cpp_PACKAGE_PATH}/gensrv)

    set(_output_cpp ${PROJECT_SOURCE_DIR}/srv/cpp/${PROJECT_NAME}/${_srv})
    string(REPLACE ".srv" ".h" _output_cpp ${_output_cpp})
  
    # Add the rule to build the .h from the .srv
    add_custom_command(OUTPUT ${_output_cpp} 
                       COMMAND ${gensrv_cpp_exe} ${_input}
                       DEPENDS ${_input} ${gensrv_cpp_exe} ${gendeps_exe} ${${PROJECT_NAME}_${_srv}_GENDEPS} ${ROS_MANIFEST_LIST})
    list(APPEND _autogen ${_output_cpp})
  endforeach(_srv)
  # Create a target that depends on the union of all the autogenerated
  # files
  add_custom_target(ROSBUILD_gensrv_cpp DEPENDS ${_autogen})
  # Add our target to the top-level gensrv target, which will be fired if
  # the user calls gensrv()
  add_dependencies(rospack_gensrv ROSBUILD_gensrv_cpp)
endmacro(gensrv_cpp)

# Call the macro we just defined.
gensrv_cpp()
