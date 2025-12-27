#include <vector>
#include <string>
#include <algorithm>

using namespace std;

enum class SearchMode {
    MATCH_ALL,  // AND logic - all criteria must match
    MATCH_ANY   // OR logic - any criteria can match
};

struct SearchCriteria {
    string id;
    string firstName;
    string lastName;
    string department;
    int age;
    char sex;
    int yearOfStudy;
    double minGPA;
    double maxGPA;
    
    SearchCriteria() : id(""), firstName(""), lastName(""), department(""),
                      age(-1), sex('\0'), yearOfStudy(-1), minGPA(-1.0), maxGPA(-1.0) {}
};

class SearchManager {
public:
    // Multi-parameter search
    static vector<Student> search(const vector<Student>& students,
                                      const SearchCriteria& criteria,
                                      SearchMode mode = SearchMode::MATCH_ALL);
    
private:
    static bool matchesCriteria(const Student& student, const SearchCriteria& criteria,
                               SearchMode mode);
};

vector<Student> SearchManager::search(const vector<Student>& students,
                                          const SearchCriteria& criteria,
                                          SearchMode mode) {
    vector<Student> results;
    
    for (const Student& student : students) {
        if (matchesCriteria(student, criteria, mode)) {
            results.push_back(student);
        }
    }
    
    return results;
}

bool SearchManager::matchesCriteria(const Student& student, const SearchCriteria& criteria,
                                   SearchMode mode) {
    int matchCount = 0;
    int criteriaCount = 0;
    
    // Check ID
    if (!criteria.id.empty()) {
        criteriaCount++;
        if (student.getId() == criteria.id) {
            matchCount++;
        }
    }
    
    // Check first name
    if (!criteria.firstName.empty()) {
        criteriaCount++;
        string firstName = student.getFirstName();
        string searchName = criteria.firstName;
        transform(firstName.begin(), firstName.end(), firstName.begin(), ::tolower);
        transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
        
        if (firstName.find(searchName) != string::npos) {
            matchCount++;
        }
    }
    
    // Check last name
    if (!criteria.lastName.empty()) {
        criteriaCount++;
        string lastName = student.getLastName();
        string searchName = criteria.lastName;
        transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
        transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
        
        if (lastName.find(searchName) != string::npos) {
            matchCount++;
        }
    }
    
    // Check department
    if (!criteria.department.empty()) {
        criteriaCount++;
        if (student.getDepartment() == criteria.department) {
            matchCount++;
        }
    }
    
    // Check age
    if (criteria.age != -1) {
        criteriaCount++;
        if (student.getAge() == criteria.age) {
            matchCount++;
        }
    }
    
    // Check sex
    if (criteria.sex != '\0') {
        criteriaCount++;
        if (student.getSex() == criteria.sex) {
            matchCount++;
        }
    }
    
    // Check year of study
    if (criteria.yearOfStudy != -1) {
        criteriaCount++;
        if (student.getYearOfStudy() == criteria.yearOfStudy) {
            matchCount++;
        }
    }
    
    // Check GPA range
    if (criteria.minGPA != -1.0 || criteria.maxGPA != -1.0) {
        criteriaCount++;
        double gpa = student.getGPA();
        bool gpaMatch = true;
        
        if (criteria.minGPA != -1.0 && gpa < criteria.minGPA) {
            gpaMatch = false;
        }
        if (criteria.maxGPA != -1.0 && gpa > criteria.maxGPA) {
            gpaMatch = false;
        }
        
        if (gpaMatch) {
            matchCount++;
        }
    }
    
    // Return based on mode
    if (mode == SearchMode::MATCH_ALL) {
        return matchCount == criteriaCount;
    } else {  // MATCH_ANY
        return matchCount > 0;
    }
}
