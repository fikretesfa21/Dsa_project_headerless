#include <stack>
#include <memory>
#include <string>

using namespace std;


class StudentManager;


class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual string getDescription() const = 0;
};


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
