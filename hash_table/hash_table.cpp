#include <iostream>
#include <vector>
#include <string>

using namespace std;

template<class val_type>
class Double_linked_node
{
  public:

  Double_linked_node() : val_{}, prev_{ nullptr }, next_{ nullptr }
  {}

  Double_linked_node(val_type val) : val_{ val }, prev_{ nullptr }, next_{ nullptr }
  {}

  Double_linked_node(const Double_linked_node<val_type>&) = delete;
  Double_linked_node(Double_linked_node<val_type>&&) = delete;

  Double_linked_node& operator=(const Double_linked_node<val_type>&) = delete;
  Double_linked_node& operator=(Double_linked_node<val_type>&&) = delete;

  virtual ~Double_linked_node()
  {
    //cout << this << " double linked node (val = " << val_ << ", prev = " << prev_ << ", next = " << next_ << ") destr\n";

    val_ = {};
    next_ = { nullptr };
    prev_ = { nullptr };
  }

  val_type Get_val()
  {
    return val_;
  }

  Double_linked_node<val_type>* Get_prev()
  {
    return prev_;
  }

  Double_linked_node<val_type>* Get_next()
  {
    return next_;
  }

  template<class val_type> friend class Double_linked_list;

  private:

  val_type val_;

  Double_linked_node<val_type> *prev_;
  Double_linked_node<val_type> *next_;
};

template<class val_type>
class Double_linked_list
{
  public:

  Double_linked_list() : first_{ nullptr }, size_{}
  {}

  virtual ~Double_linked_list()
  {
    while (nullptr != first_)
    {
      void *tmp{ first_->next_ };
      first_->~Double_linked_node();
      first_ = { static_cast<Double_linked_node<val_type>*>(tmp) };
      size_--;
    }

    if (0 != size_)
    {
      cerr << "~Double_linked_list: all elements were destroyed, but size = " << size_ << " (" << __FILE__ << ", " << __LINE__ << ")\n";
    }

    cout << this << " double linked list destr\n";
  }

  Double_linked_node<val_type>* Get_first()
  {
    return first_;
  }

  bool Emplace_element(val_type val, Double_linked_node<val_type> *after, Double_linked_node<val_type> **result)
  {
    if (nullptr == result)
    {
      cerr << "Emplace_element: Result node wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { new Double_linked_node<val_type>(val) };

      *result = { first_ };
      size_++;
      return true;
    }

    // Add as a first node
    if (nullptr == after)
    {
      *result = { new Double_linked_node<val_type>(val) };

      if (nullptr == *result)
      {
        cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";
        return false;
      }

      Double_linked_node<val_type> *tmp{ first_ };

      first_ = { *result };

      (*result)->next_ = { tmp };
      size_++;

      return true;
    }

    if (false == Find_node(after))
    {
      cerr << "Emplace_element: Failed to find node " << after << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    *result = { new Double_linked_node<val_type>(val) };

    if (nullptr == *result)
    {
      cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    Double_linked_node<val_type> *tmp{ after->next_ };

    after->next_ = { *result };

    (*result)->next_ = { tmp };
    (*result)->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { *result };
    }
    size_++;

    return true;
  }

  bool Add_element(Double_linked_node<val_type> **el, Double_linked_node<val_type> *after)
  {
    if (nullptr == el || nullptr == *el)
    {
      cerr << "Add_element: Element to add wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    if (nullptr != after && false == Find_node(after))
    {
      cerr << "Add_element: Failed to find node " << after << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { *el };
      size_++;

      return true;
    }

    // Add as a first node
    if (nullptr == after)
    {
      Double_linked_node<val_type> *tmp{ first_ };

      first_ = { *el };

      (*el)->next_ = { tmp };
      size_++;

      return true;
    }

    Double_linked_node<val_type> *tmp{ after->next_ };

    after->next_ = { *el };

    (*el)->next_ = { tmp };
    (*el)->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { *el };
    }
    size_++;

    return true;
  }

  bool Delete_element(Double_linked_node<val_type> *el)
  {
    if (nullptr == el)
    {
      cerr << "Delete_element: Node to delete wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (false == Find_node(el))
    {
      cerr << "Delete_element: Failed to find node " << el << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (el == first_)
    {
      first_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return true;
    }

    Double_linked_node<val_type> *parent{ el->prev_ };

    if (nullptr == parent)
    {
      cerr << "Delete_element: node " << el << " isn't the first node, but doesn't have parents (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    if (nullptr == el->next_)
    {
      // No need to connect the el's child with a new parent

      parent->next_ = { nullptr };
      el->~Double_linked_node();
      size_--;

      return true;
    }
    else
    {
      // el's child will need a new parent -- el's parent

      parent->next_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return true;
    }

    // Unreacheable for now
    //cout << "Delete_element: Failed to delete node " << el << " from single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    //return false;
  }

  bool Find_value_first_occur(val_type val, Double_linked_node<val_type> **result)
  {
    if (nullptr == result || nullptr == *result)
    {
      cerr << "Find_value_first_occur: Result node wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (nullptr == first_)
    {
      cerr << "Find_value_first_occur: Single linked list " << this << "doesn't have any nodes (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    Double_linked_node<val_type> *curr{ first_ };

    while (nullptr != curr)
    {
      if (curr->val_ == val)
      {
        *result = { curr };
        return true;
      }

      curr = curr->next_;
    }

    *result = { nullptr };
    cout << "Find_value_first_occur: Failed to find node vith val " << val << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    return false;
  }

  bool Find_node(Double_linked_node<val_type> *node)
  {
    if (nullptr == node)
    {
      cerr << "Find_node: Node to find wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (nullptr == first_)
    {
      cerr << "Find_node: Single linked list " << this << "doesn't have any nodes (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    Double_linked_node<val_type> *curr{ first_ };

    while (nullptr != curr)
    {
      if (curr == node)
      {
        return true;
      }

      curr = { curr->next_ };
    }

    cout << "Find_node: Node " << node << " is not a part of linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    return false;
  }

  Double_linked_node<val_type>* operator[](size_t index) const
  {
    if (nullptr == first_)
    {
      cerr << "Double_linked_node operator[]: no elements (" << __FILE__ << ", " << __LINE__ << ")\n";
      return nullptr;
    }

    if (index >= size_)
    {
      cerr << "Double_linked_node operator[]: index " << index << " is out of bounds (" << __FILE__ << ", " << __LINE__ << ")\n";
      return nullptr;
    }

    Double_linked_node<val_type> *curr{ first_ };
    val_type ret{};

    for (size_t i{}; i < index; ++i)
    {
      if (nullptr != curr)
      {
        curr = { curr->next_ };
      }
      else
      {
        cerr << "Double_linked_node operator[]: Can't access element with index" << index << " (" << __FILE__ << ", " << __LINE__ << ")\n";
        return nullptr;
      }
    }

    return curr;
  }

  Double_linked_node<val_type>* Get_node_by_index(size_t index) const
  {
    return operator[](index);
  }

  size_t Get_size() const noexcept
  {
    return size_;
  }

  bool Move_after(size_t what, size_t after)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_after: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (after >= size_)
    {
      cerr << "Double_linked_node Move_after: after (" << after << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == after)
    {
      return true;
    }

    if (abs(what - after) == 1 && what > after)
    {
      return true;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };
    Double_linked_node<val_type> *after_ptr{ operator[](after) };

    return Move(what_ptr, after_ptr);
  }

  bool Move_before(size_t what, size_t before)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_before: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (before >= size_)
    {
      cerr << "Double_linked_node Move_before: before (" << before << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == before)
    {
      return true;
    }

    if (abs(what - before) == 1 && what < before)
    {
      return true;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };

    if (before > 0)
    {
      Double_linked_node<val_type> *after_ptr{ operator[](before - 1) };

      return Move(what_ptr, after_ptr);
    }
    else
    {
      return Move(what_ptr, nullptr);
    }
  }

  bool Swap_vals(size_t a, size_t b)
  {
    if (a >= size_)
    {
      cerr << "Double_linked_node Swap: a (" << a << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (b >= size_)
    {
      cerr << "Double_linked_node Swap: b (" << b << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (a == b)
    {
      return true;
    }

    Double_linked_node<val_type> *a_ptr{ operator[](a) };
    Double_linked_node<val_type> *b_ptr{ operator[](b) };

    return Swap_vals(a_ptr, b_ptr);
  }

  bool Swap_vals(Double_linked_node<val_type>* a, Double_linked_node<val_type>* b)
  {
    if (nullptr == a)
    {
      cerr << "Double_linked_node Swap: a (" << a << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (nullptr == b)
    {
      cerr << "Double_linked_node Swap: b (" << b << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (a == b)
    {
      return true;
    }

    if (false == Find_node(a))
    {
      cerr << "Double_linked_node Swap: node a (" << a << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (false == Find_node(b))
    {
      cerr << "Double_linked_node Swap: node b (" << b << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    val_type tmp{ a->val_ };
    a->val_ = { b->val_ };
    b->val_ = { tmp };

    return true;
  }

  private:

  bool Move(Double_linked_node<val_type>* what, Double_linked_node<val_type>* after)
  {
    if (nullptr == what)
    {
      cerr << "Double_linked_node Move: what (" << what << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (false == Find_node(what))
    {
      cerr << "Double_linked_node Move: node what (" << what << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == after)
    {
      return true;
    }

    Double_linked_node<val_type> *what_prev_old{ what->prev_ };
    Double_linked_node<val_type> *what_next_old{ what->next_ };
    Double_linked_node<val_type> *after_next_old{};

    if (nullptr != after)
    {
      after_next_old = { after->next_ };
      after->next_ = { nullptr };
    }

    what->prev_ = what->next_ = { nullptr };

    if (nullptr != what_prev_old)
    {
      what_prev_old->next_ = { what_next_old };
    }

    if (nullptr != what_next_old)
    {
      what_next_old->prev_ = { what_prev_old };
    }

    if (nullptr != after)
    {
      after->next_ = { what };
    }

    what->prev_ = { after };
    if (nullptr != after_next_old)
    {
      what->next_ = { after_next_old };
    }
    else if (nullptr == after)
    {
      what->next_ = { first_ };
    }

    if (nullptr != after_next_old)
    {
      after_next_old->prev_ = { what };
    }

    if (what == first_)
    {
      first_ = { what_next_old };
    }

    if (nullptr == after)
    {
      first_->prev_ = { what };
      first_ = { what };
    }

    return true;
  }

  size_t size_;
  Double_linked_node<val_type> *first_;
};

template<class val_type>
class Hash_table
{
  public:

  Hash_table()
  {}

  virtual ~Hash_table()
  {
    data_.clear();
    data_.shrink_to_fit();
  }

  bool Add_val(string key, val_type value)
  {
    uint64_t hash { Hash(key) };

    // Initialize.

    if (hash > data_.size())
    {
      for (size_t i { data_.size() }; i <= hash; ++i)
      {
        data_.push_back(nullptr);
      }
    }

    // Check for collisions.

    if (nullptr != data_[hash])
    {
      // This hash has already been used.

      string old_key = ((*data_[hash])[0])->Get_val().first;

      if (key == old_key)
      {
        cout << "Cannot add key " << key << " as it already exists in the table\n";

        return false;
      }

      cout << "A collision detected: hash = " << hash << ", old key = " << old_key << ", new_key = " << key << endl;

      size_t size { (*data_[hash]).Get_size() };

      node_type* last { ((*data_[hash])[size - 1]) };

      node_type* new_el { new node_type( {key, value} ) };

      if ((*data_[hash]).Add_element(&new_el, last))
      {
        cout << "New element with the same hash was successfully added\n";
      }
    }
    else
    {
      list_type* el{ new list_type };
      node_type* node{ new node_type({key, value}) };

      el->Add_element(&node, nullptr);

      data_[hash] = el;
    }

    return true;
  }

  bool Get_value(string& key, val_type& ret)
  {
    return Get_value(key.c_str(), ret);
  }

  bool Get_value(const char *key, val_type& ret)
  {
    uint64_t hash { Hash(string(key)) };

    if (hash >= data_.size())
    {
      cout << "Hash " << hash << " of the key " << key << " is out of bounds\n";

      return false;
    }

    list_type* list { data_[hash] };

    if (nullptr == list)
    {
      cout << "Failed to find an element with key = " << key << endl;

      return false;
    }

    size_t size{ (*data_[hash]).Get_size() };

    if (1 == size)
    {
      pair<string, val_type> tmp = ((*list)[0])->Get_val();

      ret = { tmp.second };
      return true;
    }
    else
    {
      cout << "Trying to acces the element with colliding hash = " << hash << " (key = " << key << ")\n";

      for (size_t i{}; i < size; ++i)
      {
        pair<string, val_type> tmp = ((*data_[hash])[i])->Get_val();
        
        if (tmp.first == string(key))
        {
          // Found the appropriate key.

          ret = { tmp.second };
          return true;
        }
      }

      cout << "Failed to find an element with key = " << key << endl;
      return false;
    }
  }

  void Print_val(string& key, bool print_nulls = false)
  {
    Print_val(key.c_str(), print_nulls);
  }

  void Print_val(const char *key, bool print_nulls = false)
  {
    val_type ret {};

    if (Get_value(key, ret))
    {
      cout << ret;
    }
    else if (print_nulls)
    {
      cout << "NULL ";
    }
  }

  void Print_all_values(bool print_nulls = false)
  {
    for (size_t i {}; i < data_.size(); ++i)
    {
      if (nullptr != data_[i])
      {
        if (1 == (data_[i])->Get_size())
        {
          cout << ((*data_[i])[0])->Get_val().second << ' ';
        }
        else
        {
          node_type *curr { (*data_[i])[0] };

          do
          {
            cout << curr->Get_val().second << ' ';

            curr = curr->Get_next();
          }
          while (curr != nullptr);
        }
      }
      else if (print_nulls)
      {
        cout << "NULL ";
      }
    }
  }

  bool Delete_key(string& key)
  {
    return Delete_key(key.c_str());
  }

  bool Delete_key(const char *key)
  {
    val_type ret {};

    if (false == Get_value(key, ret))
    {
      cout << "Failed to delete " << key << " as it is not a part of the table\n";

      return false;
    }

    uint64_t hash { Hash(key) };

    list_type *list { data_[hash] };

    if (nullptr == list)
    {
      cerr << "Element with key " << key << " is nullptr\n";

      return false;
    }

    if (1 == list->Get_size())
    {
      data_[hash] = nullptr;
    }
    else
    {
      for (size_t i {}; i < list->Get_size(); ++i)
      {
        node_type *node { ((*data_[hash])[i]) };

        if (node->Get_val().first == key)
        {
          list->Delete_element(node);

          return true;
        }
      }
    }
  }

  private:

  typedef Double_linked_list<pair<string, val_type>> list_type;
  typedef Double_linked_node<pair<string, val_type>> node_type;

  uint64_t Hash(string x)
  {
    uint64_t h {};
    uint64_t a { 31 };
    uint64_t p { 37 };

    for (size_t i {}; i < x.size(); ++i)
    {
      h = { ((h  *a) + x[i]) % p };
    }

    return h;
  }

  vector<list_type*> data_;
};

int main()
{
  Hash_table<uint32_t> ht;

  ht.Add_val("John", 1);
  ht.Add_val("Doe", 2);
  ht.Add_val("Mary", 3);
  ht.Add_val("Sue", 4);

  ht.Print_all_values();

  cout << endl;

  ht.Add_val("John", 1);
  ht.Add_val("Doe", 2);

  ht.Delete_key("Andy");
  ht.Delete_key("Mary");

  ht.Print_all_values();

  cout << endl;

  cin.get();

  return 0;
}
