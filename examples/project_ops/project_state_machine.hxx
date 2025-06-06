/************************************************************************
Copyright 2017-2019 eBay Inc.
Author/Developer(s): Jung-Sang Ahn

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**************************************************************************/

#pragma once

#include "nuraft.hxx"

#include <atomic>
#include <cassert>
#include <iostream>
#include <mutex>
#include <sstream>

using namespace nuraft;

class project_state_machine : public state_machine {
public:
    project_state_machine()
        : last_committed_idx_(0){

    }

    ~project_state_machine() {}

    ptr<buffer> pre_commit(const ulong log_idx, buffer& data) {
        // Extract string from `data.
        buffer_serializer bs(data);
        std::string str = bs.get_str();

        // Just print.
        std::cout << "pre_commit " << log_idx << ": "
                  << str << std::endl;
        return nullptr;
    }

    ptr<buffer> commit(const ulong log_idx, buffer& data) {
        // Extract string from `data.
        buffer_serializer bs(data);
        std::string str = bs.get_str();



        auto now = std::chrono::system_clock::now();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        std::cout << "[commitCmd #" << cmd_number << "] [Time (us): " << micros << "] " <<"<<" << str << ">>" << std::endl;
        cmd_number++;


        std::istringstream iss(str);
        int command = -1;

        // Extract command, check if extraction succeeded
        bool parse_result = static_cast<bool>(iss >> command);
        assert(parse_result && "Failed to parse command integer from input string");


        if (command >= 0 && command <= 3) {
            int id;
            if (!(iss >> id)) {
                std::cerr << "Expected ID after command " << command << "\n";
                return nullptr;
            }

            switch (command) {
            case 0: projects.insert(id); break;
            case 1:
                projects.erase(id);
                workson.erase(id);
                break;
            case 2: employees.insert(id); break;
            case 3:
                employees.erase(id);
                for (auto& [proj, emps] : workson) {
                    emps.erase(id);
                }
                break;
            }
        } else if (command == 4) {
            std::string token;
            if (!(iss >> token)) {
                std::cerr << "Expected mapping like 1-2 after 4\n";
                return nullptr;
            }
            size_t dash = token.find('-');
            if (dash == std::string::npos) {
                std::cerr << "Invalid format: " << token << "\n";
                return nullptr;
            }

            int proj = std::stoi(token.substr(0, dash));
            int emp = std::stoi(token.substr(dash + 1));

            if (projects.count(proj) && employees.count(emp)) {
                workson[proj].insert(emp);
            } else {
                std::cerr << "Invalid mapping: project or employee does not exist\n";
            }
        } else {
            std::cerr << "Unknown command: " << command << "\n";
        }

        // Update last committed index number.
        last_committed_idx_ = log_idx;
        return nullptr;
    }

    void commit_config(const ulong log_idx, ptr<cluster_config>& new_conf) {
        // Nothing to do with configuration change. Just update committed index.
        last_committed_idx_ = log_idx;
    }

    void rollback(const ulong log_idx, buffer& data) {
        // Extract string from `data.
        buffer_serializer bs(data);
        std::string str = bs.get_str();

        // Just print.
        std::cout << "rollback " << log_idx << ": "
                  << str << std::endl;
    }

    int read_logical_snp_obj(snapshot& s,
                             void*& user_snp_ctx,
                             ulong obj_id,
                             ptr<buffer>& data_out,
                             bool& is_last_obj)
    {
        // Put dummy data.
        data_out = buffer::alloc( sizeof(int32) );
        buffer_serializer bs(data_out);
        bs.put_i32(0);

        is_last_obj = true;
        return 0;
    }

    void save_logical_snp_obj(snapshot& s,
                              ulong& obj_id,
                              buffer& data,
                              bool is_first_obj,
                              bool is_last_obj)
    {
        std::cout << "save snapshot " << s.get_last_log_idx()
                  << " term " << s.get_last_log_term()
                  << " object ID " << obj_id << std::endl;
        // Request next object.
        obj_id++;
    }

    bool apply_snapshot(snapshot& s) {
        std::cout << "apply snapshot " << s.get_last_log_idx()
                  << " term " << s.get_last_log_term() << std::endl;
        // Clone snapshot from `s`.
        {   std::lock_guard<std::mutex> l(last_snapshot_lock_);
            ptr<buffer> snp_buf = s.serialize();
            last_snapshot_ = snapshot::deserialize(*snp_buf);
        }
        return true;
    }

    void free_user_snp_ctx(void*& user_snp_ctx) { }

    ptr<snapshot> last_snapshot() {
        // Just return the latest snapshot.
        std::lock_guard<std::mutex> l(last_snapshot_lock_);
        return last_snapshot_;
    }

    ulong last_commit_index() {
        return last_committed_idx_;
    }

    void create_snapshot(snapshot& s,
                         async_result<bool>::handler_type& when_done)
    {
        std::cout << "create snapshot " << s.get_last_log_idx()
                  << " term " << s.get_last_log_term() << std::endl;
        // Clone snapshot from `s`.
        {   std::lock_guard<std::mutex> l(last_snapshot_lock_);
            ptr<buffer> snp_buf = s.serialize();
            last_snapshot_ = snapshot::deserialize(*snp_buf);
        }
        ptr<std::exception> except(nullptr);
        bool ret = true;
        when_done(ret, except);
    }

private:
    // Last committed Raft log number.
    std::atomic<uint64_t> last_committed_idx_;

    // Last snapshot.
    ptr<snapshot> last_snapshot_;

    // Mutex for last snapshot.
    std::mutex last_snapshot_lock_;

    std::unordered_set<int> projects;
    std::unordered_set<int> employees;
    std::unordered_map<int, std::unordered_set<int>> workson;




    int cmd_number = 0;
};

