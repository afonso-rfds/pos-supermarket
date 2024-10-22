#pragma once

#include "database/databases_manager/databases_manager.hpp"
#include "state_machine/context/pos/pos_context.hpp"
#include "state_machine/states/pos_base_state.hpp"

#include <string.h>

class IdleState : public BaseState
{
  public:
    IdleState()  = default;
    ~IdleState() = default;

    void enterState(Context& context) override;
    void processState(Context& context) override;

  private:
    /// @brief Check weather the operator's name is valid (alphabetic chars only)
    /// @param operatorsName Name of operator to validade
    /// @return true:  Valid name.
    ///         false: Invalid name
    bool isValidName(std::string operatorsName);

    /// @brief Process the beginning of a transaction
    /// @param context Current state machine context
    void processTransaction(Context& context);

    /// @brief Member that deals with the databases
    DatabasesManager databasesManager;
};
