#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const unsigned int DEFAULT_SIZE = 179;

// Convert a string to uppercase
string toUpperCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// Remove extra spaces from the beginning and end of a string
string trim(string str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");

    if (first == string::npos || last == string::npos) {
        return "";
    }

    return str.substr(first, last - first + 1);
}

// Define a structure to hold course information
struct Course {
    string courseNumber;              // unique identifier
    string courseName;                // name of the course
    vector<string> preRequisites;     // list of prerequisites
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
        Node* next;

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

    unsigned int hash(string courseNumber);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();

    void Clear();
    bool Insert(Course course);
    void PrintAllAlphanumeric();
    Course Search(string courseNumber);
    vector<Course> GetAllCourses();
    void ValidatePrerequisites();
};

HashTable::HashTable() {
    nodes.resize(tableSize);
}

HashTable::HashTable(unsigned int size) {
    this->tableSize = size;
    nodes.resize(size);
}

HashTable::~HashTable() {
    Clear();
}

// Improved hash function that works with course numbers like CSCI100
unsigned int HashTable::hash(string courseNumber) {
    unsigned int hashValue = 0;

    for (char character : courseNumber) {
        hashValue = (hashValue * 31) + character;
    }

    return hashValue % tableSize;
}

// Clear all data from the hash table and free linked-list memory
void HashTable::Clear() {
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        Node* current = nodes.at(i).next;

        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }

        nodes.at(i).key = UINT_MAX;
        nodes.at(i).course = Course();
        nodes.at(i).next = nullptr;
    }
}

// Insert course into hash table and prevent duplicates
bool HashTable::Insert(Course course) {
    course.courseNumber = toUpperCase(trim(course.courseNumber));
    course.courseName = trim(course.courseName);

    if (course.courseNumber.empty() || course.courseName.empty()) {
        return false;
    }

    // Prevent duplicate course records
    Course existingCourse = Search(course.courseNumber);
    if (!existingCourse.courseNumber.empty()) {
        return false;
    }

    unsigned int key = hash(course.courseNumber);
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

    return true;
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
    courseNumber = toUpperCase(trim(courseNumber));

    if (courseNumber.empty()) {
        return course;
    }

    unsigned int key = hash(courseNumber);
    Node* node = &(nodes.at(key));

    while (node != nullptr) {
        if (node->course.courseNumber == courseNumber) {
            return node->course;
        }

        node = node->next;
    }

    return course;
}

// Check whether prerequisites listed in the CSV actually exist as courses
void HashTable::ValidatePrerequisites() {
    vector<Course> allCourses = GetAllCourses();
    bool missingPrerequisiteFound = false;

    for (const Course& course : allCourses) {
        for (const string& prerequisite : course.preRequisites) {
            Course prerequisiteCourse = Search(prerequisite);

            if (prerequisiteCourse.courseNumber.empty()) {
                cout << "Warning: " << course.courseNumber
                     << " lists missing prerequisite " << prerequisite << "." << endl;
                missingPrerequisiteFound = true;
            }
        }
    }

    if (!missingPrerequisiteFound) {
        cout << "Prerequisite validation complete. No missing prerequisites found." << endl;
    }
}

//============================================================================
// Static methods used for testing
//============================================================================

void displayCourse(Course course, HashTable* hashTable) {
    cout << course.courseNumber << ": " << course.courseName << endl;
    cout << "Prerequisites: ";

    if (course.preRequisites.empty()) {
        cout << "None" << endl;
    } else {
        cout << endl;

        for (const string& prerequisite : course.preRequisites) {
            Course prerequisiteCourse = hashTable->Search(prerequisite);

            if (!prerequisiteCourse.courseNumber.empty()) {
                cout << "  " << prerequisiteCourse.courseNumber
                     << ": " << prerequisiteCourse.courseName << endl;
            } else {
                cout << "  " << prerequisite << ": Course not found in loaded data" << endl;
            }
        }
    }
}

bool fileCanOpen(const string& textFile) {
    ifstream file(textFile);
    return file.good();
}

bool loadCourses(const string& textFile, HashTable* hashTable) {
    cout << "Loading CSV file " << textFile << endl;

    if (!fileCanOpen(textFile)) {
        cerr << "Unable to open file " << textFile << endl;
        return false;
    }

    ifstream file(textFile);
    string line;
    int lineNumber = 0;
    int loadedCount = 0;
    int skippedCount = 0;

    // Clear the table before loading so the same file does not duplicate records
    hashTable->Clear();

    while (getline(file, line)) {
        lineNumber++;

        if (trim(line).empty()) {
            continue;
        }

        stringstream ss(line);
        Course course;
        string token;

        // Read course number
        getline(ss, course.courseNumber, ',');
        course.courseNumber = toUpperCase(trim(course.courseNumber));

        // Read course name
        getline(ss, course.courseName, ',');
        course.courseName = trim(course.courseName);

        // Validate required fields
        if (course.courseNumber.empty() || course.courseName.empty()) {
            cout << "Skipping invalid row " << lineNumber
                 << ": course number or course name is missing." << endl;
            skippedCount++;
            continue;
        }

        // Read prerequisites
        while (getline(ss, token, ',')) {
            token = toUpperCase(trim(token));

            if (!token.empty()) {
                course.preRequisites.push_back(token);
            }
        }

        if (hashTable->Insert(course)) {
            loadedCount++;
        } else {
            cout << "Skipping duplicate or invalid course on row "
                 << lineNumber << ": " << course.courseNumber << endl;
            skippedCount++;
        }
    }

    file.close();

    cout << "Courses loaded successfully!" << endl;
    cout << "Loaded records: " << loadedCount << endl;

    if (skippedCount > 0) {
        cout << "Skipped records: " << skippedCount << endl;
    }

    hashTable->ValidatePrerequisites();

    return true;
}

int getMenuChoice() {
    string input;
    int choice = 0;

    cout << "\nWhat would you like to do? ";
    getline(cin, input);

    stringstream ss(input);

    if (!(ss >> choice)) {
        return -1;
    }

    return choice;
}

string getFilePathFromUser(const string& defaultFile) {
    string filePath;

    cout << "Enter the full path to the course file." << endl;
    cout << "Press Enter to use the default file: " << defaultFile << endl;
    cout << "File path: ";

    getline(cin, filePath);
    filePath = trim(filePath);

    if (filePath.empty()) {
        return defaultFile;
    }

    return filePath;
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

        choice = getMenuChoice();

        switch (choice) {
            case 1: {
                string selectedFile = getFilePathFromUser(textFile);
                loadCourses(selectedFile, courseTable);
                break;
            }

            case 2:
                courseTable->PrintAllAlphanumeric();
                break;

            case 3: {
                string courseKey;

                cout << "What course do you want to know about? ";
                getline(cin, courseKey);

                courseKey = toUpperCase(trim(courseKey));

                Course course = courseTable->Search(courseKey);

                if (!course.courseNumber.empty()) {
                    displayCourse(course, courseTable);
                } else {
                    cout << "Course not found!" << endl;
                }

                break;
            }

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << "That is not a valid option." << endl;
                break;
        }
    }

    delete courseTable;
    return 0;
}