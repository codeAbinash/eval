// #include "./engine/mainEngine.hpp"
// #include "./define.hpp"

int mainEval(argCVDef) {
    if (argc == 1) {
        // Direct run,
        mainEngine(argCV);
        return 0;
    }
    if (argc == 2) {
        // Check for commands
        if (ifDashCommand(argCV)) {
            // Execute the specific command
            return executeCommand(argCV);
        }
        // The second argument is an expression
        string exp (argv[1]);
        cout << dye::grey("< ") << dye::light_green(eval(exp)) << endl;
        return 0;
    }
    return 0;
}