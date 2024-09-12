#include "invoice_database.hpp"
#include <iostream>

/// @brief Initialization must be done outside to prevent race conditions (in case threads are used)
InvoiceDatabase* InvoiceDatabase::singletonInstance = nullptr;

InvoiceDatabase::InvoiceDatabase()
{
    // Create or open the database file
    invoiceDatabase.reset(new SQLite::Database(INVOICE_DATABASE_PATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE));
    openStatus = true;

    // Create the table if it doesn't exist
    invoiceDatabase->exec("CREATE TABLE IF NOT EXISTS invoices ("
                          "invoice_number TEXT PRIMARY KEY, "
                          "store_identification TEXT, "
                          "subtotal_price REAL, "
                          "tax_price REAL, "
                          "total_price REAL, "
                          "amount_paid REAL, "
                          "cash_change REAL, "
                          "operator_identification TEXT);");
}

// Singleton instance accessor
InvoiceDatabase* InvoiceDatabase::getInstance()
{
    if (singletonInstance == nullptr)
    {
        singletonInstance = new InvoiceDatabase();
    }
    return singletonInstance;
}

void InvoiceDatabase::addInvoice(const Invoice& invoice)
{
    // Compile a SQL query
    SQLite::Statement query(*invoiceDatabase,
                            "INSERT OR REPLACE INTO invoices (invoice_number, store_identification, subtotal_price, tax_price, total_price, amount_paid, cash_change, operator_identification) "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");

    // Bind SQL fields to invoices fields
    query.bind(1, invoice.invoiceNumber);
    query.bind(2, invoice.storeIdentification);
    query.bind(3, invoice.subtotalPrice);
    query.bind(4, invoice.taxPrice);
    query.bind(5, invoice.totalPrice);
    query.bind(6, invoice.amountPaid);
    query.bind(7, invoice.cashChange);
    query.bind(8, invoice.operatorIdentification);

    // Execute the query
    query.exec();
}

Invoice* InvoiceDatabase::getInvoice(const std::string& invoiceNumber) const
{
    SQLite::Statement query(*invoiceDatabase,
                            "SELECT invoice_number, store_identification, subtotal_price, tax_price, total_price, amount_paid, cash_change, operator_identification "
                            "FROM invoices WHERE invoice_number = ?;");

    query.bind(1, invoiceNumber);

    if (query.executeStep())
    {
        // Create an Invoice object and populate its fields with the query result
        auto* invoice                   = new Invoice();

        invoice->invoiceNumber          = query.getColumn(0).getString();
        invoice->storeIdentification    = query.getColumn(1).getString();
        invoice->subtotalPrice          = query.getColumn(2).getDouble();
        invoice->taxPrice               = query.getColumn(3).getDouble();
        invoice->totalPrice             = query.getColumn(4).getDouble();
        invoice->amountPaid             = query.getColumn(5).getDouble();
        invoice->cashChange             = query.getColumn(6).getDouble();
        invoice->operatorIdentification = query.getColumn(7).getString();

        return invoice;
    }

    return nullptr; // Return nullptr if no invoice is found
}

void InvoiceDatabase::printAllInvoices() const
{
    SQLite::Statement query(*invoiceDatabase, "SELECT * FROM invoices;");

    // Check if there are results
    if (!query.executeStep())
    {
        std::cout << "--- No invoices in database! ---\n\n";
        return;
    }

    // Iterate over all results
    do
    {
        std::string invoiceNumber = query.getColumn(0).getString();
        Invoice* invoice          = getInvoice(invoiceNumber);

        if (invoice)
        {
            invoice->printSavedData();
            std::cout << std::endl;
            delete invoice; // Clean up after using the invoice
        }
    } while (query.executeStep());
}

void InvoiceDatabase::removeAllInvoices()
{
    // Check if the database exists and is accessible
    if (!openStatus)
    {
        std::cout << "--- Database does not exist or is not accessible! ---\n\n";
        return;
    }

    // Check if there are any invoices in the database
    SQLite::Statement checkQuery(*invoiceDatabase, "SELECT COUNT(*) FROM invoices;");
    checkQuery.executeStep();

    if (checkQuery.getColumn(0).getInt() == 0)
    {
        std::cout << "---- No invoices to delete. The database is empty! ---\n\n";
        return;
    }

    // Proceed with deletion if invoices exist
    SQLite::Transaction transaction(*invoiceDatabase);

    // Check if the invoice_products table exists
    bool tableExists = false;
    SQLite::Statement checkTableQuery(*invoiceDatabase, "SELECT name FROM sqlite_master WHERE type='table' AND name='invoice_products';");
    if (checkTableQuery.executeStep())
    {
        tableExists = true;
    }

    // Delete all entries from invoice_products if the table exists
    if (tableExists)
    {
        SQLite::Statement productsQuery(*invoiceDatabase, "DELETE FROM invoice_products;");
        productsQuery.exec();
    }

    // Then delete all entries from invoices
    SQLite::Statement invoiceQuery(*invoiceDatabase, "DELETE FROM invoices;");
    invoiceQuery.exec();

    transaction.commit();

    std::cout << "--- Invoices deleted! ---\n\n";
}