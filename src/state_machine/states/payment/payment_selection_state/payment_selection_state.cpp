#include "payment_selection_state.hpp"

#include "state_machine/states/payment/partial_payment_state/partial_payment_state.hpp"

#include <iostream>

void PaymentSelectionState::enterState(Context& context)
{
    std::cout << "\n\n-----------------------------------\n";
    std::cout << "  --- Total to pay:     " << context.getTransactionData().getSubtotalPrice() + context.getTransactionData().getTaxPrice() << "€ ---\n";
    std::cout << "  --- Remaining to pay: " << context.getTransactionData().getRemainingToPay() << "€ ---\n";
    std::cout << "-----------------------------------\n\n";
}

void PaymentSelectionState::processState(Context& context)
{
    do
    {
        std::cout << "You can pay via MBWay, card or cash\n";
        std::cout << "Payment method: ";

        getUserInput();

    } while (userInput != "MBWAY" && userInput != "CARD" && userInput != "CASH");

    context.getTransactionData().setPaymentMethod(userInput);
    context.transitionToState(StateType::PartialPayment);
}