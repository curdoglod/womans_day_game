#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

struct PlayerRecord
{
    int id;
    std::string name;
    long int time;
};

class ScoreBoard
{
public:
    explicit ScoreBoard(const std::string &fileName)
        : fileName_(fileName), nextId_(0)
    {
        loadFromFile();
    }

    void loadFromFile()
    {
        records_.clear();
        nextId_ = 0;

        std::ifstream in(fileName_);
        if (!in.is_open())
        {
            return;
        }

        std::string line;
        while (std::getline(in, line))
        {
            if (line.empty())
                continue;

            std::stringstream ss(line);

            std::string first;
            std::string second;
            std::string third;

            if (std::getline(ss, first, ',') &&
                std::getline(ss, second, ',') &&
                std::getline(ss, third))
            {
                try
                {
                    int id = std::stoi(first);
                    long int time = std::stol(third);
                    const std::string &name = second;

                    records_.push_back({id, name, time});
                    if (id > nextId_)
                        nextId_ = id;
                    continue;
                }
                catch (...)
                {
                }
            }

            ss.clear();
            ss.str(line);

            std::string name;
            std::string timeStr;

            if (std::getline(ss, name, ',') &&
                std::getline(ss, timeStr))
            {
                try
                {
                    long int time = std::stol(timeStr);
                    int id = ++nextId_; // auto-generate id
                    records_.push_back({id, name, time});
                }
                catch (...)
                {
                    continue;
                }
            }
        }

        sortByTime();
    }

    void saveToFile() const
    {
        std::ofstream out(fileName_, std::ios::trunc);
        if (!out.is_open())
        {
            return;
        }

        for (const auto &r : records_)
        {
            // Save in format: id,name,time
            out << r.id << "," << r.name << "," << r.time << "\n";
        }
    }

    void addRecord(const std::string &name, long int time)
    {
        int id = ++nextId_;
        records_.push_back({id, name, time});
        sortByTime();
    }

    const std::vector<PlayerRecord> &getRecords() const
    {
        return records_;
    }

    void clear()
    {
        records_.clear();
        nextId_ = 0;
    }

    std::vector<PlayerRecord> getTop(std::size_t count) const
    {
        std::vector<PlayerRecord> result = records_;
        if (result.size() > count)
            result.resize(count);
        return result;
    }

    const int& getLastId(){
        return nextId_; 
    } 

private:
    void sortByTime()
    {
        std::sort(records_.begin(), records_.end(),
                  [](const PlayerRecord &a, const PlayerRecord &b)
                  {
                      return a.time < b.time;
                  });
    }

    std::string fileName_;
    std::vector<PlayerRecord> records_;
    int nextId_;
};