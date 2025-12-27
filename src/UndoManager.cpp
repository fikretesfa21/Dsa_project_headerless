#include <stack>
#include <memory>
#include <string>

using namespace std;

// Forward declaration
class StudentManager;

// Abstract Command base class
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual string getDescription() const = 0;
};

// Add Student Command
class AddStudentCommand : public Command {
private:
    StudentManager* manager;
    Student student;
    bool executed;
    
public:
    AddStudentCommand(StudentManager* mgr, const Student& s);
    void execute() override;
    void undo() override;
    string getDescription() const override;
};

// Update Student Command
class UpdateStudentCommand : public Command {
private:
    StudentManager* manager;
    Student oldStudent;
    Student newStudent;
    bool executed;
    
public:
    UpdateStudentCommand(StudentManager* mgr, const Student& oldS, const Student& newS);
    void execute() override;
    void undo() override;
    string getDescription() const override;
};

// Delete Student Command
class DeleteStudentCommand : public Command {
private:
    StudentManager* manager;
    Student student;
    bool executed;
    
public:
    DeleteStudentCommand(StudentManager* mgr, const Student& s);
    void execute() override;
    void undo() override;
    string getDescription() const override;
};

// Undo Manager
class UndoManager {
private:
    stack<unique_ptr<Command>> undoStack;
    static const int MAX_UNDO_SIZE = 5;
    
public:
    void executeCommand(unique_ptr<Command> command);
    bool canUndo() const;
    void undo();
    void clear();
    int getUndoCount() const;
    string getLastCommandDescription() const;
};
