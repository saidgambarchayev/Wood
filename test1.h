#ifndef TEST_H
#define TEST_H
#include <iostream>

#define ASSERT_EQ(val1, val2) \
if ((val1) != (val2)) { \
std::cerr << __FILE__ << ":" << __LINE__ << ": error: Expected " << #val1 << " == " << #val2 << " (" << (val1) << " vs " << (val2) << ")\n"; \
return false; \
}

#define ASSERT_TRUE(val) \
if (!(val)) { \
std::cerr << __FILE__ << ":" << __LINE__ << ": error: Expected " << #val << " to be true\n"; \
return false; \
}

#define TEST(test_suite_name, test_name) \
bool test_suite_name##_##test_name()

#define RUN_TEST(test_suite_name, test_name) \
if (!test_suite_name##_##test_name()) { \
std::cerr << #test_suite_name "." #test_name " failed.\n"; \
} else { \
std::cout << #test_suite_name "." #test_name " passed.\n"; \
}

#endif
