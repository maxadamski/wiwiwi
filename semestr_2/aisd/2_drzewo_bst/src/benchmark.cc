#include "testkit.hh"

using namespace std;

typedef void (*Printer)(void*, std::string);

// BST:
// 
// remove(bst, obj):
// 	replace obj with max(obj.left) or min(obj.right)
// 
// mktree(bst):
// 		median
// 	   /      \
// 	median    median
// 	/    \    /    \
//    med  med  med  med
//    ...  ...  ...  ...
// 
// wzdluzny/pre-order(bst):
// 	root,left,right
// 
// poprzeczny/in-order(bst):
// 	left,root,right
// 
// wsteczy/post-order(bst):
// 	left,right,root

struct Student {
	// names: 2*8*12 bit * 10^7 = 240 MB
	// index: 32     bit * 10^7 = 40 MB
	long int index;
	char first_name[12];
	char last_name[12];

	Student(long int index = -1) :
		index(index) {};
};

void print_student(void *item, std::string terminator = "\n") {
	if (!item) {
		std::cout << "null";
	} else {
		Student *student = (Student*) item;
		std::cout << "{" << std::setfill('0') << std::setw(7)
			<< student->index << ", "
			<< student->first_name << ", " 
			<< student->last_name << "}" << terminator;
	}
}

auto read_names(std::string path, int rows) {
	std::vector<Student*> students;
	std::ifstream data(path);
	for (int i = 0; i < rows; i++) {
		std::string first_name, last_name, index;
		data >> first_name >> last_name >> index;
		Student *student = new Student;
		for (size_t i = 0; i < first_name.size(); i++)
			student->first_name[i] = first_name[i];
		for (size_t i = 0; i < last_name.size(); i++)
			student->last_name[i] = last_name[i];
		student->index = std::stoi(index);
		students.push_back(student);
	}
	return students;
}

///////////////////////////////////////////////////////////////////////////////
// COMPARATORS
///////////////////////////////////////////////////////////////////////////////

enum ComparatorResult { 
	EQ = 0, LT = -1, GT = 1
};

typedef ComparatorResult (*Comparator)(void*, void*);

ComparatorResult cmp_student(void *a, void *b) {
	Student *sa = (Student*) a, *sb = (Student*) b;
	if (sa->index < sb->index) return LT;
	if (sa->index > sb->index) return GT;
	return EQ;
}

ComparatorResult cmp_pointer(void *a, void *b) {
	if (a < b) return LT;
	if (a > b) return GT;
	return EQ;
}

ComparatorResult cmp_integer(void *ap, void *bp) {
	int a = *(int*) ap, b = *(int*) bp;
	if (a < b) return LT;
	if (a > b) return GT;
	return EQ;
}

///////////////////////////////////////////////////////////////////////////////
// GENERIC LIST
///////////////////////////////////////////////////////////////////////////////

struct ListNode {
	void *item;
	ListNode *next;

	ListNode(void *item = nullptr, ListNode *next = nullptr): 
		item(item), next(next) {}
};

// returns: pointer to the list head
ListNode *append(ListNode *node, void *item) {
	ListNode *head = node;
	while (node->next) node = node->next;
	node->next = new ListNode(item);
	return head;
}

// returns: pointer to the list head
ListNode *prepend(ListNode *node, void *item) {
	return new ListNode(item, node);
}

// note: does nothing if removing from a single element list
// returns: pointer to the removed element, null if not found
ListNode *remove(ListNode *node, void *item, Comparator cmp = cmp_pointer) {
	for (; node && node->next; node = node->next) {
		ListNode *next = node->next;
		if (cmp(item, next->item) == EQ) {
			node->next = next->next;
			return next;
		}
	}
	return nullptr;
}

// returns: pointer to the first matching element, null if not found
ListNode *find(ListNode *node, void *item, Comparator cmp = cmp_pointer) {
	for (; node; node = node->next) {
		if (cmp(item, node->item) == EQ) {
			return node;
		}
	}
	return nullptr;
}

void print(ListNode *node, Printer print) {
	std::cout << "[\n";
	for (; node; node = node->next) {
		std::cout << "  ";
		print(node->item, node->next ? ",\n" : "");
	}
	std::cout << "\n]\n";
}

///////////////////////////////////////////////////////////////////////////////
// STUDENT LIST
///////////////////////////////////////////////////////////////////////////////

ListNode *remove_student(ListNode *students, long int index) {
	Student *dummy = new Student;
	dummy->index = index;
	ListNode *removed = remove(students, dummy, cmp_student);
	delete dummy;
	return removed;
}

ListNode *find_student(ListNode *students, long int index) {
	Student *dummy = new Student;
	dummy->index = index;
	ListNode *found = find(students, dummy, cmp_student);
	delete dummy;
	return found;
}

///////////////////////////////////////////////////////////////////////////////
// GENERIC BST
///////////////////////////////////////////////////////////////////////////////

struct TreeNode {
	Student *item;
	TreeNode *left, *right;

	TreeNode(Student *item, TreeNode *left = nullptr, TreeNode *right = nullptr): 
		item(item), left(left), right(right) {}

	Student *min_value() {
		return left ? left->min_value() : item;
	}

	TreeNode *remove_deep(TreeNode *parent, Student *target) {
		if (target->index < item->index) {
			return left ? left->remove_deep(this, target) : nullptr;
		} else if (target->index > item->index) {
			return right ? right->remove_deep(this, target) : nullptr;
		} else {
			if (left && right) {
				item = right->min_value();
				return right->remove_deep(this, item);
			} else if (parent->left == this) {
				parent->left = left ? left : right;
				return this;
			} else if (parent->right == this) {
				parent->right = left ? left : right;
				return this;
			}
		}
	}

	bool remove(Student *target) {
		TreeNode *removed;
		if (item->index == target->index) {
			TreeNode root(0, this);
			removed = remove_deep(&root, target);
		} else {
			removed = remove_deep(nullptr, target);
		}

		if (removed) {
			//delete removed;
			return true;
		}
		return false;
	}

	void append(Student *target) {
		if (target->index > item->index) {
			if (!right)
				right = new TreeNode(target);
			else
				right->append(target);
		} else {
			if (!left)
				left = new TreeNode(target);
			else
				left->append(target);
		}
	}

	TreeNode *find(Student *target) {
		if (target->index > item->index)
			return right->find(target);
		else if (target->index < item->index)
			return left->find(target);
		else
			return this;
	}

	void print(int level = 0) {
		if (left) left->print(level + 1);
		if (item) cerr << " " << level << ":" << item->index << " ";
		if (right) right->print(level + 1);
	}
};


//int element_count(TreeNode *node) {
//	if (!node) return 0;
//	return 1 + element_count(node->left) + element_count(node->right);
//}
//
//int height(TreeNode *node) {
//	if (!node) return 0;
//	return 1 + max(height(node->left), height(node->right));
//}
//
//bool is_full(TreeNode *node) {
//	if (!node) return true;
//	return is_full(node->left) && is_full(node->right);
//}
//
//bool is_balanced(TreeNode *node) {
//	return abs(element_count(node->left) - element_count(node->right)) <= 1;
//}
//

///////////////////////////////////////////////////////////////////////////////
// STUDENT BST
///////////////////////////////////////////////////////////////////////////////

TreeNode *avl_from_students(std::vector<Student*> s) {
	int L = s.size();
	int M = L / 2;

	if (L == 1) {
		return new TreeNode(s[0]);
	} else if (L == 2) {
		return new TreeNode(s[1], new TreeNode(s[0]));
	} else {
		TreeNode *n = new TreeNode(s[M]);

		std::vector<Student*> s_left(s.begin(), s.begin() + M);
		n->left = avl_from_students(s_left);

		std::vector<Student*> s_right(s.begin() + M + 1, s.end());
		n->right = avl_from_students(s_right);

		return n;
	}
}

///////////////////////////////////////////////////////////////////////////////
// BENCHMARK
///////////////////////////////////////////////////////////////////////////////

void bench() {
	std::vector<int> ns;
	int max = 20000;
	for (int i = 1000; i <= max; i += 1000)
		ns.push_back(i);

	auto student_data = read_names("input/names.csv", max);

	cout << "n,list-append,list-find,list-remove,bst-rand-append,bst-rand-find,bst-rand-remove,bst-append,bst-find,bst-remove,bbst-append,bbst-find,bbst-remove\n"; // naglowek csv

	for (int n : ns) {
		cout << n << "," << flush;

		vector<int> indices;
		for (int i = 0; i < n; i++) indices.push_back(i);
		shuffle(indices);

		///////////////////////////////////////////////////////////////////////
		// Lista
		///////////////////////////////////////////////////////////////////////

		ListNode *node = new ListNode(student_data[0]);

		{
			// zapisywanie do listy
			cout << fixed << benchmark_simple([&node, n, student_data]{ 
				for (int i = 0; i < n; i++) {

					append(node, student_data[i]);

				}
			}) * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// wyszukiwanie z posortowanej listy
			long int sum = 0;
			for (int i : indices) {
				sum += benchmark_simple([&node, i, student_data]{ 
					find(node, student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// usuwanie z listy
			long int sum = 0;
			for (int i = n - 1; i >= 0; i--) {
				sum += benchmark_simple([&node, i, student_data]{ 
					remove(node, student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
			cerr << ".";
		}

		///////////////////////////////////////////////////////////////////////
		// BST losowe
		///////////////////////////////////////////////////////////////////////

		Student *dummy = new Student();
		TreeNode rbst(dummy);

		{
			// zapisywanie do losowego bst
			cout << fixed << benchmark_simple([&rbst, student_data, indices]{
				for (int i : indices) {
					rbst.append(student_data[i]);
				}
			}) * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// wyszukiwanie z losowego bst
			long int sum = 0;
			for (int i : indices) {
				sum += benchmark_simple([&rbst, i, student_data]{ 
					rbst.find(student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// usuwanie z losowego bst
			long int sum = 0;
			for (int i = n - 1; i >= 0; i--) {
				sum += benchmark_simple([&rbst, i, student_data]{ 
					rbst.remove(student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
			cerr << ".";
		}

		///////////////////////////////////////////////////////////////////////
		// BST posortowane
		///////////////////////////////////////////////////////////////////////

		TreeNode bst(dummy);

		{
			// zapisywanie
			cout << fixed << benchmark_simple([&bst, student_data, n]{
				for (int i = 0; i < n; i++) {
					bst.append(student_data[i]);
				}
			}) * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// wyszukiwanie
			long int sum = 0;
			for (int i : indices) {
				sum += benchmark_simple([&bst, i, student_data]{ 
					bst.find(student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// usuwanie
			long int sum = 0;
			for (int i = n - 1; i >= 0; i--) {
				sum += benchmark_simple([&bst, i, student_data]{ 
					bst.remove(student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
			cerr << ".";
		}


		///////////////////////////////////////////////////////////////////////
		// BBST
		///////////////////////////////////////////////////////////////////////
		
		TreeNode *bbst = nullptr;

		{
			// zapisywanie
			vector<Student*> students(student_data.begin(), student_data.begin() + n);
			cout << fixed << benchmark_simple([&bbst, students]{
				bbst = avl_from_students(students);
			}) * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// wyszukiwanie z bbst
			long int sum = 0;
			for (int i : indices) {
				sum += benchmark_simple([&bbst, i, student_data]{ 
					bbst->find(student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
			cerr << ".";
		}

		{
			// usuwanie z bbst
			long int sum = 0;
			for (int i = n - 1; i >= 0; i--) {
				sum += benchmark_simple([&bbst, i, student_data]{ 
					bbst->remove(student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "" << flush;
			cerr << ".";
		}

		cout << "\n";
		cerr << "\n";
	}

	for (auto student : student_data)
		delete student;
}

void test() {
	vector<Student*> students;
	for (int i = 0; i < 7; i++)
		students.push_back(new Student(i));
	TreeNode *t = avl_from_students(students);
	t->print(); cerr << "\n";
	t->remove(students[1]);
	t->print(); cerr << "\n";
}

void usage(bool abort = true) {
	cout << "usage: benchmark [--bench|--test]\n";
	if (abort) exit(1);
}

int main(int argc, char **argv) {
	random_seed();
	if (argc != 2)
		usage();
	if (!strcmp(argv[1], "--bench"))
		bench();
	if (!strcmp(argv[1], "--test"))
		test();
	return 0;
}

