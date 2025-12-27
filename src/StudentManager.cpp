#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class StudentManager {
private:
    AVLTree<Student> studentTree;
    vector<Course> courses;
    UndoManager undoManager;
    
 
    string studentsFilePath;
    string coursesFilePath;
    
 
    void loadCoursesFromCSV();
    void loadStudentsFromCSV();
    void saveStudentsToCSV();
    
public:
    StudentManager(const string& studentsFile, const string& coursesFile);
    ~StudentManager();
    
 
    enum class UserType { ADMIN, STUDENT, INVALID };
    UserType authenticate(const string& id, const string& password, Student** outStudent = nullptr);
    
   
    bool addStudent(const Student& student, bool useUndo = true);
    bool updateStudent(const string& id, const Student& newData, bool useUndo = true);
    bool deleteStudent(const string& id, bool useUndo = true);
    Student* getStudent(const string& id);
    vector<Student> getAllStudents();
    
   
    vector<Course> getCoursesForStudent(const string& department, int yearOfStudy);
    const vector<Course>& getAllCourses() const { return courses; }
    
   
    bool canUndo() const { return undoManager.canUndo(); }
    void undo() { undoManager.undo(); }
    string getLastOperation() const { return undoManager.getLastCommandDescription(); }
    
  
    void save();
    
   
    friend class AddStudentCommand;
    friend class UpdateStudentCommand;
    friend class DeleteStudentCommand;
};


StudentManager::StudentManager(const string& studentsFile, const string& coursesFile)
    : studentsFilePath(studentsFile), coursesFilePath(coursesFile) {
    loadCoursesFromCSV();
    loadStudentsFromCSV();
}

StudentManager::~StudentManager() {
    save();
}

void StudentManager::loadCoursesFromCSV() {
    ifstream file(coursesFilePath);
    
    if (!file.is_open()) {
        cerr << "Warning: Could not open courses file: " << coursesFilePath << endl;
        return;
    }
    
    string line;
  
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        Course course = Course::fromCSV(line);
        if (!course.getCourseCode().empty()) {
            courses.push_back(course);
        }
    }
    
    file.close();
    cout << "Loaded " << courses.size() << " courses from database." << endl;
}

void StudentManager::loadStudentsFromCSV() {
    ifstream file(studentsFilePath);
    
    if (!file.is_open()) {
        cerr << "Warning: Could not open students file: " << studentsFilePath << endl;
        return;
    }
    
    string line;
    
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        Student student = Student::fromCSV(line);
        if (!student.getId().empty()) {
            studentTree.insert(student);
        }
    }
    
    cout << "Loaded " << studentTree.size() << " students from database." << endl;
}

void StudentManager::saveStudentsToCSV() {
    ofstream file(studentsFilePath);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open students file for writing: " << studentsFilePath << endl;
        return;
    }
    
  
    file << "id,password_hash,first_name,last_name,department,age,sex,year_of_study,section,course_results" << endl;
    
   
    vector<Student> allStudents = studentTree.getAll();
    for (const Student& student : allStudents) {
        file << student.toCSV() << endl;
    }
    
    file.close();
    cout << "Saved " << allStudents.size() << " students to database." << endl;
}

StudentManager::UserType StudentManager::authenticate(const string& id, const string& password,
                                                      Student** outStudent) {
   
    if (id == "admin" && password == "admin123") {
        return UserType::ADMIN;
    }
    
  
    Student* student = getStudent(id);
    if (student && student->verifyPassword(password)) {
        if (outStudent) {
            *outStudent = student;
        }
        return UserType::STUDENT;
    }
    
    return UserType::INVALID;
}

bool StudentManager::addStudent(const Student& student, bool useUndo) {

    if (studentTree.search(student)) {
        return false;
    }
    
    if (useUndo) {
        auto command = make_unique<AddStudentCommand>(this, student);
        undoManager.executeCommand(move(command));
    } else {
        studentTree.insert(student);
    }
    
    return true;
}

bool StudentManager::updateStudent(const string& id, const Student& newData, bool useUndo) {
    Student searchStudent;
    searchStudent.setId(id);
    
    Student* existing = studentTree.find(searchStudent);
    if (!existing) {
        return false;
    }
    
    if (useUndo) {
        Student oldData = *existing;
        auto command = make_unique<UpdateStudentCommand>(this, oldData, newData);
        undoManager.executeCommand(move(command));
    } else {
        studentTree.remove(*existing);
        studentTree.insert(newData);
    }
    
    return true;
}

bool StudentManager::deleteStudent(const string& id, bool useUndo) {
    Student searchStudent;
    searchStudent.setId(id);
    
    Student* existing = studentTree.find(searchStudent);
    if (!existing) {
        return false;
    }
    
    if (useUndo) {
        Student studentCopy = *existing;
        auto command = make_unique<DeleteStudentCommand>(this, studentCopy);
        undoManager.executeCommand(move(command));
    } else {
        studentTree.remove(*existing);
    }
    
    return true;
}

Student* StudentManager::getStudent(const string& id) {
    Student searchStudent;
    searchStudent.setId(id);
    return studentTree.find(searchStudent);
}

vector<Student> StudentManager::getAllStudents() {
    return studentTree.getAll();
}

vector<Course> StudentManager::getCoursesForStudent(const string& department, int yearOfStudy) {
    vector<Course> result;
    
    for (const Course& course : courses) {
        if (course.getDepartment() == department && course.getYearOfStudy() == yearOfStudy) {
            result.push_back(course);
        }
    }
    
    return result;
}

void StudentManager::save() {
    saveStudentsToCSV();
}


AddStudentCommand::AddStudentCommand(StudentManager* mgr, const Student& s)
    : manager(mgr), student(s), executed(false) {}

void AddStudentCommand::execute() {
    if (!executed) {
        manager->addStudent(student, false);  // Don't use undo for undo commands
        executed = true;
    }
}

void AddStudentCommand::undo() {
    if (executed) {
        manager->deleteStudent(student.getId(), false);
        executed = false;
    }
}

string AddStudentCommand::getDescription() const {
    return "Add student: " + student.getId() + " (" + student.getFullName() + ")";
}


UpdateStudentCommand::UpdateStudentCommand(StudentManager* mgr, const Student& oldS, const Student& newS)
    : manager(mgr), oldStudent(oldS), newStudent(newS), executed(false) {}

void UpdateStudentCommand::execute() {
    if (!executed) {
        manager->updateStudent(newStudent.getId(), newStudent, false);
        executed = true;
    }
}

void UpdateStudentCommand::undo() {
    if (executed) {
        manager->updateStudent(oldStudent.getId(), oldStudent, false);
        executed = false;
    }
}

string UpdateStudentCommand::getDescription() const {
    return "Update student: " + newStudent.getId() + " (" + newStudent.getFullName() + ")";
}

DeleteStudentCommand::DeleteStudentCommand(StudentManager* mgr, const Student& s)
    : manager(mgr), student(s), executed(false) {}

void DeleteStudentCommand::execute() {
    if (!executed) {
        manager->deleteStudent(student.getId(), false);
        executed = true;
    }
}

void DeleteStudentCommand::undo() {
    if (executed) {
        manager->addStudent(student, false);
        executed = false;
    }
}

string DeleteStudentCommand::getDescription() const {
    return "Delete student: " + student.getId() + " (" + student.getFullName() + ")";
}

void UndoManager::executeCommand(unique_ptr<Command> command) {
    command->execute();
    
    undoStack.push(move(command));
    

    if (undoStack.size() > MAX_UNDO_SIZE) {

        stack<unique_ptr<Command>> tempStack;
        
        while (undoStack.size() > 1) {
            tempStack.push(move(undoStack.top()));
            undoStack.pop();
        }
        
        undoStack.pop();  
        
        while (!tempStack.empty()) {
            undoStack.push(move(tempStack.top()));
            tempStack.pop();
        }
    }
}

bool UndoManager::canUndo() const {
    return !undoStack.empty();
}

void UndoManager::undo() {
    if (canUndo()) {
        undoStack.top()->undo();
        undoStack.pop();
    }
}

void UndoManager::clear() {
    while (!undoStack.empty()) {
        undoStack.pop();
    }
}

int UndoManager::getUndoCount() const {
    return undoStack.size();
}

string UndoManager::getLastCommandDescription() const {
    if (canUndo()) {
        return undoStack.top()->getDescription();
    }
    return "No operations to undo";
}
