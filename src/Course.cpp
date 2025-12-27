#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Course {
private:
    string courseCode;
    string courseName;
    string department;
    int yearOfStudy;
    int creditHours;

public:
    // Constructors
    Course();
    Course(const string& code, const string& name, 
           const string& dept, int year, int credits);
    
    // Getters
    string getCourseCode() const { return courseCode; }
    string getCourseName() const { return courseName; }
    string getDepartment() const { return department; }
    int getYearOfStudy() const { return yearOfStudy; }
    int getCreditHours() const { return creditHours; }
    
    // Setters
    void setCourseCode(const string& code) { courseCode = code; }
    void setCourseName(const string& name) { courseName = name; }
    void setDepartment(const string& dept) { department = dept; }
    void setYearOfStudy(int year) { yearOfStudy = year; }
    void setCreditHours(int credits) { creditHours = credits; }
    
    // CSV serialization
    string toCSV() const;
    static Course fromCSV(const string& csvLine);
};

Course::Course() : courseCode(""), courseName(""), department(""),
                   yearOfStudy(0), creditHours(0) {}

Course::Course(const string& code, const string& name,
               const string& dept, int year, int credits)
    : courseCode(code), courseName(name), department(dept),
      yearOfStudy(year), creditHours(credits) {}

string Course::toCSV() const {
    return department + "," + to_string(yearOfStudy) + "," +
           courseCode + "," + courseName + "," + to_string(creditHours);
}

Course Course::fromCSV(const string& csvLine) {
    // Note: Utils::split and Utils::trim are assumed to be available
    // because we will include Utils.cpp before Course.cpp in the unity build.
    // However, to make this file compilable on its own if needed (or just valid C++),
    // we should really rely on the fact that Utils is available.
    // But since we are deleting headers, we can't include Utils.h.
    // In a strict unity build, we depend on the order.
    
    vector<string> fields = Utils::split(csvLine, ',');
    
    if (fields.size() < 5) {
        return Course();
    }
    
    string dept = Utils::trim(fields[0]);
    int year = stoi(Utils::trim(fields[1]));
    string code = Utils::trim(fields[2]);
    string name = Utils::trim(fields[3]);
    int credits = stoi(Utils::trim(fields[4]));
    
    return Course(code, name, dept, year, credits);
}
