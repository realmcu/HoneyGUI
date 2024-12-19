
set(BOX2D_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_broad_phase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_chain_shape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_circle_shape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_collide_circle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_collide_edge.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_collide_polygon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_collision.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_distance.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_dynamic_tree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_edge_shape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_polygon_shape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/collision/b2_time_of_impact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/common/b2_block_allocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/common/b2_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/common/b2_math.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/common/b2_settings.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/common/b2_stack_allocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/common/b2_timer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_body.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_chain_circle_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_chain_circle_contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_chain_polygon_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_chain_polygon_contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_circle_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_circle_contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_contact_manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_contact_solver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_contact_solver.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_distance_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_edge_circle_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_edge_circle_contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_edge_polygon_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_edge_polygon_contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_fixture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_friction_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_gear_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_island.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_island.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_motor_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_mouse_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_polygon_circle_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_polygon_circle_contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_polygon_contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_polygon_contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_prismatic_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_pulley_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_revolute_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_weld_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_wheel_joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_world.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dynamics/b2_world_callbacks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/rope/b2_rope.cpp
	CACHE INTERNAL  "sources")

set(BOX2D_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/../include/box2d/
	${CMAKE_CURRENT_SOURCE_DIR}/../include/
	CACHE INTERNAL  "headers")


