#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <memory>

/// @brief 跳表对比排序规则 优先比较积分 之后比较成员字典序
static inline bool less_score_member(double a_sc, const std::string & a_m, double b_sc, const std::string & b_m) {
    if (a_sc != b_sc) {
        return a_sc < b_sc ;
    }
    return a_m < b_m ;
}

struct SkiplistNode {
    double score ;
    std::string member ;
    std::vector<SkiplistNode*> forward ; // 指向各层下一个节点的指针 数组
    SkiplistNode(int level, double sc, const std::string &mem)
        : score(sc), member(mem), forward(static_cast<size_t>(level), nullptr) {} 
};


class SkipList {
private:
    SkiplistNode *head_ ; // 头结点，不存储实际数据
    int level_;           // 当前跳表实际使用的最大层数（从1开始计数）
    int length_;          // 节点总数（不含头结点）

    static constexpr int kMaxLevel = 32;    // 最大允许层数
    static constexpr double kProbability = 0.25;  // 晋升概率

public:
    SkipList() ;
    virtual ~SkipList() ;

public:
    bool insert(double score, const std::string &member) ;
    bool erase(double score, const std::string &member) ;
    SkiplistNode* find(double score, const std::string &member) const;
    void rangeByRank(int64_t start, int64_t stop, std::vector<std::string> &out) const ;
    void toVector(std::vector<std::pair<double, std::string>> &out) const ;
    void clear() ;
    int size() const { return length_; }
    int level() const { return level_; }


private:
    int randomLevel() ; // 随机生成新节点的层高
};