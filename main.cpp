#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class trie_impl {
public:

    struct node {
        bool terminating{};
        std::unordered_map<char, node*> children;
    };

    using size_type = size_t;
    using value_type = node;
    using pointer_type = node*;
    using const_pointer_type = const node*;
    using reference = node&;
    using const_reference = const node&;

    trie_impl() = default;

    void destroy() noexcept {
        destroy(root_);
        this->root_ = nullptr;
    }

    void destroy(const_pointer_type n) noexcept {

        if(!n) return;

        for(auto [k, v] : n->children) {
            destroy(v);
        }

        delete n;
    }

    ~trie_impl() {
        destroy();
    }

    pointer_type root_{};
    size_type count_{};
};

class trie : private trie_impl {
public:
    using trie_impl::node;
    using trie_impl::size_type;
    using trie_impl::value_type;
    using trie_impl::pointer_type;
    using trie_impl::const_pointer_type;
    using trie_impl::reference;
    using trie_impl::const_reference;

    trie() {
        this->root_ = new node{};
    }

    trie(trie const& rhs) {
        this->root_ = new node{};
        copy(this->root_, rhs.root_);
    }

    trie& operator=(trie const& rhs) {
        if(this != &rhs) {
            auto copy = rhs;
            *this = std::move(copy);
        }

        return *this;
    }

    trie(trie&& rhs) noexcept {
        this->root_ = std::exchange(rhs.root_, nullptr);
        this->count_ = std::exchange(rhs.count_, 0);
    }

    trie& operator=(trie&& rhs) noexcept {
        if(this != &rhs) {
            destroy(this->root_);
            this->root_ = std::exchange(rhs.root_, nullptr);
            this->count_ = std::exchange(rhs.count_, 0);
        }

        return *this;
    }

    void clear() noexcept {
        destroy();
    }

    void insert(std::string_view word) {
        
        auto current = this->root_;

        for(decltype(word.size()) i = 0; i < word.size(); ++i) {
            auto it = current->children.find(word[i]);
            if(it != current->children.end()) {
                current = it->second;
            }else{
                auto [it, _] = current->children.insert(std::make_pair(word[i], new node{false}));
                current = it->second;
            }
        }

        if(!current->terminating) {
            current->terminating = true;
            ++this->count_;
        }
    }

    [[nodiscard]] bool contains(std::string_view word) const noexcept {
        auto current = this->root_;

        for(decltype(word.size()) i = 0; i < word.size(); ++i) {
            auto it = current->children.find(word[i]);
            if(it == current->children.end()) {
                return false;
            }
            current = it->second;
        }

        return current->terminating;

    }

    [[nodiscard]] std::vector<std::string> starts_with(std::string_view word) const {
        std::vector<std::string> results;

        std::string current_word{word.data()};
        auto current = this->root_;

        for(decltype(word.size()) i = 0; i < word.size(); ++i) {
            auto it = current->children.find(word[i]);
            if(it == current->children.end()) {
                return results;
            }
            current = it->second;
        }

        starts_with_impl(results, current_word, current);

        return results;
    }

    [[nodiscard]] bool empty() const noexcept {
        return this->count_ == 0;
    }

    [[nodiscard]] size_type size() const noexcept {
        return this->count_;
    }

    ~trie() = default;

private:

    void copy(pointer_type n, const_pointer_type other_node) {
        if(!other_node) {
            return;
        }

        for(auto [k,v] : other_node->children) {
            auto t = new node{};
            t->terminating = v->terminating;

            auto [it, _] = n->children.insert(std::make_pair(k, t));
            copy(it->second, v);
        }

    }

    void starts_with_impl(std::vector<std::string>& words, std::string& word, const_pointer_type n) const {
        if(n->terminating) {
            words.push_back(word.data());
        }

        for(auto [k, v] : n->children) {
            word.push_back(k);
            starts_with_impl(words, word , v);
            word.pop_back();
        }

    }

};

int main(int argc, char** argv) {

    trie t1{};

    t1.insert("cat");
    t1.insert("catalogue");
    t1.insert("car");
    t1.insert("cartographer");
    t1.insert("cart");
    t1.insert("do");
    t1.insert("dont");
    t1.insert("done");
    t1.insert("frog");
    t1.insert("from");
    t1.insert("alice");
    t1.insert("at");

    auto t2 = t1;

    std::cout << "trie size: " << t2.size() << std::endl;
    std::cout << "trie contains alice: " << std::boolalpha << t2.contains("alice") << std::endl;
    std::cout << "trie contains fred: " << std::boolalpha << t2.contains("fred") << std::endl;
    std::cout << "trie contains ca: " << std::boolalpha << t2.contains("ca") << std::endl;
    std::cout << "trie contains cat: " << std::boolalpha << t2.contains("cat") << std::endl;

    auto sw1 = t2.starts_with("cat");

    std::cout << "words starting with cat\n ";

    for(auto const& e : sw1) {
        std::cout << e << std::endl;
    }

    return EXIT_SUCCESS;
}
