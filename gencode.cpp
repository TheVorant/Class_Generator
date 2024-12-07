#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Members
{
  string type;
  vector<string> namesM;
  bool isPointer = false;
  bool isConst = false;
};

struct constG
{
  string pref;
  vector<string> namesC;
};


struct Class 
{
  string name;
  vector<constG> constants;
  vector<constG> addedG;
  vector<Members> proMems;
  vector<Members> pubMems;
};

struct Entry 
{
  string name;
  Entry* next;

  Entry(const string& nv, Entry* n = NULL) 
  {
    name = nv;
    next = n;
  }
};


class HashTable 
{
  protected:
    Entry * * table;
    int count, cap;

    unsigned int hash(const string &s) 
    {
      unsigned int v = 92439827;
      for (int i = 0; i < s.length(); i++) 
      {
        v = v * 169 + s[i];
      }
      return v;
    }
    
    void resize() 
    {
        int newsize = 4 + cap * 3;
        Entry * * newTable = new Entry * [newsize];
        
        for (int i = 0; i < newsize; i++)
        {
          newTable[i] = NULL;
        } 

        for (int i = 0; i < cap; i++) 
        {
          Entry* current = table[i];
          while (current != NULL) 
          {
            Entry* next = current->next;
            int newIndex = hash(current->name) % newsize;

            current->next = newTable[newIndex];
            newTable[newIndex] = current;

            current = next;
          }
        }
        delete[] table;
        table = newTable;
        cap = newsize;
    }

public:
    HashTable()
    {
      count = 0;
      cap = 1;
      table = new Entry * [cap];
      for(int i = 0; i < cap; i++)
      {
        table[i] = NULL; 
      }
    }

    ~HashTable() 
    {
      for (int i = 0; i < cap - 1; i++) 
      {
        Entry* current = table[i];
        while (current != NULL) 
        {
          Entry* next = current->next;
          delete current;
          current = next;
        }
        table[i] = NULL; 
      }
      delete[] table;  
      table = NULL;       
    }

    void insert(string &name) 
    {
      if(contains(name))
      {
        return;
      }
      
      if (count >= cap * 0.75) 
      {
        resize();
      }
      unsigned int index = hash(name) % cap;
      table[index] = new Entry(name, table[index]);
      count++;
    }

    bool contains(const string& name) 
    {
      unsigned int index = hash(name) % cap;
      Entry* current = table[index];
      while (current != NULL) 
      {
        if (current->name == name) 
        {
          return true; 
        }
        current = current->next;
      }
      return false; 
    }
    
};

HashTable RRw;

void RRword()
{
  string words[] = {
      "int", "void", "if", "else", "for", "while", 
      "return", "class", "public", "protected",
      "private", "const", "static", "struct", "bool", 
      "double", "float"
  };

  
  for (int i = 0; i < 17; i++)
  {
    RRw.insert(words[i]);
  }
}

bool validate(const string& name, int lineNum)
{
  if(RRw.contains(name))
  {
    cout << "Error on line " << lineNum << ": '" << name << "' is not a valid C++ name. (ex. Reserved word)\n";
    return false; 
  }
  if(name.empty() || (!isalpha(name[0]) && name[0] != '_'))
  {
    cout << "Error on line " << lineNum << ": '" << name << "' is not a valid C++ name. (ex. Either empty or invalid)\n";
    return false;
  }
  return true;
  
}


void parsconstG(const string &line, constG &group, const vector<string> &lines, 
                int &line_ind, HashTable& classnames, bool & flag)
{
  istringstream in(line.substr(6));
  in >> group.pref;
  
  string name;
  if(!validate(group.pref, line_ind))
  {
    return;
  }
  while(in >> name)
  { 
    if(name != "-")
    {
      string fullname = group.pref + "_" + name;
      if(!validate(fullname, line_ind) || classnames.contains(fullname))
      {
        cout << "Error on line " << line_ind <<": Duplicate or invalid constant name\n";
        flag = true;
      }
      else
      {
        classnames.insert(fullname);
        group.namesC.push_back(name);
      }
    }
  }
  bool cont = !line.empty() && line.back() == '-';
  
  while(cont && ++line_ind < lines.size())
  {
    istringstream next(lines[line_ind]);
    while(next >> name)
    {
      if(name == "-")
      {
         cont = true;
      }
      
      string fullname = group.pref + "_" + name;
      if(!validate(fullname, line_ind) || classnames.contains(fullname))
      {
        cout << "Error on line " << line_ind <<": Duplicate or invalid constant name\n";
        flag = true;
      }
      else
      {
        classnames.insert(fullname);
        group.namesC.push_back(name);
        cont = false;
      }      
    }
    cont = !lines[line_ind].empty() && lines[line_ind].back() == '-';
  }
}


void parseMems(Class &newCL, const string &line, bool isPub,
               HashTable& classnames, int line_ind)
{
  istringstream in(line);
  Members memberhold;
  string word;

  while (in >> word && word != "-")
  {
    if(memberhold.type.empty())
    {
      memberhold.type += "";
    }
    memberhold.type += word;
  }

  for(int i = 0; i < memberhold.type.size(); i++)
  {
    if(memberhold.type[i] == '*')
    {
      memberhold.isPointer = true;
      memberhold.type = memberhold.type.substr(0, memberhold.type.size() - 1);
      if(memberhold.type.substr(0,5) == "const")
      {
        memberhold.type = memberhold.type.substr(5);
        memberhold.isConst = true;
      }
      break;
    }
  }

  

  while (in >> word)
  {
    if(word != "-")
    {
      if(!validate(word, line_ind) || classnames.contains(word))
      {
        cout << "Error on line " << line_ind <<": Duplicate or invalid member name\n";
      }
      else
      {
        classnames.insert(word);
        memberhold.namesM.push_back(word);
      }
    }
  }


  if(isPub)
  {
    newCL.pubMems.push_back(memberhold);
  }
  else
  {
    newCL.proMems.push_back(memberhold);
  }
}

void parsMemsConsts(Class &newCL, const string &line, bool &isPub, const vector<string> &lines, 
                    int &line_ind, HashTable &classnames)
{

  if(line == "pro")
  {
    isPub = false;
    return;
  }
  else if(line == "pub")
  {
    isPub = true;
    return;
  }
  
  if(line.substr(0,5) == "names")
  {
    constG group;
    bool flag = false;
    parsconstG(line, group, lines, line_ind, classnames, flag);
    if(!flag)
    {
      newCL.constants.push_back(group);
    }
  }
  else
  {
    parseMems(newCL, line, isPub, classnames, line_ind);
  }
}

void parsCl(const string &line, int &line_ind, vector<Class> &classes, const vector<string> &lines)
{

  Class new_cl;
  new_cl.name = line.substr(6);
  line_ind++;

  if(!validate(new_cl.name, line_ind))
  { 
    return;
  }

  HashTable classnames;
  classnames.insert(new_cl.name);

  bool isPub = false;
  
  while(line_ind < lines.size() && lines[line_ind] != "end")
  {
    parsMemsConsts(new_cl, lines[line_ind], isPub, lines, line_ind, classnames);
    line_ind++;
  }
  
  line_ind++;
  
  while(line_ind < lines.size() && lines[line_ind].substr(0,5) == "names")
  {
    constG append;
    bool flag = false;
    parsconstG(lines[line_ind], append, lines, line_ind, classnames, flag);
    if(!flag)
    {
      new_cl.addedG.push_back(append);
    }
    line_ind++;
  }
  
  classes.push_back(new_cl);

}


void outHelper(const constG &curconst, ofstream &out, int maxLen, int startLen)
{
  int curLen = startLen;
        
  for (int k = 0; k < curconst.namesC.size(); k++)
  {
    string constStr = curconst.pref + "_" + curconst.namesC[k] + " = " + to_string(k + 1);
    curLen += constStr.length();
                
    if (curLen > maxLen)
    {
      out << ",\n" << string(startLen, ' ');
      curLen = startLen + constStr.length(); 
    }
    else if (k > 0)
    {
      out << ", ";
    }
    out << constStr;
  }
}

void writeget(const Members &curmember, const string &name, ofstream &out)
{
  out << "  ";
  if(curmember.isConst && curmember.isPointer)
  {
    out << " const";
  }
  out << " " << curmember.type;
  if(curmember.isPointer)
  {
    out << " *";
  } 
  out << " get_" << name << "() const\n";
  out << "   { return " << name << "; }\n\n"; 
  
}

void writeset(const Members &curmember, const string &name, ofstream &out)
{

  out << "   void set_" << name << "(";

  if(curmember.isConst && curmember.isPointer)
  {
    out << "const ";
  }
  else if (!curmember.isConst && curmember.isPointer)
  {
    out << "";
  }
  else
  {
    out << "const ";
  }

  out << curmember.type;
  if(curmember.isPointer)
  {
    out << " *";
  }    
  out << " & v)\n";
  out << "   { " << name << " = v; }\n\n"; 
   
}

void getset(const Members &curmember, ofstream &out)
{
  for (int m = 0; m < curmember.namesM.size(); m++)
  {
    const string &name = curmember.namesM[m];
    writeget(curmember, name, out);
    writeset(curmember, name, out);
    
  }
}

void outMembers(const vector<Members> &curmembers, ofstream &out, int idenlevel)
{   
  string indent(idenlevel, ' ');
  for (int n = 0; n < curmembers.size(); n++)
  {
    const Members &curmember = curmembers[n];
    if(!curmember.namesM.empty())
    {
      out << indent << curmember.type << " ";
      for (int p = 0; p < curmember.namesM.size(); p++)
      {
        if(curmember.isPointer)
        {
          out << "* ";
        }
      
        out << curmember.namesM[p];
        if(p < curmember.namesM.size() - 1)
        {
          out << ", ";
        } 
      }
      out << ";\n";          
    }
  }
}


void outfile(const vector<Class> &classes, ofstream &out)
{
  for (int i = 0; i < classes.size(); i++)
  {
    int maxLen = 72;
    const Class &curclass = classes[i];
    out << "class " << curclass.name << "\n";
    out << "{\n";

    if(!curclass.proMems.empty())
    {
      out << " protected: \n";
    }

    outMembers(curclass.proMems, out, 3);

    if(!curclass.pubMems.empty() || !curclass.constants.empty() || !curclass.proMems.empty())
    {
      out << "\n public:\n";
    }
    outMembers(curclass.pubMems, out, 3);
    out << "\n";    
    for (int j = 0; j < curclass.constants.size(); j++)
    {
      const constG &curconst = curclass.constants[j];
      out << "  static const int ";
      outHelper(curconst, out, maxLen, 20);
      out << ";\n\n";
    }
    
    for (int q = 0; q < curclass.proMems.size(); q++)
    {
      const Members &curmember = curclass.proMems[q];
      getset(curmember, out);
    }
    
    for (int r = 0; r < curclass.pubMems.size(); r++)
    {
      const Members &curmember = curclass.pubMems[r];
      getset(curmember, out);
    }

    
    out << "};\n\n";


    for (int l = 0; l < curclass.addedG.size(); l++)
    {
      const constG &curadded = curclass.addedG[l];
      out << "const int ";
      outHelper(curadded, out, maxLen, 10);
      out << ";\n\n";
    }
  }
}






void processFile(const string &filename, const string &output) 
{
  ifstream file(filename);
  if (!file.is_open()) 
  {
    cerr << "Error opening file: " << filename << "\n";
    return;
  }
  ofstream out(output);
  if (!out.is_open())
  {
    cerr << "Error opening file: " << output << "\n";
    return;  
  }
  
  vector<Class> classes;
  vector<string> lines;
  string line;

  while (getline(file, line)) 
  {
    if(!line.empty())
    {
      lines.push_back(line);
    }
  }
  file.close();
 
  int line_ind = 0;
  RRword();
  

  while(line_ind < lines.size())
  {
    if(lines[line_ind].substr(0,5) == "class")
    {
     parsCl(lines[line_ind], line_ind, classes, lines);
    }
    else
    {
      line_ind++;
    }
  }
  
  outfile(classes, out);
  out.close();
  
}

int main(int argc, char *argv[]) 
{
  string filename = "input.txt";
  string outputF = "output.txt";
  if (argc > 1) 
  {
    filename = argv[1];
  }
  if (argc > 2) 
  {
    outputF = argv[2]; 
  }
    processFile(filename, outputF);
    return 0;
}
