#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <utility>

using namespace std;

struct Student;

struct Table{
    int id;
    vector<Student> studentsSeated;
    pair<int, int> Coordinates;
    int capacity;
    vector<Student> waitingQueue;
    string type;
};

struct Student{
    int id;
    string name;
    int friendID;
    int enemyID;
    Table* seatedTable = nullptr;
    Table* waitingTable = nullptr;
};

vector<vector<string>> loadCSVData(const string& filename);
void loadTablesData(const vector<vector<string>>& tablesData, unordered_map<int, Table>& tables); 
void loadStudentsData(const vector<vector<string>>& studentsData, unordered_map<int, Student>& students);
void displayStudentsAtTable(const Table& table);
void displayTableRemainingCapacity(const Table& table);
void displayTableWaitingQueue(const Table& table);
void displayTableDetails(const int& tableID, unordered_map<int, Table>& tables);
int calculateDistance(int studentID, const pair<int, int>& tableCoordinates, unordered_map<int, Student>& students);
int calculateScore(const Table& table, unordered_map<int, Student>& students, int studentID);
vector<pair<int, int>> calculateTableScores(int studentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void sortTablesByScore(vector<pair<int, int>>& tableScores);
void displayRankedTables(const vector<pair<int, int>>& tableScores, unordered_map<int, Table>& tables);
void processStudentEntry(int studentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void reserveTable(int studentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables, int tableID = -1);
void removeStudentFromTable(Student& student, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void processStudentExit(int StudentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
Student* findStudent(int id, vector<Student>& students);
void switchSeatsBetweenStudents(Student& student, Student& friendOfStudent,unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void switchSeats(int StudentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void handleShowTableInfo(istringstream& iss, unordered_map<int, Table>& tables);
void handleEnterCommand(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void handleReserveTable(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void handleExitCommand(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void handleSwitchSeats(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);
void processCommand(const string& input, unordered_map<int, Student>& students, unordered_map<int, Table>& tables);

int main(int argc, char* argv[]) {
    unordered_map<int, Table> tables;
    unordered_map<int, Student> students;

    vector<vector<string>> tablesData = loadCSVData(argv[1]);
    vector<vector<string>> studentsData = loadCSVData(argv[2]);

    loadTablesData(tablesData, tables);
    loadStudentsData(studentsData, students);

    string input;
    while (getline(cin, input)) {
        processCommand(input, students, tables);
    }
    return 0;
}

void handleShowTableInfo(istringstream& iss, unordered_map<int, Table>& tables){
    int tableID;
    iss >> tableID;
    displayTableDetails(tableID, tables);
}

void handleEnterCommand(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    int studentID;
    iss >> studentID;
    processStudentEntry(studentID, students, tables);
}

void handleReserveTable(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    int studentID, tableID = -1;
    iss >> studentID;
    if (iss >> tableID) {
        reserveTable(studentID, students, tables, tableID);
    } else {
        reserveTable(studentID, students, tables);
    }
}

void handleExitCommand(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    int studentID;
    iss >> studentID;
    processStudentExit(studentID, students, tables);
}

void handleSwitchSeats(istringstream& iss, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    int studentID;
    iss >> studentID;
    switchSeats(studentID, students, tables);
}

void processCommand(const string& input, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    istringstream iss(input);
    string command;
    iss >> command;

    if(command == "show_table_info"){
        handleShowTableInfo(iss, tables);
    }else if(command == "enter"){
        handleEnterCommand(iss, students, tables);
    }else if(command == "reserve_table"){
        handleReserveTable(iss, students, tables);
    }else if(command == "exit"){
        handleExitCommand(iss, students, tables);
    }else if(command == "switch"){
        handleSwitchSeats(iss, students, tables);
    }
}

vector<vector<string>> loadCSVData(const string& filename){
    ifstream file(filename);
    vector<vector<string>> data;
    string line, value;

    getline(file, line);
    while(getline(file, line)){
        vector<string> row;
        istringstream iss(line);
        while(getline(iss, value, ',')){
            row.push_back(value);
        }
        data.push_back(row);
    }

    file.close();
    return data;
}

void loadTablesData(const vector<vector<string>>& tablesData, unordered_map<int, Table>& tables){
    for (const auto& row : tablesData) {
        Table table;
        table.id = stoi(row[0]);
        table.Coordinates = {stoi(row[1]), stoi(row[2])};
        table.capacity = stoi(row[3]);
        table.type = row[4];
        tables[table.id] = table;
    }
}

void loadStudentsData(const vector<vector<string>>& studentsData, unordered_map<int, Student>& students){
    for (const auto& row : studentsData){
        Student student;
        student.id = stoi(row[0]);
        student.name = row[1];
        student.friendID = stoi(row[2]);
        student.enemyID = stoi(row[3]);
        student.seatedTable = nullptr;
        student.waitingTable = nullptr;
        students[student.id] = student;
    }
}

void displayStudentsAtTable(const Table& table){
    cout << "People at the table: ";
    vector<Student> sortedStudents = table.studentsSeated;
    sort(sortedStudents.begin(), sortedStudents.end(), [](const Student& a, const Student& b) {
        return a.name < b.name;
    });

    for (size_t i = 0; i < sortedStudents.size(); ++i) {
        cout << sortedStudents[i].name;
        if (i != sortedStudents.size() - 1) {
            cout << ", ";
        }
    }
    cout << endl;
}

void displayTableRemainingCapacity(const Table& table){
    int remainingCapacity = table.capacity - table.studentsSeated.size();
    cout << "Table remaining capacity: " << remainingCapacity << endl;
}

void displayTableWaitingQueue(const Table& table){
    int waitingQueueLength = table.waitingQueue.size();
    cout << "Waiting queue length: " << waitingQueueLength << endl;
}

void displayTableDetails(const int& tableID, unordered_map<int, Table>& tables){
    auto tableIt = tables.find(tableID);
    Table& table = tableIt->second;
    
    cout << "Table ID: " << tableID << endl;
    displayStudentsAtTable(table);
    displayTableRemainingCapacity(table);
    displayTableWaitingQueue(table);
}

int calculateDistance(int studentID, const pair<int, int>& tableCoordinates, unordered_map<int, Student>& students){
    auto studentIt = students.find(studentID);
    Student& student = studentIt->second;
    if(student.seatedTable == nullptr){
        return 0;
    }

    if(student.seatedTable->Coordinates == tableCoordinates){
        return 0; 
    }

    int x1 = student.seatedTable->Coordinates.first;
    int y1 = student.seatedTable->Coordinates.second;
    int x2 = tableCoordinates.first;
    int y2 = tableCoordinates.second;
    return  abs(x1 - x2) + abs(y1 - y2);
}

int calculateScore(const Table& table, unordered_map<int, Student>& students, int studentID ){
    auto studentIt = students.find(studentID);
    Student& student = studentIt->second;
    const pair<int, int> tableCoordinates {table.Coordinates.first,table.Coordinates.second};
    int friendID = student.friendID;
    int enemyID = student.enemyID;
    int distanceToFriend = calculateDistance(friendID, tableCoordinates, students);
    int distanceToEnemy = calculateDistance(enemyID, tableCoordinates, students);
    int typeBonus;
    if (table.type =="window"){
        typeBonus = 6;
    }else if(table.type =="door"){
        typeBonus = 4;
    }else{
        typeBonus = 2;
    }

    return distanceToEnemy - distanceToFriend + typeBonus;
}

vector<pair<int, int>> calculateTableScores(int studentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    vector<pair<int, int>> tableScores; 
    for (const auto& tableEntry : tables) {
        int tableID = tableEntry.first;
        const Table& table = tableEntry.second;
        int score = calculateScore(table, students, studentID);
        tableScores.push_back({tableID, score});
    }
    return tableScores;
}

void sortTablesByScore(vector<pair<int, int>>& tableScores){
    sort(tableScores.begin(), tableScores.end(), [&](const pair<int, int>& tableA, const pair<int, int>& tableB) {
        if (tableA.second != tableB.second) {
            return tableA.second > tableB.second; 
        }
        return tableA.first < tableB.first; 
    });
}

void displayRankedTables(const vector<pair<int, int>>& tableScores, unordered_map<int, Table>& tables){
    for(const auto& entry : tableScores){
        int tableID = entry.first;
        const Table& table = tables.at(tableID);

        int remainingCapacity = table.capacity - table.studentsSeated.size();
        int waitingQueueLength = table.waitingQueue.size();

        cout << "Table " << tableID << ": " 
             << remainingCapacity << " " 
             << waitingQueueLength << endl;
    }
}

void processStudentEntry(int studentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
vector<pair<int, int>> tableScores = calculateTableScores(studentID, students, tables);
sortTablesByScore(tableScores);
displayRankedTables(tableScores, tables);
}

void reserveTable(int studentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables, int tableID){
    auto studentIt = students.find(studentID);
    Student& student = studentIt->second;
    if (tableID == -1) {
        vector<pair<int, int>> tableScores = calculateTableScores(studentID, students, tables);
        sortTablesByScore(tableScores);
        tableID = tableScores[0].first;
    }

    auto tableIt = tables.find(tableID);

    Table& table = tableIt->second;
    
    if(table.capacity == table.studentsSeated.size()){
        table.waitingQueue.push_back(student);
        student.waitingTable = &table;
        cout << student.name << " enters the waiting queue of table " << tableID <<endl;           
    }else{
        table.studentsSeated.push_back(student);
        student.seatedTable = &table;
        cout << student.name << " sits at table " << tableID <<endl;           
    }
}

void removeStudentFromTable(Student& student, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    auto& peopleAtTheTable = student.seatedTable->studentsSeated;
    peopleAtTheTable.erase(remove_if(peopleAtTheTable.begin(), peopleAtTheTable.end(),
    [&](const Student& s) { return s.id == student.id; }), 
    peopleAtTheTable.end());
}

void processStudentExit(int StudentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables) {
    auto studentIt = students.find(StudentID);
    if (studentIt == students.end()) {
        cout << "Student not found!" << endl;
        return;
    }
    Student& exitingStudent = studentIt->second;
    Table* pTable = exitingStudent.seatedTable;

    cout << exitingStudent.name << " exits!" << endl;

    if (exitingStudent.seatedTable != nullptr) {
        removeStudentFromTable(exitingStudent, students, tables);
    }
    
    if (pTable != nullptr && !pTable->waitingQueue.empty()) {
        int replacementIndex = -1;
        for (int i = 0; i < pTable->waitingQueue.size(); i++) {
            if (pTable->waitingQueue[i].id == exitingStudent.friendID) {
                replacementIndex = i;
                break;
            }
        }
        if (replacementIndex == -1) {
            replacementIndex = 0;
        }
        
        int replacementID = pTable->waitingQueue[replacementIndex].id;
        pTable->waitingQueue.erase(pTable->waitingQueue.begin() + replacementIndex);
        
        Student& replacement = students[replacementID];
        replacement.seatedTable = pTable;
        replacement.waitingTable = nullptr;

        pTable->studentsSeated.push_back(replacement);
    }
    exitingStudent.seatedTable = nullptr;
    exitingStudent.waitingTable = nullptr;
}

Student* findStudent(int id, vector<Student>& students){
    auto it = find_if(students.begin(), students.end(), 
                    [id](const Student& student) {
                    return student.id == id;
                    });
    if (it != students.end()) {
        return &(*it);
    }
    return nullptr;
}

void switchSeatsBetweenStudents(Student& student, Student& friendOfStudent,unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    Table* studentTable = student.seatedTable;
    Table* friendTable = friendOfStudent.seatedTable;
    
    studentTable->studentsSeated.push_back(friendOfStudent);
    friendTable->studentsSeated.push_back(student);

    student.seatedTable = friendTable;
    friendOfStudent.seatedTable = studentTable;
}

void switchSeats(int StudentID, unordered_map<int, Student>& students, unordered_map<int, Table>& tables){
    auto studentIt = students.find(StudentID);

    Student& student = studentIt->second;

    if (student.seatedTable == nullptr) {
        cout << student.name << " is not seated at any table!" << endl;
        return;
    }
    auto friendIt = students.find(student.friendID);

    Student& friendOfStudent = friendIt->second;

    if (friendOfStudent.seatedTable == nullptr) {
        cout << friendOfStudent.name << " is not seated at any table!" << endl;
        return;
    }

    removeStudentFromTable(student, students, tables);
    removeStudentFromTable(friendOfStudent, students, tables);
    switchSeatsBetweenStudents(student, friendOfStudent, students, tables);
    cout << student.name <<" switches seats with " << friendOfStudent.name << "!" <<endl;
}
