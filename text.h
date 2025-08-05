#pragma once

namespace text {
    const char header[] = ""
      "  ****  ****    ***     *****   ******   ****    *     *\n"
      " *      *      *   *    *    *  *        *        *   * \n"
      "*       ****  *     *   *****   *****    ****      * *  \n"
      "*          *  *     *   *       *           *       *   \n"
      " *         *   *   *    *       *           *       *   \n"
      "  ****  ****    ***     *       ******   ****       *   \n";
    const char welcome[] = "\033[32mHello, Command Line Emulator v1.0\033[0m\n";
    const char instruction[] = 
        "\033[38;5;229mType 'exit' to quit, 'clear' to clear the screen,\n"
        "and help to show commands and their description\033[0m\n";

    const char processHeader[] = ""
        "---------------------------------\n"
        "Running processes: \n";

    namespace cpu_util
    {
        const char header[] = 
            "---------------------------------------------------------------------------\n"
            "|                      |         Util (%)          |No# processes in queue|\n"
            "---------------------------------------------------------------------------\n";
        const char body[] = "|{:.22}|{:.27d}|{:.22d}|\n";
        const char footer[] =
            "---------------------------------------------------------------------------";
    }

    namespace error {
        namespace no_file {
            const char help_txt[] = "File help.txt not found";
            const char screen_help_txt[] = "File screen-help.txt not found";
        }
    
    }

    namespace file {
        const char help_txt[] = "help.txt";
        const char screen_help_txt[] = "screen-help.txt";
        const char config_txt[] = "config.txt";
    }
}

