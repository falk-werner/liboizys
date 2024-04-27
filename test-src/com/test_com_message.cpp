#include "com_message.hpp"

#include <gtest/gtest.h>

TEST(com_message, construct_default)
{
    com::com_message message;
}

TEST(com_message, construct_by_content)
{
    std::string const content("some message");
    com::com_message message(content);

    ASSERT_EQ(content, message.payload);
    ASSERT_EQ(0, message.header[0]);
    ASSERT_EQ(0, message.header[1]);
    ASSERT_EQ(0, message.header[2]);
    ASSERT_EQ(12, message.header[3]);
}

TEST(com_message, construct_large_message)
{
    std::string const content(0x123456, ' ');
    com::com_message message(content);

    ASSERT_EQ(content, message.payload);
    ASSERT_EQ(0, message.header[0]);
    ASSERT_EQ(0x12, message.header[1]);
    ASSERT_EQ(0x34, message.header[2]);
    ASSERT_EQ(0x56, message.header[3]);
}
TEST(com_message, construct_max_size_message)
{
    std::string const content(0xffffff, ' ');
    com::com_message message(content);

    ASSERT_EQ(content, message.payload);
    ASSERT_EQ(0, message.header[0]);
    ASSERT_EQ(0xff, message.header[1]);
    ASSERT_EQ(0xff, message.header[2]);
    ASSERT_EQ(0xff, message.header[3]);
}

TEST(com_message, fail_to_construct_empty_message)
{
    std::string const content;
    ASSERT_ANY_THROW({
        com::com_message message(content);
    });
}

TEST(com_message, fail_to_construct_too_large_message)
{
    std::string const content(0x1000000, ' ');
    ASSERT_ANY_THROW({
        com::com_message message(content);
    });
}
