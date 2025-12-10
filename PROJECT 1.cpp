#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

map<string, double> GRADE_POINTS = {
    {"A+", 4.0}, {"A", 4.0}, {"A-", 3.7},
    {"B+", 3.3}, {"B", 3.0}, {"B-", 2.7},
    {"C+", 2.3}, {"C", 2.0}, {"C-", 1.7},
    {"D+", 1.3}, {"D", 1.0}, {"F", 0.0}
};

class Course {
private:
    string code;
    string name;
    double credits;
    string grade;

public:
    Course(string c = "", string n = "", double cr = 0.0, string g = "")
        : code(c), name(n), credits(cr), grade(g) {}

    double getGradePoints() const {
        auto it = GRADE_POINTS.find(grade);
        if (it != GRADE_POINTS.end())
            return it->second * credits;
        return 0.0;
    }

    double getCredits() const { return credits; }
    string getGrade() const { return grade; }
    string getCode() const { return code; }
    string getName() const { return name; }

    string toCSV() const {
        stringstream ss;
        ss << code << "," << name << "," << credits << "," << grade;
        return ss.str();
    }

    static Course fromCSV(const string& line) {
        string c, n, crStr, g;
        stringstream ss(line);

        getline(ss, c, ',');
        getline(ss, n, ',');
        getline(ss, crStr, ',');
        getline(ss, g, ',');

        double cr = 0.0;
        if (!crStr.empty()) cr = stod(crStr);

        return Course(c, n, cr, g);
    }
};

class Semester {
private:
    string semesterCode;
    vector<Course> courses;

public:
    Semester(string code = "") : semesterCode(code) {}

    void addCourse(const Course& c) { courses.push_back(c); }

    double calculateSemesterGPA() const {
        double totalPoints = 0, totalCredits = 0;

        for (const auto& c : courses) {
            totalPoints += c.getGradePoints();
            totalCredits += c.getCredits();
        }

        return (totalCredits == 0) ? 0.0 : totalPoints / totalCredits;
    }

    string getSemesterCode() const { return semesterCode; }
    const vector<Course>& getCourses() const { return courses; }

    string toCSV() const {
        stringstream ss;
        ss << "SEMESTER," << semesterCode << "\n";
        for (const auto& c : courses)
            ss << "COURSE," << c.toCSV() << "\n";
        return ss.str();
    }
};

class Student {
private:
    string name;
    string id;
    vector<Semester> semesters;

public:
    Student(string n = "", string i = "") : name(n), id(i) {}

    void addSemester(const Semester& s) { semesters.push_back(s); }

    double calculateCumulativeGPA() const {
        double totalPoints = 0, totalCredits = 0;

        for (const auto& s : semesters) {
            for (const auto& c : s.getCourses()) {
                totalPoints += c.getGradePoints();
                totalCredits += c.getCredits();
            }
        }
        return (totalCredits == 0) ? 0.0 : totalPoints / totalCredits;
    }

    const vector<Semester>& getSemesters() const { return semesters; }
    string getName() const { return name; }
    string getId() const { return id; }
};

class Transcript {
private:
    Student student;

public:
    Transcript(Student s) : student(std::move(s)) {}

    void addSemester(const Semester& s) { student.addSemester(s); }

    void generateReport() const {
        cout << "Transcript for: " << student.getName()
             << " (" << student.getId() << ")\n\n";

        for (const auto& s : student.getSemesters()) {
            cout << "Semester: " << s.getSemesterCode() << "\n";

            cout << left << setw(10) << "Code"
                 << setw(28) << "Name"
                 << setw(10) << "Credits"
                 << setw(8) << "Grade" << "\n";

            for (const auto& c : s.getCourses()) {
                cout << left
                     << setw(10) << c.getCode()
                     << setw(28) << c.getName()
                     << setw(10) << c.getCredits()
                     << setw(8) << c.getGrade()
                     << "\n";
            }

            cout << "Semester GPA: " << fixed << setprecision(2)
                 << s.calculateSemesterGPA() << "\n\n";
        }

        cout << "Cumulative GPA: " << fixed << setprecision(2)
             << student.calculateCumulativeGPA() << "\n";
    }

    void saveToFile(const string& filename) const {
        ofstream out(filename);

        if (!out) {
            cerr << "Error: Could not open file " << filename << "\n";
            return;
        }

        for (const auto& s : student.getSemesters()) {
            out << s.toCSV();
        }
    }
};

int main() {
    Student george("George Lordkipanidze", "S12345");
    Transcript transcript(george);

    Semester fall2025("2025A0");
    fall2025.addCourse(Course("CSC319", "Object-Oriented Design", 3.0, "A"));
    fall2025.addCourse(Course("ENG101", "English Composition", 3.0, "B+"));
    transcript.addSemester(fall2025);

    Semester spring2025("2025B0");
    spring2025.addCourse(Course("CSC223", "Data Structures", 3.0, "A-"));
    spring2025.addCourse(Course("MAT223", "Discrete Math", 3.0, "B"));
    transcript.addSemester(spring2025);

    transcript.generateReport();
    transcript.saveToFile("transcript.csv");

    cout << "\nDemonstration complete. Check transcript.csv for saved data.\n";
    return 0;
}
