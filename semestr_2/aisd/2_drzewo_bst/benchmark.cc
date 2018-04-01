#include "testkit.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <functional>
#include <vector>
#include <string>

typedef void (*Printer)(void*, std::string);

typedef struct Student Student;
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
	using namespace std;
	vector<Student*> students;
	ifstream data(path);
	for (int i = 0; i < rows; i++) {
		string line; 
		getline(data, line);
		stringstream stream(line);
		string first_name, last_name, index;
		getline(stream, first_name, ',');
		getline(stream, last_name, ',');
		getline(stream, index, ',');
		Student *student = new Student;
		for (size_t i = 0; i < first_name.size(); i++)
			student->first_name[i] = first_name[i];
		for (size_t i = 0; i < last_name.size(); i++)
			student->last_name[i] = last_name[i];
		student->index = stoi(index);
		students.push_back(student);
	}
	return students;
}

///////////////////////////////////////////////////////////////////////////////
// COMPARATORS
///////////////////////////////////////////////////////////////////////////////

enum ComparatorResult { 
	EQ = 0, LT, GT 
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

typedef struct ListNode ListNode;
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

typedef struct TreeNode TreeNode;
struct TreeNode {
	void *item;
	TreeNode *left, *right;

	TreeNode(void *item = nullptr, TreeNode *left = nullptr, TreeNode *right = nullptr): 
		item(item), left(left), right(right) {}
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

TreeNode *append_avl(TreeNode *node, void *item, Comparator cmp) {
}

TreeNode *remove(TreeNode *node, void *item, Comparator cmp) {
}

TreeNode *remove_avl(TreeNode *node, void *item, Comparator cmp) {
}

TreeNode *find(TreeNode *node, void *item, Comparator cmp) {
	if (!node || (cmp(item, node->item) == EQ))
		return node;

	if (cmp(item, node->item) == LT)
		return find(node->left, item, cmp);
	else
		return find(node->right, item, cmp);
}

///////////////////////////////////////////////////////////////////////////////
// STUDENT BST
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BENCHMARK
///////////////////////////////////////////////////////////////////////////////

int main() {
	using namespace std;
	random_seed();

	//auto students = new ListNode(student_data[0]);

	//append(students, student_data[1]);
	//append(students, student_data[0]);
	//append(students, student_data[2]);
	//append(students, student_data[99]);
	//students = prepend(students, student_data[9]);
	//students = prepend(students, student_data[8]);

	//cout << "students:\n";
	//print(students, print_student);

	//cout << "find 0:\n";
	//if (auto node = find_student(students, 0))
	//	print_student(node->item);

	//cout << "remove 2:\n";
	//if (auto node = remove_student(students, 2))
	//	print_student(node->item);

	//cout << "students:\n";
	//print(students, print_student);

	//return 0;

	int n_min = 1e4, n_max = 1e5, n_step = n_min;
	auto student_data = read_names("data/names.csv", n_max);

	cout << "n,list-append,list-find,list-remove\n"; // naglowek csv

	for (int n = n_min; n <= n_max; n += n_step) {
		cerr << 100 * n / n_max << "% ";
		cout << n << "," << flush;

		ListNode *node = new ListNode(student_data[0]);

		// zapisywanie do listy

		{
			cout << fixed << benchmark_simple([&node, n, student_data]{ 
				for (int i = 1; i < n; i++) {
					append(node, student_data[i]);
				}
			}) * 1e-9 << "," << flush;
		}

		// wyszukiwanie z posortowanej listy
		
		{
			long int sum = 0;
			for (int i = 0; i < n; i++) {
				sum += benchmark_simple([&node, n, student_data]{ 
					find(node, student_data[random(0, n-1)]);
				});
			}
			cout << fixed << sum * 1e-6 / n << "," << flush;
		}

		// usuwanie z listy

		{
			long int sum = 0;
			for (int i = n; i > 0; i--) {
				sum += benchmark_simple([&node, i, student_data]{ 
					remove(node, student_data[i]);
				});
			}
			cout << fixed << sum * 1e-9 << "," << flush;
		}

		// zapisywanie do bst
		// usuwanie z bst
		// wyszukiwanie z bst
		// wyszukiwanie z bbst
		
		//auto elapsed = benchmark(1, false, before, measure, after);
		//cout << "," << fixed << elapsed * 1e-9 << flush;
		cout << "\n";
	}
	cerr << "\n";
	cin.ignore();

	for (auto student : student_data)
		delete student;

	return 0;
}

