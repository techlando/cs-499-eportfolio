#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <string> // atoi
#include <time.h>
#include <vector>
#include <sstream>

using namespace std;

const unsigned int DEFAULT_SIZE = 179;

// Convert a string to uppercase
string toUpperCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// Define a structure to hold course information
struct Course {
    string courseNumber;    // unique identifier
    string courseName;      // name of the course
    vector<string> preRequisites;   // list of prerequisites
    Course() = default;
};

//============================================================================
// Hash Table class definition
//============================================================================

class HashTable {

private:
    struct Node {
        Course course;
        unsigned int key;
        Node *next;

        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        Node(Course aCourse, unsigned int aKey) : course(aCourse), key(aKey) {
            next = nullptr;
        }
    };

    vector<Node> nodes;
    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAllAlphanumeric();
    Course Search(string courseNumber);
    vector<Course> GetAllCourses();  // Get all courses for sorting
};

HashTable::HashTable() {
    nodes.resize(tableSize);
}

HashTable::HashTable(unsigned int size) {
    this->tableSize = size;
    nodes.resize(size);
}

HashTable::~HashTable() {
    nodes.erase(nodes.begin());
}

unsigned int HashTable::hash(int key) {
    return key % tableSize;
}

void HashTable::Insert(Course course) {
    unsigned int key = hash(atoi(course.courseNumber.c_str()));
    Node* oldNode = &(nodes.at(key));

    if (oldNode->key == UINT_MAX) {
        oldNode->key = key;
        oldNode->course = course;
        oldNode->next = nullptr;
    } else {
        while (oldNode->next != nullptr) {
            oldNode = oldNode->next;
        }
        oldNode->next = new Node(course, key);
    }
}

vector<Course> HashTable::GetAllCourses() {
    vector<Course> allCourses;
    for (auto i = nodes.begin(); i != nodes.end(); ++i) {
        if (i->key != UINT_MAX) {
            allCourses.push_back(i->course);
            Node* node = i->next;
            while (node != nullptr) {
                allCourses.push_back(node->course);
                node = node->next;
            }
        }
    }
    return allCourses;
}

void HashTable::PrintAllAlphanumeric() {
    vector<Course> allCourses = GetAllCourses();

    if (allCourses.empty()) {
        cout << "No courses loaded." << endl;
        return;
    }

    sort(allCourses.begin(), allCourses.end(), [](const Course& a, const Course& b) {
        return a.courseNumber < b.courseNumber;
    });

    cout << "Here is a sample schedule:" << endl;
    for (const auto& course : allCourses) {
        cout << course.courseNumber << ": " << course.courseName << endl;
    }
}

Course HashTable::Search(string courseNumber) {
    Course course;
    unsigned int key = hash(atoi(courseNumber.c_str()));
    Node* node = &(nodes.at(key));

    while (node != nullptr) {
        if (node->course.courseNumber == courseNumber) {
            return node->course;
        }
        node = node->next;
    }

    return course;  // Return an empty course if not found
}

//============================================================================
// Static methods used for testing
//============================================================================

void displayCourse(Course course) {
    cout << course.courseNumber << ": " << course.courseName << endl;
    cout << "Prerequisites: ";
    
    if (course.preRequisites.empty()) {
        cout << "None" << endl;
    } else {
        for (size_t i = 0; i < course.preRequisites.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << course.preRequisites[i];
        }
        cout << endl;
    }
}

void loadCourses(const string& textFile, HashTable* hashTable) {
    cout << "Loading CSV file " << textFile << endl;
    ifstream file(textFile);
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream ss(line);
            Course course;
            string token;

            // Read course number
            getline(ss, course.courseNumber, ',');
            // Read course name
            getline(ss, course.courseName, ',');

            // Read prerequisites (multiple columns for prerequisites)
            while (getline(ss, token, ',')) {
                // If the token is not empty, add it as a prerequisite
                if (!token.empty()) {
                    course.preRequisites.push_back(token);
                }
            }

            // Insert the course into the hash table
            hashTable->Insert(course);
        }
        file.close();
        cout << "Courses loaded successfully!" << endl;
    } else {
        cerr << "Unable to open file " << textFile << endl;
    }
}

//============================================================================
// Main method with the menu options
//============================================================================

int main(int argc, char* argv[]) {
    string textFile = "CS_300_ABCU_Advising_Program_Input.csv";
    if (argc > 1) {
        textFile = argv[1];
    }

    HashTable* courseTable = new HashTable();
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << "\n1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit." << endl;
        cout << "\nWhat would you like to do? ";
        cin >> choice;

        switch (choice) {
            case 1:
                loadCourses(textFile, courseTable);
                break;

            case 2:
                courseTable->PrintAllAlphanumeric();
                break;

            case 3: {
                string courseKey;
                cout << "What course do you want to know about? ";
                cin >> courseKey;

                // Convert the courseKey to uppercase before searching
                courseKey = toUpperCase(courseKey);

                Course course = courseTable->Search(courseKey);
                if (!course.courseNumber.empty()) {
                    displayCourse(course);
                } else {
                    cout << "Course not found!" << endl;
                }
                break;
            }

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    delete courseTable;
    return 0;
}
