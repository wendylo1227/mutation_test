    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <vector>
    #include <string>
    #include <map>
    #include <utility>
    #include <filesystem>
    #include <chrono>
    #include <set>
    #include <windows.h>
    #include <codecvt>
    #include <algorithm>
    using namespace std;

    // use global variable to store mutation operators
    map<string, vector<string>> mutationop;

    // read source code, and simply branch the code and access the entire code
    string readSourceCode(const string& filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Can't open file:" << filename << endl;
            return "";
        }
        string line, content;
        // read program by line, and connect each line
        while (getline(file, line)) {
            content += line + "\n";
        }
        return content;
    }

    // select which mutation test to use to generate mutants
    string choose()
    {
        int ans;
        cout << "Choose one mutation operator:" << endl;
        cout << "1. Relation operator" << endl;
        cout << "2. Arithmetic operator" << endl;
        cout << "3. Logical connector operator" << endl;
        cout << "4. Data type replacement" << endl;
        cout << "5. Logical fault injection" << endl;
        cout << "Input selection: ";
        cin >> ans;
        switch (ans)
        {
        case 1:
            return "relation";
        case 2:
            return "arithmetic";
        case 3:
            return "logical";
        case 4:
            return "datatype";
        case 5:
            return "logicalfaultinjection";
        default:
            return "";
        }
    }


    /*
        mutationop["=="] = { "!=", ">",">=","<","<=" };
        mutationop["!="] = { "==", ">",">=","<","<=" };
        mutationop[">"] = { "!=", "==",">=","<","<=" };
        mutationop["<"] = { "!=", ">",">=","==","<=" };
        mutationop[">="] = { "!=", "==",">","<","<=" };
        mutationop["<="] = { "!=", ">",">=","<","==" };
    */

    // mutation operator
    // define_operator() function just read operator
    void define_operator()
    {
        // relation operator(ROR) and for loop operation co
        // nvert
        mutationop["=="] = { "!=", ">",">=","<","<=" };
        mutationop["!="] = { "==", ">",">=","<","<=" };
        mutationop[">"] = { "!=", "==",">=","<","<=" };
        mutationop["<"] = { "!=", ">",">=","==","<=" };
        mutationop[">="] = { "!=", "==",">","<","<=" };
        mutationop["<="] = { "!=", ">",">=","<","==" };
        // arithmetic operator(AOR)
        mutationop["+"] = { "-", "*", "/"};
        mutationop["-"] = { "+", "*", "/"};
        mutationop["*"] = { "+", "-", "/"};
        mutationop["/"] = { "+", "-", "*"};
        mutationop["++"] = { "+=2","+=3","+=4","+=5" };
        mutationop["+=2"] = { "++","+=3","+=4","+=5" };
        mutationop["+=3"] = { "++","+=2","+=4","+=5" };
        mutationop["+=4"] = { "++","+=2","+=3","+=5" };
        mutationop["+=5"] = { "++","+=2","+=3","+=4" };
        mutationop["+1"] = { "+2","+3","+4","+5" };
        mutationop["+2"] = { "+1","+3","+4","+5" };
        mutationop["+3"] = { "+1","+2","+4","+5" };
        mutationop["+4"] = { "+1","+2","+3","+5" };
        mutationop["+5"] = { "+1","+2","+3","+4" };
        mutationop["--"] = { "-=2","-=3","-=4","-=5" };
        mutationop["-=2"] = { "--","-=3","-=4","-=5" };
        mutationop["-=3"] = { "--","-=2","-=4","-=5" };
        mutationop["-=4"] = { "--","-=2","-=3","-=5" };
        mutationop["-=5"] = { "--","-=2","-=3","-=4" };
        mutationop["-1"] = { "-2","-3","-4","-5" };
        mutationop["-2"] = { "-1","-3","-4","-5" };
        mutationop["-3"] = { "-1","-2","-4","-5" };
        mutationop["-4"] = { "-1","-2","-3","-5" };
        mutationop["-5"] = { "-1","-2","-3","-4" };
        // logical connector operator(LCR)
        mutationop["&&"] = { "||" };
        mutationop["||"] = { "&&" };
        // data type replacement
        mutationop["int"] = { "float","bool" };
        mutationop["float"] = { "int","bool" };
        mutationop["bool"] = { "int","float" };
        // logical fault injection
        mutationop["true"] = { "false" };
        mutationop["false"] = { "true" };
    }

    enum operatortype
    {
        relation,
        arithmetic,
        logical,
        datatype,
        logicalfaultinjection,
        unknow
    };

    // convert operator_type into enum
    operatortype get_operator_type(const string& operator_type)
    {
        if (operator_type == "relation") return relation;
        if (operator_type == "arithmetic") return arithmetic;
        if (operator_type == "logical") return logical;
        if (operator_type == "datatype") return datatype;
        if (operator_type == "logicalfaultinjection") return logicalfaultinjection;
        return unknow;
    }

    // bug location
    vector<pair<int, string>> find_bug_location(const string& operator_type, const string& code, const vector<string>& skip_templates)
    {
        vector<string> operators;
        operatortype type = get_operator_type(operator_type);
        switch (type)
        {
        case relation:
            operators = { "==","!=",">=","<=",">","<" };
            cout << "Use 'Relation Operator' to find all bugs location in program." << endl;
            break;
        case arithmetic:
            operators = { "+","-","*","/","++","+=2","+=3","+=4","+=5","+1","+2","+3","+4","+5",
                          "--","-=2","-=3","-=4","-=5","-1","-2","-3","-4","-5" };
            cout << "Use 'Arithmetic Operator' to find all bugs location in program." << endl;
            break;
        case logical:
            operators = { "&&","||" };
            cout << "Use 'Logic Connector Operator' to find all bugs location in program." << endl;
            break;
        case datatype:
            operators = { "int","float","bool" };
            cout << "Use 'Data Type Replacement' to find all bugs location in program." << endl;
            break;
        case logicalfaultinjection:
            operators = { "true","false" };
            cout << "Use 'Logic Fault Injection' to find all bugs location in program." << endl;
            break;
        }

        vector<pair<int, string>> locations;
        size_t n = code.size();

        bool inBlockComment = false;
        bool inString = false;
        bool inChar = false;
        int line = 1;

        const vector<string> multi_ops = { "==","!=","<=",">=","&&","||","<<",">>","++","--","+=","-=","*=","/=",
                                           "&","|","^","~","?:" };

        for (size_t i = 0; i < n; ++i)
        {
            char ch = code[i];

            if (ch == '\n') { ++line; continue; }

            if (inBlockComment) {
                if (ch == '*' && i + 1 < n && code[i + 1] == '/') { inBlockComment = false; ++i; }
                continue;
            }
            if (inString) {
                if (ch == '\\' && i + 1 < n) { ++i; continue; }
                if (ch == '"') { inString = false; }
                continue;
            }
            if (inChar) {
                if (ch == '\\' && i + 1 < n) { ++i; continue; }
                if (ch == '\'') { inChar = false; }
                continue;
            }

            // 跳過註解、字串、巨集
            if (ch == '/' && i + 1 < n && code[i + 1] == '*') { inBlockComment = true; ++i; continue; }
            if (ch == '/' && i + 1 < n && code[i + 1] == '/') {
                while (i < n && code[i] != '\n') ++i;
                ++line;
                continue;
            }
            if (ch == '"') { inString = true; continue; }
            if (ch == '\'') { inChar = true; continue; }
            if (ch == '#') {
                size_t j = i + 1;
                while (j < n && isspace(code[j])) j++;
                if (code.compare(j, 6, "define") != 0) {
                    while (i < n && code[i] != '\n') ++i;
                    ++line;
                    continue;
                }
            }

            // 跳過 template / 容器宣告
            if (ch == '<')
            {
                bool skip_template = false;
                size_t j = i;
                while (j > 0 && isspace((unsigned char)code[j - 1])) --j;
                string before;
                size_t k = j;
                while (k > 0 && (isalnum((unsigned char)code[k - 1]) || code[k - 1] == '_' || code[k - 1] == '>')) {
                    --k;
                }
                before = code.substr(k, j - k);
                if (before == "template" || before == "vector" || before == "map" ||
                    before == "pair" || before == "set" || before == "list" ||
                    before == "static_cast" || before == "dynamic_cast" ||
                    before == "reinterpret_cast" || before == "const_cast" ||
                    (find(skip_templates.begin(), skip_templates.end(), before) != skip_templates.end())) {
                    skip_template = true;
                }
                if (skip_template) {
                    int depth = 1;
                    size_t m = i + 1;
                    while (m < n && depth > 0) {
                        if (code[m] == '<') depth++;
                        else if (code[m] == '>') depth--;
                        else if (code[m] == '\n') line++;
                        m++;
                    }
                    if (depth == 0) {
                        i = m - 1;
                        continue;
                    }
                }
            }

            // ===== 特殊過濾：忽略 cout/cerr/clog =====
            if (i >= 4) {
                string prev4 = code.substr(i - 4, 4);
                if (prev4 == "cout" || prev4 == "cerr" || prev4 == "clog") {
                    size_t tmp = i + 1;
                    while (tmp < n && isspace(code[tmp])) tmp++;
                    if (code.substr(tmp, 2) == "<<") {
                        i = tmp + 2; // 只跳過 <<，繼續檢查後面的運算
                        continue;
                    }
                }
            }


            // ====== 特殊處理：忽略 <=> ======
            if (i + 2 < n && code.compare(i, 3, "<=>") == 0) {
                i += 2; // 跳過整個 <=> 符號
                continue; // 不要記錄
            }

            if ((type == datatype || type == logicalfaultinjection) && isalpha((unsigned char)ch)) {
                size_t j = i;
                while (j < n && (isalnum((unsigned char)code[j]) || code[j] == '_')) j++;
                string word = code.substr(i, j - i);
                if (find(operators.begin(), operators.end(), word) != operators.end()) {
                    locations.emplace_back(line, word);
                    cout << "Line " << line << " include datatype " << word << endl;
                }
                i = j - 1;
                continue;
            }
            
            // 偵測多字元運算子
            bool matched = false;
            for (const string& mop : multi_ops)
            {
                size_t len = mop.size();
                if (i + len <= n && code.compare(i, len, mop) == 0)
                {
                    if (find(operators.begin(), operators.end(), mop) != operators.end())
                    {
                        locations.emplace_back(line, mop);
                        cout << "Line " << line << " include operator " << mop << endl;
                    }
                    i += len - 1;
                    matched = true;
                    break;
                }
            }
            if (matched) continue;

            // 偵測單字元運算子
            if (ch == '<' || ch == '>' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
                ch == '&' || ch == '|' || ch == '^' || ch == '~' || ch == '?')
            {
                string token(1, ch);
                if (find(operators.begin(), operators.end(), token) != operators.end())
                {
                    locations.emplace_back(line, token);
                    cout << "Line " << line << " include operator " << token << endl;
                }
            }
        }

        return locations;
    }

    void single_mutation(const string& code, const vector<pair<int, string>>& bug_locations, const string& output_directory)
    {
        define_operator(); // global variable mutationop

        int mutant_count = 1;
        set<string> generated_mutants; 

        vector<string> lines;
        size_t start = 0;
        size_t end;
        while ((end = code.find('\n', start)) != string::npos) {
            lines.push_back(code.substr(start, end - start));
            start = end + 1;
        }
        lines.push_back(code.substr(start));

        for (const auto& location : bug_locations)
        {
            int line_number = location.first;
            const string& operator_to_replace = location.second;

            const auto& replacements = mutationop[operator_to_replace];
            if (replacements.empty()) continue;

            if (line_number > 0 && line_number <= lines.size())
            {
                string& target_line = lines[line_number - 1];
                size_t pos = 0;
                while ((pos = target_line.find(operator_to_replace, pos)) != string::npos)
                {
                    for (const auto& replacement_op : replacements)
                    {
                        vector<string> mutated_lines = lines;
                        string& mutated_line = mutated_lines[line_number - 1];
                        mutated_line.replace(pos, operator_to_replace.length(), replacement_op);

                        string mutated_code;
                        for (const auto& line : mutated_lines)
                        {
                            mutated_code += line + "\n";
                        }

                        if (generated_mutants.count(mutated_code) == 0) {
                            generated_mutants.insert(mutated_code); 

                            string filename = output_directory + "/mutant_" + to_string(mutant_count) + ".cpp";
                            ofstream outfile(filename);
                            if (outfile.is_open()) {
                                outfile << mutated_code;
                                outfile.close();
                                cout << "Generate mutant file: " << filename
                                    << ", line " << line_number << " pos " << pos
                                    << " replace " << operator_to_replace << " with " << replacement_op << endl;
                                mutant_count++;
                            }
                            else {
                                cout << "Can't open file:" << filename << endl;
                            }
                        }
                        else {
                            cout << "Skip duplicate mutant at line " << line_number << ", replacement " << operator_to_replace << " → " << replacement_op << endl;
                        }
                    }
                    pos += operator_to_replace.length(); 
                }
            }
        }
    }



    void double_mutation(const string& code, const vector<pair<int, string>>& bug_locations1, const vector<pair<int, string>>& bug_locations2, const string& output_directory)
    {
        define_operator();
        int mutant_count = 1;

        // 將原始碼切分成行
        vector<string> lines;
        size_t start = 0, end;
        while ((end = code.find('\n', start)) != string::npos) {
            lines.push_back(code.substr(start, end - start));
            start = end + 1;
        }
        lines.push_back(code.substr(start));

        // 建立一個 map 來紀錄每一行中各個運算子出現的次序（index）
        // 這能讓我們區分同一行中的第一個、第二個運算子
        auto get_nth_pos = [](const string& line, const string& op, int occurrence) -> size_t {
            size_t pos = line.find(op);
            for (int i = 0; i < occurrence && pos != string::npos; ++i) {
                pos = line.find(op, pos + 1);
            }
            return pos;
            };

        // 為了知道 loc1 是第幾個，我們需要掃描 bug_locations
        for (size_t i = 0; i < bug_locations1.size(); ++i) {
            int line1 = bug_locations1[i].first;
            string op1 = bug_locations1[i].second;

            // 計算 loc1 是在 line1 中的第幾次出現
            int occ1 = 0;
            for (size_t k = 0; k < i; ++k) {
                if (bug_locations1[k].first == line1 && bug_locations1[k].second == op1) occ1++;
            }

            for (size_t j = 0; j < bug_locations2.size(); ++j) {
                int line2 = bug_locations2[j].first;
                string op2 = bug_locations2[j].second;

                // 計算 loc2 是在 line2 中的第幾次出現
                int occ2 = 0;
                for (size_t k = 0; k < j; ++k) {
                    if (bug_locations2[k].first == line2 && bug_locations2[k].second == op2) occ2++;
                }

                // 排除完全相同的變異點（同一行、同一個運算子、同一個序位）
                if (line1 == line2 && op1 == op2 && occ1 == occ2) continue;
                if (mutationop[op1].empty() || mutationop[op2].empty()) continue;

                for (const auto& repl1 : mutationop[op1]) {
                    for (const auto& repl2 : mutationop[op2]) {
                        vector<string> mutated_lines = lines;

                        // 找出確切的 pos1 與 pos2
                        size_t pos1 = get_nth_pos(lines[line1 - 1], op1, occ1);
                        size_t pos2 = get_nth_pos(lines[line2 - 1], op2, occ2);

                        if (pos1 == string::npos || pos2 == string::npos) continue;

                        // 同一行變異時，必須由後往前替換
                        if (line1 == line2) {
                            if (pos1 > pos2) {
                                mutated_lines[line1 - 1].replace(pos1, op1.length(), repl1);
                                mutated_lines[line2 - 1].replace(pos2, op2.length(), repl2);
                            }
                            else {
                                mutated_lines[line2 - 1].replace(pos2, op2.length(), repl2);
                                mutated_lines[line1 - 1].replace(pos1, op1.length(), repl1);
                            }
                        }
                        else {
                            mutated_lines[line1 - 1].replace(pos1, op1.length(), repl1);
                            mutated_lines[line2 - 1].replace(pos2, op2.length(), repl2);
                        }

                        string filename = output_directory + "/mutant_" + to_string(mutant_count++) + ".cpp";
                        ofstream outfile(filename);
                        for (const auto& ml : mutated_lines) outfile << ml << "\n";
                        outfile.close();

                        cout << "Generate mutant file: " << filename
                            << ", line " << line1 << " pos " << pos1 << " " << op1 << " to " << repl1
                            << " and line " << line2 << " pos " << pos2 << " " << op2 << " to " << repl2 << endl;
                    }
                }
            }
        }
    }

    /*
        Each time a file is generated for a mutation operator,
        if a previously generated file is found during the next execution,
        the previous file will be deleted.
    */
    void deleteFiles(const string& folderPath) {
        // convert string to wstring
        wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
        wstring wideFolderPath = converter.from_bytes(folderPath);
        wstring searchPath = wideFolderPath + L"\\*"; // distribution path for searching

        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            cerr << "Can't open folder:" << folderPath << " (Error:" << GetLastError() << ")" << endl;
            return;
        }

        int deleteCount = 0; // record the file successfully deleted

        do {
            wstring fileName = findData.cFileName;

            if (fileName == L"." || fileName == L"..") {
                continue;
            }

            // check the file extantion if it is .cpp or not
            if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == L".cpp") {
                // build the complete path of the file
                wstring fullFilePath = wideFolderPath + L"/" + fileName;

                // check if it is a file rather than a folder
                if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    if (DeleteFileW(fullFilePath.c_str())) {
                        deleteCount++;
                    }
                    else {
                        cerr << "Can't delete the file:" << converter.to_bytes(fullFilePath)
                            << " (Error:" << GetLastError() << ")" << endl;
                    }
                }
            }
        } while (FindNextFileW(hFind, &findData));

        FindClose(hFind);

    }

    char ask() {
        char ans;
        cout << "How many mutantions needed to generate?" << endl;
        cout << "1. single" << endl;
        cout << "2. double" << endl;
        cout << "Input selection: ";
        cin >> ans;
        if (ans == '1') {
            return '1';
        }
        else if (ans == '2') {
            return '2';
        }
    }

    int main()
    {
        define_operator();
        string original = readSourceCode("original_program.cpp");
        if (original.empty())
            return 1;

        string folderPath = ".\\mutants";
        auto start = chrono::high_resolution_clock::now();

        // ======= 讀取 skip_list =======
        vector<string> skip_list;
        cout << "請輸入要跳過變異的 template 名稱（輸入 end 結束）：\n";
        string temp;
        while (true) {
            cout << "> ";
            cin >> temp;
            if (temp == "end") break;
            skip_list.push_back(temp);
        }

        string operator_type;
        vector<pair<int, string>> bug_locations1, bug_locations2;

        switch (ask()) {
        case '1':
            operator_type = choose();
            bug_locations1 = find_bug_location(operator_type, original, skip_list);
            deleteFiles(folderPath);
            single_mutation(original, bug_locations1, folderPath);
            break;
        case '2':
            operator_type = choose();
            bug_locations1 = find_bug_location(operator_type, original, skip_list);
            operator_type = choose();
            bug_locations2 = find_bug_location(operator_type, original, skip_list);
            deleteFiles(folderPath);
            double_mutation(original, bug_locations1, bug_locations2, folderPath);
            break;
        default:
            cout << "Error command.";
            break;
        }

        cout << "Mutation Testing Successfully Enforce." << endl;
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;
        cout << "Running time:" << duration.count() << " ms" << endl;

        return 0;
    }




