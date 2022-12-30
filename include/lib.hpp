#include "./define.hpp"

/**
 * @param s contains the first command line argument
 * @return true if the argument is a command
 */
bool ifDashCommand(argCVDef) {
    char *s = argv[1];
    if (strlen(s) > 2 && s[0] == '-' && s[1] == '-') {
        /**
         * It seems that the entered command must be system command because it
         * starts with '--'
         */
        // executeCommand(argCV);
        return true;
    }
    return false;
}