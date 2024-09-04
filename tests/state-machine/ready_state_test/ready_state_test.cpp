#include "ready_state_test.hpp"

#include "state_machine/idle_state/idle_state.hpp"
#include "state_machine/items_state/items_state.hpp"

INLINE_FUNCTION void ReadyStateTest::createCustomInputString(std::string inputString)
{
    std::istringstream input(inputString);
    std::cin.rdbuf(input.rdbuf());
}

TEST_F(ReadyStateTest, enterState)
{
    EXPECT_TRUE(true);
}

TEST_F(ReadyStateTest, processState_toItemsState)
{
    createCustomInputString("");

    readyState = new ReadyState();
    posContext = new POSContext(readyState);

    readyState->processState(*posContext);
    ASSERT_TRUE(dynamic_cast<ItemsState*>(posContext->getCurrentState()) != nullptr);
}

TEST_F(ReadyStateTest, processState_toIdleState)
{
    createCustomInputString("cancel");

    readyState = new ReadyState();
    posContext = new POSContext(readyState);

    readyState->processState(*posContext);
    ASSERT_TRUE(dynamic_cast<IdleState*>(posContext->getCurrentState()) != nullptr);
}