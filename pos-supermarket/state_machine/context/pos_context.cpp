#include "pos_context.hpp"
#include "state_machine/states/pos_state.hpp"

#include <cmath>

POSContext::POSContext(POSState* initialState, std::string storeIdentifier) : currentState(initialState), transactionData(storeIdentifier)
{
    currentState->enterState(*this);
}

void POSContext::setState(POSState* newState)
{
    currentState.reset(newState); // Clean up the old state and switch to the new one
    currentState->enterState(*this);
}

void POSContext::processCurrentState()
{
    currentState->processState(*this);
}

void POSContext::transitionToState(StateType newState)
{
    setState(createState(newState));
}

POSState* POSContext::createState(StateType newState)
{
    switch (newState)
    {
    case StateType::Idle:
        return new IdleState();
    case StateType::Ready:
        return new ReadyState();
    case StateType::Items:
        return new ItemsState();
    case StateType::PaymentSelection:
        return new PaymentSelectionState();
    case StateType::PartialPayment:
        return new PartialPaymentState();
    case StateType::PaymentCompletion:
        return new PaymentCompletionState();
    default:
        return nullptr;
    }
}

//**** -------------- Getters -------------- ****

POSState* POSContext::getCurrentState() const
{
    return currentState.get();
}

TransactionData& POSContext::getTransactionData()
{
    return transactionData;
}
