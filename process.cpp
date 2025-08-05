#include "process.h"
#include "memory-manager.h"
#include <sstream>
#include <iostream>

extern MemoryManager memManager;

// Old ctor
Process::Process(
    const std::string& process_name,
    int process_id,
    int num_instructions
) : name(process_name),
id(process_id),
total_instructions(num_instructions),
remaining_instructions(num_instructions),
core_id(-1),
memory_required(0),
is_allocated(false),
memory_ptr(nullptr),
num_pages(0),
instr_index(0)
{
}

// New ctor with memory and pages
Process::Process(
    const std::string& process_name,
    int process_id,
    int num_instructions,
    size_t mem_required,
    int num_pages
) : name(process_name),
id(process_id),
total_instructions(num_instructions),
remaining_instructions(num_instructions),
core_id(-1),
memory_required(mem_required),
is_allocated(false),
memory_ptr(nullptr),
num_pages(num_pages),
instr_index(0)
{
}

// New ctor with instruction list
Process::Process(
    const std::string& process_name,
    int process_id,
    size_t mem_required,
    int num_pages,
    const std::vector<std::string>& instr_list
) : name(process_name),
id(process_id),
total_instructions(static_cast<int>(instr_list.size())),
remaining_instructions(static_cast<int>(instr_list.size())),
core_id(-1),
memory_required(mem_required),
is_allocated(false),
memory_ptr(nullptr),
num_pages(num_pages),
instructions(instr_list),
instr_index(0)
{
}

Process::~Process() = default;

void Process::execute_instruction(int core)
{
    if (instr_index < instructions.size()) {
        std::istringstream iss(instructions[instr_index]);
        std::string op;
        iss >> op;
        if (op == "DECLARE") {
            std::string var; uint16_t val;
            iss >> var >> val;
            vars[var] = val;
        }
        else if (op == "ADD") {
            std::string dst, a, b;
            iss >> dst >> a >> b;
            vars[dst] = vars[a] + vars[b];
        }
        else if (op == "WRITE") {
            size_t addr; std::string var;
            iss >> std::hex >> addr >> var;
            memManager.write_memory(addr, vars[var]);
        }
        else if (op == "READ") {
            std::string var; size_t addr;
            iss >> var >> std::hex >> addr;
            vars[var] = memManager.read_memory(addr);
        }
        else if (op == "PRINT") {
            std::string token;
            std::getline(iss, token);
            auto plus = token.find('+');
            std::string lit = token.substr(0, plus);
            std::string var = token.substr(plus + 1);
            auto start = lit.find('"'), end = lit.rfind('"');
            std::string literal = lit.substr(start + 1, end - start - 1);
            while (var.size() && isspace(var[0])) var.erase(0, 1);
            std::cout << literal << vars[var] << "\n";
        }
        instr_index++;
    }
    else if (remaining_instructions > 0) {
        remaining_instructions--;
    }
}

// Accessor definitions
int Process::get_remaining_instructions() const {
    return remaining_instructions;
}
int Process::get_total_instructions() const {
    return total_instructions;
}
bool Process::has_finished() const {
    return remaining_instructions <= 0;
}
int Process::get_process_id() const {
    return id;
}
std::string Process::get_process_name() const {
    return name;
}
void Process::set_core_id(int core) {
    core_id = core;
}
int Process::get_num_pages() const {
    return num_pages;
}

// Memory accessors
size_t Process::get_memory_required() const {
    return memory_required;
}
bool Process::get_is_allocated() const {
    return is_allocated;
}
void* Process::get_memory_ptr() const {
    return memory_ptr;
}
void Process::set_is_allocated(bool allocated) {
    is_allocated = allocated;
}
void Process::set_memory_ptr(void* ptr) {
    memory_ptr = ptr;
}
