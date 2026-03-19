# Test executables

# Simple test to verify linking
add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic PRIVATE regent Catch2::Catch2WithMain)
add_test(NAME BasicTest COMMAND test_basic)

# CoNLL-U parser test
add_executable(test_conllu test_conllu.cpp)
target_link_libraries(test_conllu PRIVATE regent Catch2::Catch2WithMain)
add_test(NAME CoNLLUTest COMMAND test_conllu)

# Dependency graph test
add_executable(test_dep_graph test_dep_graph.cpp)
target_link_libraries(test_dep_graph PRIVATE regent Catch2::Catch2WithMain)
add_test(NAME DepGraphTest COMMAND test_dep_graph)

# Transformer test
add_executable(test_transformer test_transformer.cpp)
target_link_libraries(test_transformer PRIVATE regent Catch2::Catch2WithMain)
add_test(NAME TransformerTest COMMAND test_transformer)

# Lineariser test
add_executable(test_lineariser test_lineariser.cpp)
target_link_libraries(test_lineariser PRIVATE regent Catch2::Catch2WithMain)
add_test(NAME LineariserTest COMMAND test_lineariser)

# Integration test
add_executable(test_integration test_integration.cpp)
target_link_libraries(test_integration PRIVATE regent Catch2::Catch2WithMain)
add_test(NAME IntegrationTest COMMAND test_integration)
