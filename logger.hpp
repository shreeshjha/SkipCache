#include "ReadableFlexibleLogger.hpp"
#include <iostream>

int main() {
    try {
        ReadableFlexibleLogger logger("persistent_readable_log.txt");
        logger.log("User login event");
        logger.log("Transaction committed");
        logger.log("Error encountered during processing");
        std::cout << "Enhanced, human-readable logging completed." << std::endl;
    } catch(const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
