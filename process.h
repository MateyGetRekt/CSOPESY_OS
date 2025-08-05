#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <cstddef>

class Process
{
public:
    Process(const std::string& process_name,
        int process_id,
        int num_instructions);
    Process(const std::string& process_name,
        int process_id,
        int num_instructions,
        size_t mem_required,
        int num_pages);
    Process(const std::string& process_name,
        int process_id,
        size_t mem_required,
        int num_pages,
        const std::vector<std::string>& instr_list);
    ~Process();

    void execute_instruction(int core);

    // original accessors
    int get_remaining_instructions() const;
    int get_total_instructions() const;
    bool has_finished() const;
    int get_process_id() const;
    std::string get_process_name() const;
    void set_core_id(int core);
    int get_num_pages() const;

    // new memory accessors
    size_t get_memory_required() const;
    bool get_is_allocated() const;
    void set_is_allocated(bool allocated);
    void* get_memory_ptr() const;
    void set_memory_ptr(void* ptr);

private:
    std::string name;
    int id;
    int total_instructions;
    int remaining_instructions;
    int core_id;
    size_t memory_required{ 0 };
    bool is_allocated{ false };
    void* memory_ptr{ nullptr };
    int num_pages{ 0 };

    // for variable support
    std::map<std::string, uint16_t> vars;
    std::vector<std::string> instructions;
    size_t instr_index{ 0 };
};
