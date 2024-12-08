#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <cpuid.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <thread>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

// Define command to clear terminal
#ifdef __linux__
    char clear_screen_command[6] = "clear";
#elif _WIN32 or _WIN64
    std::cout << "This is version for linux, please download windows version."
    return 0;
#else
    std::cout << "Undefined operating system" << std::endl;
    return 0;
#endif

void clear_screen() {
    // Clear the terminal
    system(clear_screen_command);
}

void blankline() {
    // Leave an empty line
    std::cout << '\n';
}

void sleep(int seconds) {
    // Do nothing for X seconds
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void get_time() {
    // Get current time
    auto time = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(time);
    std::cout << "Current time: " << std::ctime(&time_now);
}

bool if_file_exists(const std::string& filename) {
    // Check if a file exists
    return std::filesystem::exists(filename);
}

void config() {
    // Configure hardware
    clear_screen();
    // Host
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    // Cpu
    long number_of_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    // Memory
    long number_of_pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    int memory_size = number_of_pages*page_size/pow(1024,2);
    std::string choice;
    // Create/open config file
    if (!if_file_exists("Assets/client-config.conf")) {
        std::cout << "Creating configuration file" << std::endl;
        std::ofstream write_config ("Assets/client-config.conf");
        write_config.close();
    }
    std::ofstream write_config;
    write_config.open("Assets/client-config.conf");
    // Host
    std::cout << "Host:" << std::endl;
    std::cout << "Hostname: " << hostname << std::endl;
    write_config << "HOSTNAME= " << hostname << std::endl;
    blankline();
    // Cpu
    std::cout << "CPU:" << std::endl;
    std::cout << "Detected CPU cores: " << number_of_cpus << std::endl;
    write_config << "NUMBER_OF_CPUS= " << number_of_cpus << std::endl;
    std::cout << "CPU config saved" << std::endl;
    blankline();
    //Memory
    std::cout << "Memory:" << std::endl;
    std::cout << "Detected memory size: " << memory_size << std::endl;
    write_config << "MEMORY_SIZE= " << memory_size << std::endl;
    std::cout << "Memory config saved" << std::endl;
    write_config.close();
    blankline();
}

void socket_server() {
    // Socket server configurtion
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    // Server address
    server_address.sin_addr.s_addr = INADDR_ANY;
    // Get hostname
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    // Connect with openpanel-main
    connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    // Send message to openpanel-main
    const char* message = hostname;
    send(client_socket, message, strlen(message), 0);
    close(client_socket);
}

int main(int argc, char const *argv[]) {
    int command;
    // Main function
    clear_screen();
    std::cout << "Panel - Client" << std::endl;
    blankline();
    // Selftest here
    // Wait for user input to continue
    do {
        std::cout << "Press enter to continue..." << std::endl;
    } while (std::cin.get() != '\n');
    // Main menu
    clear_screen();
    while (command != 99) {
        std::cout << "> ";
        if (!(std::cin >> command)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            clear_screen();
            std::cout << "Invalid argument" << std::endl;
            blankline();
            continue;
        }
        switch (command) {
            case 0:
                config();
                break;
            case 3:
                socket_server();
                break;
            case 99:
                std::cout << "Exit" << std::endl;
                return 0;
                break;
            default:
                clear_screen();
                std::cout << "Invalid argument" << std::endl;
                blankline();
                break;
        }
    }
}