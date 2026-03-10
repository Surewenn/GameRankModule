#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "skip_list.h"  // 假设你的跳表类定义在 skiplist.h 中

void printSkipList(const SkipList& sl, const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
    std::vector<std::pair<double, std::string>> vec;
    sl.toVector(vec);
    std::cout << "size = " << sl.size() << ", level = " << sl.level() << "\n";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << "[" << i << "] (" << vec[i].first << ", " << vec[i].second << ")\n";
    }
}

void testBasic() {
    std::cout << "\n========== testBasic ==========\n";
    SkipList sl;

    // 插入几个元素
    assert(sl.insert(3.5, "apple"));
    assert(sl.insert(2.0, "banana"));
    assert(sl.insert(1.0, "cherry"));
    assert(sl.insert(2.5, "date"));
    assert(sl.insert(4.0, "elderberry"));

    assert(sl.size() == 5);
    printSkipList(sl, "after inserts");

    // 查找存在的节点
    auto node = sl.find(2.0, "banana");
    assert(node != nullptr && node->score == 2.0 && node->member == "banana");

    // 查找不存在的节点
    node = sl.find(3.0, "fig");
    assert(node == nullptr);

    // 插入已存在的 member（允许，因为跳表不处理唯一性，但测试一下）
    assert(sl.insert(2.0, "banana") == false);  // 完全相同，返回 false
    assert(sl.insert(2.5, "banana"));            // 不同 score，可以插入（测试重复 member）
    assert(sl.size() == 6);
    printSkipList(sl, "after inserting duplicate banana");
}

void testErase() {
    std::cout << "\n========== testErase ==========\n";
    SkipList sl;
    sl.insert(1.0, "a");
    sl.insert(2.0, "b");
    sl.insert(3.0, "c");
    sl.insert(4.0, "d");
    sl.insert(5.0, "e");
    assert(sl.size() == 5);

    // 删除中间节点
    assert(sl.erase(3.0, "c"));
    assert(sl.size() == 4);
    assert(sl.find(3.0, "c") == nullptr);
    printSkipList(sl, "after erase middle");

    // 删除头节点
    assert(sl.erase(1.0, "a"));
    assert(sl.size() == 3);
    assert(sl.find(1.0, "a") == nullptr);
    printSkipList(sl, "after erase first");

    // 删除尾节点
    assert(sl.erase(5.0, "e"));
    assert(sl.size() == 2);
    assert(sl.find(5.0, "e") == nullptr);
    printSkipList(sl, "after erase last");

    // 删除不存在的节点
    assert(sl.erase(10.0, "x") == false);
    assert(sl.size() == 2);

    // 清空
    sl.clear();
    assert(sl.size() == 0);
    assert(sl.level() == 1);
    printSkipList(sl, "after clear");
}

void testRangeByRank() {
    std::cout << "\n========== testRangeByRank ==========\n";
    SkipList sl;
    sl.insert(1.0, "a");
    sl.insert(2.0, "b");
    sl.insert(3.0, "c");
    sl.insert(4.0, "d");
    sl.insert(5.0, "e");

    std::vector<std::string> out;

    // 正常范围
    sl.rangeByRank(1, 3, out);
    assert(out.size() == 3);
    assert(out[0] == "b" && out[1] == "c" && out[2] == "d");

    // 包含两端
    sl.rangeByRank(0, 4, out);
    assert(out.size() == 5);
    assert(out[0] == "a" && out[4] == "e");

    // 单元素
    sl.rangeByRank(2, 2, out);
    assert(out.size() == 1 && out[0] == "c");

    // 负数索引
    sl.rangeByRank(-3, -1, out);   // 倒数第3到倒数第1
    assert(out.size() == 3);
    assert(out[0] == "c" && out[1] == "d" && out[2] == "e");

    sl.rangeByRank(-1, -1, out);   // 最后一个
    assert(out.size() == 1 && out[0] == "e");

    // 越界索引（自动修正）
    sl.rangeByRank(-10, 10, out);  // 全部
    assert(out.size() == 5);

    sl.rangeByRank(10, 20, out);   // 空范围? 实际上 norm 会截断，返回最后一个? 注意实现：start>end 返回空
    // 根据你的实现，如果 start 和 stop 都被截断为同一个最大值，且 start <= stop，则返回最后一个元素。
    // 我们验证一下
    sl.rangeByRank(10, 20, out);
    // 因为 start=10 被截断为 4，stop=20 被截断为 4，所以应该返回一个元素 'e'
    assert(out.size() == 1 && out[0] == "e");

    // 无效范围：start > stop
    sl.rangeByRank(3, 1, out);
    assert(out.empty());

    // 空表
    sl.clear();
    sl.rangeByRank(0, 0, out);
    assert(out.empty());
}

void testOrdering() {
    std::cout << "\n========== testOrdering ==========\n";
    SkipList sl;
    // 插入一些无序的数据，验证 toVector 返回是否有序
    sl.insert(5.0, "e");
    sl.insert(1.0, "a");
    sl.insert(3.0, "c");
    sl.insert(2.0, "b");
    sl.insert(4.0, "d");

    std::vector<std::pair<double, std::string>> vec;
    sl.toVector(vec);
    assert(vec.size() == 5);
    for (size_t i = 0; i < vec.size() - 1; ++i) {
        assert(less_score_member(vec[i].first, vec[i].second, vec[i+1].first, vec[i+1].second));
    }
    std::cout << "Ordering is correct.\n";
}

void testRandomLevel() {
    std::cout << "\n========== testRandomLevel ==========\n";
    SkipList sl;
    // 插入大量元素，观察 level 是否增长
    for (int i = 0; i < 1000; ++i) {
        sl.insert(i * 1.0, "item" + std::to_string(i));
    }
    std::cout << "After 1000 inserts, level = " << sl.level() << "\n";
    assert(sl.size() == 1000);
    assert(sl.level() > 1);  // 大概率大于1
}

void testDuplicateMemberDifferentScore() {
    std::cout << "\n========== testDuplicateMemberDifferentScore ==========\n";
    SkipList sl;
    sl.insert(1.0, "x");
    sl.insert(2.0, "x");   // 相同 member，不同 score
    sl.insert(1.5, "x");   // 又一个
    assert(sl.size() == 3);

    std::vector<std::pair<double, std::string>> vec;
    sl.toVector(vec);
    // 应按照 score 排序
    assert(vec[0].first == 1.0 && vec[0].second == "x");
    assert(vec[1].first == 1.5 && vec[1].second == "x");
    assert(vec[2].first == 2.0 && vec[2].second == "x");

    // 删除其中一个
    assert(sl.erase(1.5, "x"));
    assert(sl.size() == 2);
    vec.clear();
    sl.toVector(vec);
    assert(vec[0].first == 1.0 && vec[0].second == "x");
    assert(vec[1].first == 2.0 && vec[1].second == "x");
}

int main() {
    testBasic();
    testErase();
    testRangeByRank();
    testOrdering();
    testRandomLevel();
    testDuplicateMemberDifferentScore();

    std::cout << "\nAll tests passed!\n";
    return 0;
}