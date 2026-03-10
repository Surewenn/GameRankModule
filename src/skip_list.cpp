#include "skip_list.h"
#include "random_utils.hpp"

/// @brief SkipList 构造函数
SkipList::SkipList()
    : level_(1),
      length_(0)
{
    head_ = new SkiplistNode(kMaxLevel, 0.0f, "") ;
}

/// @brief SkipList 析构函数
SkipList::~SkipList() {
    // 从底层链表开始释放所有节点
    clear();
    delete head_; 
}

/// @brief 随机层高
/// @return 随机选中的层高数
int SkipList::randomLevel() {
    int lvl = 1;
    while ((RandomUtils::rand() & 0xFFFF) < static_cast<int>(kProbability * 0xFFFF)
        && lvl < kMaxLevel) {
        lvl++ ;        
    }
    return lvl ;
}

/// @brief SkipList 插入操作
/// @param score 分数
/// @param member 成员
/// @return 
bool SkipList::insert(double score, const std::string &member) {
    // update 数组存储当前数据对应每一层的前驱阶段
    std::vector<SkiplistNode*> update(static_cast<int>(kMaxLevel)) ;
    SkiplistNode* node = head_ ;
    for (int i = level_ - 1; i >= 0;i--) {
        while (
            node->forward[i]
            && 
            less_score_member(
                node->forward[i]->score,
                node->forward[i]->member,
                score,
                member
            )
        ) {
            node = node->forward[i]; // node 的作用就是跳表 Jump 的过程
        }
        update[i] = node ;
    }

    // node->forward[0] 0层存储所有数据 因此如果存在，必定可以获得
    node = node->forward[0] ;
    if (node && node->score == score && node->member == member) {
        return false ;
    }
    int lvl = randomLevel();
    if (lvl > level_) {
        for (int i = level_; i < lvl;i++) {
            update[i] = head_ ;
        }
        level_ = lvl ;
    }
    SkiplistNode* nx = new SkiplistNode(lvl, score, member) ;
    for (int i = 0;i < lvl;i++) {
        nx->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = nx;
    }
    ++length_;
    return true ;
}

/// @brief 删除数据
/// @param score 
/// @param member 
/// @return 删除是否成功
bool SkipList::erase(double score, const std::string &member) {
    std::vector<SkiplistNode*> update(static_cast<int>(kMaxLevel)) ;
    SkiplistNode* node = head_ ;
    for (int i = level_ - 1; i >= 0;i--) {
        while (
            node->forward[i]
            && 
            less_score_member(
                node->forward[i]->score,
                node->forward[i]->member,
                score,
                member
            )
        ) {
            node = node->forward[i];
        }
        update[i] = node ;
    }
    node = node->forward[0] ;
    if (!node || node->score != score || node->member != member) {
        return false ;
    }
    for (int i = 0;i < node->forward.size();i++) {
        if (update[i]->forward[i] == node) {
            update[i]->forward[i] = node->forward[i] ;
        } 
    }
    delete node ;
    while (level_ > 1 && head_->forward[level_-1] == nullptr) {
        level_--;
    }
    length_-- ;
    return true ;
}

SkiplistNode* SkipList::find(double score, const std::string &member) const {
    SkiplistNode* node = head_ ;
    for (int i = level_ - 1; i >= 0;i--) {
        while (
            node->forward[i]
            && 
            less_score_member(
                node->forward[i]->score,
                node->forward[i]->member,
                score,
                member
            )
        ) {
            node = node->forward[i];
        }
    }
    node = node->forward[0] ;
    if (!node || node->score != score || node->member != member) {
        return nullptr ;
    }
    return node ;
}

/// @brief 清空跳表数据
void SkipList::clear() {
    SkiplistNode *cur  = head_->forward[0];
    while (cur) {
        SkiplistNode *next = cur->forward[0] ;
        delete cur ;
        cur = next ;
    }
    for (int i = 0; i < kMaxLevel; ++i) {
        head_->forward[i] = nullptr;
    }
    level_ = 1;
    length_ = 0;
}

/// @brief 范围查询
/// @param start 起始点
/// @param stop  结束点
/// @param out  输出
void SkipList::rangeByRank(int64_t start, int64_t stop, std::vector<std::string> &out) const {
    out.clear() ;
    if (length_ == 0) return;
    // norm
    auto norm = [&](int64_t pos) -> int64_t  {
        if (pos < 0) pos = pos + length_ ;
        if (pos < 0) pos = 0;
        if (pos >= length_) pos = length_ - 1;
        return pos ;
    };
    int64_t realStartPos = norm(start) ;
    int64_t realStopPos = norm(stop) ;
    if (realStartPos > realStopPos) {
        return ; // 传入参数错误
    }
    SkiplistNode* node = head_->forward[0] ;
    if (node == nullptr) {
        return ;
    }
    int64_t pos = 0;
    while (node != nullptr && pos < realStartPos) {
        node = node->forward[0]; 
        pos++;
    }
    while (node != nullptr && pos <= realStopPos) {
        out.push_back(node->member) ;
        node = node->forward[0]; 
        pos ++ ;
    }
}


void SkipList::toVector(std::vector<std::pair<double, std::string>> &out) const {
    out.clear() ;
    SkiplistNode* node = head_->forward[0] ;
    while (node != nullptr) {
        out.emplace_back(node->score, node->member) ;
        node = node->forward[0] ;
    }
}