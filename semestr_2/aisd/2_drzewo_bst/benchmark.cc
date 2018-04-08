#include "testkit.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <functional>
#include <vector>
#include <string>

typedef void (*Printer)(void*, std::string);

struct Student {
	// names: 2*8*12 bit * 10^7 = 240 MB
	// index: 32     bit * 10^7 = 40 MB
	long int index;
	char first_name[12];
	char last_name[12];
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
	void *item;
	TreeNode *left, *right;

	TreeNode(void *item = nullptr, TreeNode *left = nullptr, TreeNode *right = nullptr): 
		item(item), left(left), right(right) {}

	~TreeNode() {
		delete left;
		delete right;
	}
};

TreeNode *append(TreeNode *node, void *item, Comparator cmp) {
	if (!node)
		return new TreeNode(item);

	if (cmp(item, node->item) == LT)
		node->left = append(node->left, item, cmp);
	else
		node->right = append(node->right, item, cmp);

	return node;
}

void *min(TreeNode *node) {
	return node->left ? min(node->left) : node->item;
}

TreeNode *remove(TreeNode *node, TreeNode *parent, void *item, Comparator cmp) {
	ComparatorResult res = cmp(item, node->item);
	if (res == LT) {
		return node->left ? remove(node->left, node, item, cmp) : NULL;
	} else if (res == GT) {
		return node->right ? remove(node->right, node, item, cmp) : NULL;
	} else {
		if (node->left && node->right) {
			node->item = min(node->right);
			return remove(node->right, node, item, cmp);
		} else if (parent->left == node) {
			parent->left = node->left ? node->left : node->right;
			return node;
		} else if (parent->right == node) {
			parent->right = node->left ? node->left : node->right;
			return node;
		}
	}
}

TreeNode *find(TreeNode *node, void *item, Comparator cmp) {
	if (!node || (cmp(item, node->item) == EQ))
		return node;

	if (cmp(item, node->item) == LT)
		return find(node->left, item, cmp);
	else
		return find(node->right, item, cmp);
}

int max(int a, int b) {
	return a > b ? a : b;
}

int abs(int a) {
	return a < 0 ? -a : a;
}

int element_count(TreeNode *node) {
	if (!node) return 0;
	return 1 + element_count(node->left) + element_count(node->right);
}

int height(TreeNode *node) {
	if (!node) return 0;
	return 1 + max(height(node->left), height(node->right));
}

bool is_full(TreeNode *node) {
	if (!node) return true;
	return is_full(node->left) && is_full(node->right);
}

bool is_balanced(TreeNode *node) {
	return abs(element_count(node->left) - element_count(node->right)) <= 1;
}

///////////////////////////////////////////////////////////////////////////////
// STUDENT BST
///////////////////////////////////////////////////////////////////////////////

TreeNode *insert_students_avl(std::vector<Student*> s) {
	int L = s.size();
	int M = L / 2;

	if (L == 1) {
		return new TreeNode(s[0]);
	} else if (L == 2) {
		return new TreeNode(s[1], new TreeNode(s[0]));
	} else {
		TreeNode *n = new TreeNode(s[M]);

		std::vector<Student*> s_left(s.begin(), s.begin() + M);
		n->left = insert_students_avl(s_left);

		std::vector<Student*> s_right(s.begin() + M + 1, s.end());
		n->right = insert_students_avl(s_right);

		return n;
	}
}

///////////////////////////////////////////////////////////////////////////////
// BENCHMARK
///////////////////////////////////////////////////////////////////////////////

int main() {
	using namespace std;
	random_seed();

	std::vector<int> ns;
	int max = 4000;
	for (int i = 200; i <= max; i += 200)
		ns.push_back(i);

	auto student_data = read_names("data/names.csv", max);


	cout << "n,list-append,list-find,list-remove,bst-append,bst-find,bst-remove,bbst-append,bbst-find,bbst-remove\n"; // naglowek csv

	for (int n : ns) {
		cerr << ".";
		cout << n << "," << flush;

		vector<int> indices;
		for (int i = 0; i < n; i++) indices.push_back(i);

		// zapisywanie do listy
		//

		ListNode *node = new ListNode(student_data[0]);

		{
			cout << fixed << benchmark_simple([&node, n, student_data]{ 
				for (int i = 0; i < n; i++) {

					append(node, student_data[i]);

				}
			}) * 1e-9 << "," << flush;
		}

		cerr << "_";

		// wyszukiwanie z posortowanej listy
		//
		
		{
			long int sum = 0;
			for (int i : indices) {
				sum += benchmark_simple([&node, i, student_data]{ 

					find(node, student_data[i]);

				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
		}

		cerr << "_";

		// usuwanie z listy
		//

		{
			long int sum = 0;
			for (int i = n - 1; i >= 0; i--) {
				sum += benchmark_simple([&node, i, student_data]{ 

					remove(node, student_data[i]);

				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
		}

		cerr << "_";

		// zapisywanie do bst
		//
		
		TreeNode *bst = nullptr;

		{
			cout << fixed << benchmark_simple([&bst, student_data, n]{
				for (int i = 0; i < n; i++) {

					bst = append(bst, student_data[i], cmp_student);

				}
			}) * 1e-9 << "," << flush;
		}

		cerr << "_";

		// wyszukiwanie z bst
		//

		{
			long int sum = 0;
			for (int i : indices) {
				sum += benchmark_simple([&bst, i, student_data]{ 

					find(bst, student_data[i], cmp_student);

				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
		}

		cerr << "_";

		// usuwanie z bst
		//

		{
			long int sum = 0;
			for (int i = n - 1; i >= 0; i--) {
				sum += benchmark_simple([&bst, i, student_data]{ 

					remove(bst, new TreeNode(0, bst), student_data[i], cmp_student);

				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
		}

		cerr << "_";

		// zapisywanie do bbst
		//
		
		TreeNode *bbst;

		{
			vector<Student*> students(student_data.begin(), student_data.begin() + n);
			cout << fixed << benchmark_simple([&bbst, students]{

				bbst = insert_students_avl(students);

			}) * 1e-9 << "," << flush;
			
		}

		cerr << "_";

		// wyszukiwanie z bbst
		//

		{
			long int sum = 0;
			for (int i : indices) {
				sum += benchmark_simple([&bbst, i, student_data]{ 

					find(bbst, student_data[i], cmp_student);

				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
		}
		
		cerr << "_";

		// usuwanie z bbst
		//

		{
			long int sum = 0;
			for (int i = n - 1; i >= 0; i--) {
				sum += benchmark_simple([&bbst, i, student_data]{ 

					remove(bbst, new TreeNode(0, bbst), student_data[i], cmp_student);

				});
			}
			cout << fixed << sum * 1e-9 << "" << flush;
		}

		cout << "\n";
	}
	cerr << "\n";

	for (auto student : student_data)
		delete student;

	return 0;
}

