//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Ryan Ward
// Version     : 1.0
// Copyright   : Copyright � 2023 SNHU COCE
// Description : Project Two - ABCU using Binary Search Tree
//============================================================================

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// define a structure to hold course information
struct Course {
    string courseNumber; // unique identifier
    string courseTitle;
    vector<string> prerequisites;
    Course() {}
};

// function for displaying a single course w/ prerequisites
void displayCourseWithPrereq(Course course) {
    cout << course.courseNumber << ": " << course.courseTitle;
        if (course.prerequisites.size() > 0) {
            cout << " | Prerequisites: ";
            // loop through and display prerequisites
            int i;
            for (i = 0; i < course.prerequisites.size() - 1; ++i) {
                cout << course.prerequisites[i] << ", ";
            }
            cout << course.prerequisites[i];
        }
        cout << endl << endl;
    return;
}

// function for displaying just the course number and title
void displayCourseOnly(Course course) {
    cout << course.courseNumber << ": " << course.courseTitle << endl;
}

// Internal structure for tree node
struct Node {
    Course course;
    Node *left;
    Node *right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // initialize with a course
    Node(Course aCourse) :
            Node() {
        course = aCourse;
    }
};
void DeleteRootSubtree(Node* root);

// Internal structure for linked list node
struct LLNode {
    string courseNumber;
    LLNode* next;

    // default constructor
    LLNode() {
        next = nullptr;
    }
    // initialize with a course number
    LLNode(string aCourseNumber) :
            LLNode() {
        courseNumber = aCourseNumber;
    }
};


class LinkedList {
public:
    LLNode* head;
    LLNode* tail;
    LinkedList();
    virtual ~LinkedList();
    void addNode(LLNode* node);
};

// Default constructor
LinkedList::LinkedList() {
    head = nullptr;
    tail = nullptr;
}

//appends a new node to the linked list
void LinkedList::addNode(LLNode* node) {
    if (head == nullptr) {
        head = node;
        tail = node;
    }
    else {
        tail->next = node;
        tail = node;
    }
}

// Destructor - removes each node in linked list and sets head to null
LinkedList::~LinkedList() {
    LLNode* curNode = head;
    while (curNode != nullptr) {
        LLNode* nextNode = curNode->next;
        delete curNode;
        curNode = nextNode;
    }
    head = nullptr;
    
}

//============================================================================
// Binary Search Tree Class
//============================================================================

class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, Course course);
    void inOrder(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void Insert(Course course);
    Course Search(string courseNumber);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    // recurse from root deleting every node
    DeleteRootSubtree(root);

}

/**
* Delete nodes from the tree (used for destructor)
*/
void DeleteRootSubtree(Node* rootNode) {
    // recursively delete nodes on left and right tree, then delete root
    if (rootNode != nullptr) {
        if (rootNode->left != nullptr) {
            DeleteRootSubtree(rootNode->left);
        }
        if (rootNode->right != nullptr) {
            DeleteRootSubtree(rootNode->right);
        }
        delete rootNode;
    }
}

/**
 * Traverse the tree in order
 */
void BinarySearchTree::InOrder() {
    inOrder(root);
}


/**
 * Insert a course node into tree
 */
void BinarySearchTree::Insert(Course course) {
    // if root is null, create new node with course and set to root
    if (root == nullptr) {
        root = new Node(course);
    }
    // otherwise, recursively add node starting at root
    else addNode(root, course);
}

/**
 * Search for a course
 *
 * @param course Course to search for
 */
Course BinarySearchTree::Search(string courseNumber) {
    // create new node and set to root
    Node* curNode = root;
    // loop down tree until node is found or null node reached
    while (curNode != nullptr) {
        if (curNode->course.courseNumber.compare(courseNumber) == 0) {
            return curNode->course;
        }
        else if (curNode->course.courseNumber.compare(courseNumber) > 0) {
            curNode = curNode->left;
        }
        else curNode = curNode->right;
    }
    // if node not found, create and return empty course
    Course course;
    return course;
}

/**
 * Add a course to some node (recursive)
 *
 * @param node Current node in tree
 * @param course Course to be added
 */
void BinarySearchTree::addNode(Node* node, Course course) {
    // if the new node courseNumber is less than current node courseNumber
        // and there is no left child node
        // this node becomes the new left child node
    if (node->course.courseNumber.compare(course.courseNumber) > 0) {
        if (node->left == nullptr) {
            node->left = new Node(course);

        }
        // if left child present, recuse down left tree
        else addNode(node->left, course);
    }
    else {
        // if the new node courseNumber is greater than current node courseNumber
            // and there is no right child node
            // this node becomes the new right child node
        if (node->right == nullptr) {
            node->right = new Node(course);
        }
        // if right child present, recurse down right tree
        else addNode(node->right, course);
    }

}
/**
* Print the BST in order
*
* @param node Node to start printing from (root)
*/

void BinarySearchTree::inOrder(Node* node) {
    // if node is not null, recursively print out left tree, then print node, then recursively print out right tree
    if (node != nullptr) {
        inOrder(node->left);
        displayCourseOnly(node->course);
        inOrder(node->right);

    }

}

//============================================================================
// Method for loading courses into data structure
//============================================================================

 /**
  * Load a CSV file containing courses into a BST
  *
  * @param csvPath the path to the CSV file to load
  * @param bst the binary search tree created in main
  */

void loadCourses(string csvPath, BinarySearchTree* bst) {
    cout << endl << "Loading CSV file " << csvPath << endl;

    // open file for reading
    std::ifstream inputFile(csvPath);

    // if file does not open
    if (!inputFile.is_open()) {
        std::cout << "Unable to open file: " << csvPath << endl;
        return;
    }
    cout << "File opened successfully." << endl;

    // Set start of file marker
    streampos startOfFile = inputFile.tellg();

    cout << "Loading courses..." << endl;

    // INIT linked list for course list and other variables
    LinkedList* courses;
    courses = new LinkedList();
    string courseNumber;
    int courseCount = 0; // for tracking number of courses added

    // parse each line, adding course number to courses list
    while (!inputFile.eof()) {
        getline(inputFile, courseNumber, ',');
        // cout << courseNumber << endl; // testing - for verifying course read in
        LLNode* newNode = new LLNode(courseNumber);
        courses->addNode(newNode);
        string discard;
        getline(inputFile, discard); // discard remainder of line
    }

    // Set position to beginning of file
    inputFile.clear();
    inputFile.seekg(0, startOfFile);

    // loop to read rows of a CSV file
    while (!inputFile.eof()) {

        // Create a data structure and add to the collection of courses
        Course course;
        bool courseExists = true; // for checking prerequisistes
        getline(inputFile, course.courseNumber, ',');
        // if course number is blank, skip entry
        if (course.courseNumber.compare("") == 0) {
            continue;
        }
        getline(inputFile, course.courseTitle, ',');
        // Error if course title is missing
        if (course.courseTitle == "") {
            cout << "Error: " << course.courseNumber << " missing course title." << endl;
            inputFile.ignore();
            string discard;
            getline(inputFile, discard);
            continue;
        }
        else {
            // read in all prerequisites and parse them individually
            string allPrerequisites;
            getline(inputFile, allPrerequisites);
            stringstream ss(allPrerequisites);
            string prerequisite;
            vector<string> prerequisites;
            while (getline(ss, prerequisite, ',')) {
                // if prerequisite is not blank, add to vector
                if (prerequisite.compare("") != 0)
                course.prerequisites.push_back(prerequisite);
            }
            // check for prerequisite in course list
            if (course.prerequisites.size() > 0) {
                courseExists = false;
            }
            for (int i = 0; i < course.prerequisites.size(); ++i){
                LLNode* curNode = courses->head;
                while (curNode != nullptr) {
                    if (curNode->courseNumber.compare(course.prerequisites[i]) == 0) {
                        courseExists = true;
                    }
                    curNode = curNode->next;
                }
            }
        }
        // if all prerequisites exist, add course to course list
        // displayCourseOnly(course); // testing - for verifying courses added
        if (courseExists) {
            bst->Insert(course);
            ++courseCount;
        }
        else cout << "Error: prerequisite for " << course.courseNumber << " not found." << endl;
    }
    cout << courseCount << " courses added to course list." << endl;
    inputFile.close();
}

/**
 * The one and only main() method
 */
int main() {

    // Define a binary search tree to hold all courses
    BinarySearchTree* bst;
    bst = new BinarySearchTree();
    Course course;
    string courseKey;

    //Define user input for file path
    string filepath;

    // display main menu and get user input
    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Courses" << endl;
        cout << "  2. Display All Courses" << endl;
        cout << "  3. Find Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cout << endl;

        switch (choice) {

        case 1: // prompt for file path
            cout << "Please enter the full path and filename: ";
            cin >> filepath;
            // Complete the method call to load the courses
            loadCourses(filepath, bst);
            cout << endl;
            break;

        case 2: // print bst in order
            bst->InOrder();
            cout << endl;
            break;

        case 3:
            // allows user to search for a course by course number and print that course
            std::cout << "Please enter the course number: ";
            cin >> courseKey;
            course = bst->Search(courseKey);
            cout << endl;
            if (!course.courseNumber.empty()) {
                displayCourseWithPrereq(course);
            } else {
            	cout << "Course Number " << courseKey << " not found." << endl << endl;
            }
            break;

        case 9:
            // exit program
            cout << "Good bye." << endl;
            return 0;
            break;
        
        default:
            cout << "Please enter a valid choice." << endl << endl;
        }
    }

	return 0;
}
