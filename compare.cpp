//20029781 scylh1 Lanxin HU
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <vector>

using namespace std;

class cppfile
{
public:
  virtual void read_a_cppfile(string) = 0;

protected:
  int num_of_class;
  int num_of_cin;
  int num_of_cout;
  int num_of_public;
  int num_of_protected;
  int num_of_private;
  int num_of_const;
  int num_of_if;

  int length_of_file;
  int num_of_lines;
  int num_of_blocks;
  int num_of_asterisks;
  int num_of_ampersands;
  int num_of_commas;

  bool use_new;
  bool use_friend;
  bool use_static;
  bool use_argc;
};

class cppfile_measure : public cppfile
{
private:
  int num_of_new;
  int num_of_friend;
  int num_of_static;
  int num_of_argc;
  //replace "xxxxx" with space when counting keywords
  string fix_quotation(string str);
  //remove the space in the beginning and end of the line
  string fix_space(string str);
  //replace the comments" with blankspace
  string remove_comment1(string comment1);
  string remove_comment2(string comment2);

public:
  bool accepted_file;
  cppfile_measure();
  void read_a_cppfile(string file_name);
  vector<int> get_result_array();
};

vector<int> cppfile_measure ::get_result_array()
{
  vector<int> result;
  result.push_back(num_of_class);
  result.push_back(num_of_cin);
  result.push_back(num_of_cout);
  result.push_back(num_of_public);
  result.push_back(num_of_protected);
  result.push_back(num_of_private);
  result.push_back(num_of_const);
  result.push_back(num_of_if);
  result.push_back(length_of_file);
  result.push_back(num_of_lines);
  result.push_back(num_of_blocks);
  result.push_back(num_of_asterisks);
  result.push_back(num_of_ampersands);
  result.push_back(num_of_commas);
  if (use_new)
  {
    result.push_back(num_of_new);
  }
  else
  {
    result.push_back(0);
  }

  if (use_friend)
    result.push_back(num_of_friend);
  else
    result.push_back(0);

  if (use_static)
    result.push_back(num_of_static);
  else
    result.push_back(0);

  if (use_argc)
    result.push_back(num_of_argc);
  else
    result.push_back(0);

  return result;
}

class FileCompare
{
public:
  double cosine_similarity(vector<int> A, vector<int> B)
  {
    double mul = 0.0, d_a = 0.0, d_b = 0.0;
    int size = A.size();
    size = 14;
    for (unsigned int i = 0; i < size; ++i)
    {
      mul += A[i] * B[i];
      d_a += A[i] * A[i];
      d_b += B[i] * B[i];
    }
    return mul / (sqrt(d_a) * sqrt(d_b));
  }

  double equal_weight_similarity(vector<int> A, vector<int> B)
  {
    double *sim_arr = new double[A.size()];
    for (int i = 0; i < A.size(); i++)
    {
      if (A[i] == 0 && B[i] == 0)
      {
        sim_arr[i] = 1;
      }
      else if (A[i] == 0 || B[i] == 0)
      {
        sim_arr[i] = 0;
      }
      else if (A[i] >= B[i])
      {
        sim_arr[i] = (double)B[i] / (double)A[i];
      }
      else
      {
        sim_arr[i] = (double)A[i] / (double)B[i];
      }
    }
    double sim_sum = 0.0;

    for (int i = 0; i < A.size(); i++)
    {
      sim_sum += sim_arr[i];
    }

    delete[] sim_arr;
    return sim_sum / A.size();
  }
};

cppfile_measure::cppfile_measure()
{
  num_of_class = 0;
  num_of_cin = 0;
  num_of_cout = 0;
  num_of_public = 0;
  num_of_protected = 0;
  num_of_private = 0;
  num_of_const = 0;
  num_of_if = 0;

  length_of_file = 0;
  num_of_lines = 0;
  num_of_blocks = 0;
  num_of_asterisks = 0;
  num_of_ampersands = 0;
  num_of_commas = 0;

  use_new = false;
  num_of_new = 0;
  use_friend = false;
  num_of_friend = 0;
  use_static = false;
  num_of_static = 0;
  use_argc = false;
  num_of_argc = 0;

  accepted_file = true;
}

void cppfile_measure::read_a_cppfile(string file_name)
{
  fstream file;
  file.open(file_name); // open the file

  if (file.is_open())
  {
    accepted_file = true;
    vector<string> content;
    string word;
    bool isComment = false;
    string comment1 = "//";
    string comment2 = "/*";
    string comment3 = "*/";
    string quotation1 = "\"";
    string quotation2 = "\'";
    while (getline(file, word))
    {
      if (isComment == false)
      {
        num_of_lines++;
      }
      int size = word.size();
      length_of_file += size;
      // replace "//" and string chars after "//" with blankspace
      if (word.find(comment1) != string::npos)
      {
        word = remove_comment1(word);
      }
      // replace "/**/" and the comments between them with blankspace
      if (word.find(comment2) != string::npos)
      {
        isComment = true;
      }
      if (isComment)
      {
        word = remove_comment2(word);
      }
      if (word.find(comment3) != string::npos)
      {
        isComment = false;
      }
      //remove the blankspaces in the head and tail of the line
      word = fix_space(word);

      if (word.find(quotation1) != string::npos || word.find(quotation2) != string::npos)
      {
        word = fix_quotation(word);
      }

      string::size_type pos = 0;
      string class_s = "class";
      while ((pos = word.find(class_s, pos)) != string::npos)
      {
        num_of_class++;
        pos++;
      }

      pos = 0;
      string cin_s = "cin";
      while ((pos = word.find(cin_s, pos)) != string::npos)
      {
        num_of_cin++;
        pos++;
        pos++;
      }

      pos = 0;
      string cout_s = "cout";
      while ((pos = word.find(cout_s, pos)) != string::npos)
      {
        num_of_cout++;
        pos++;
      }
      pos = 0;
      string public_s = "public";
      while ((pos = word.find(public_s, pos)) != string::npos)
      {
        num_of_public++;
        pos++;
      }
      pos = 0;
      string protected_s = "protected";
      while ((pos = word.find(protected_s, pos)) != string::npos)
      {
        num_of_protected++;
        pos++;
      }
      pos = 0;
      string private_s = "private";
      while ((pos = word.find(private_s, pos)) != string::npos)
      {
        num_of_private++;
        pos++;
      }
      pos = 0;
      string const_s = "const";
      while ((pos = word.find(const_s, pos)) != string::npos)
      {
        num_of_const++;
        pos++;
      }
      pos = 0;
      string if_s = "if";
      while ((pos = word.find(if_s, pos)) != string::npos)
      {
        num_of_if++;
        pos++;
      }
      pos = 0;
      string block_s = "{";
      while ((pos = word.find(block_s, pos)) != string::npos)
      {
        num_of_blocks++;
        pos++;
      }
      pos = 0;
      string asterisks_s = "*";
      while ((pos = word.find(asterisks_s, pos)) != string::npos)
      {
        num_of_asterisks++;
        pos++;
      }
      pos = 0;
      string ampersands_s = "&";
      while ((pos = word.find(ampersands_s, pos)) != string::npos)
      {
        num_of_ampersands++;
        pos++;
      }
      pos = 0;
      string commas_s = ",";
      while ((pos = word.find(commas_s, pos)) != string::npos)
      {
        num_of_commas++;
        pos++;
      }
      pos = 0;
      string new_s = "new";
      while ((pos = word.find(new_s, pos)) != string::npos)
      {
        use_new = true;
        num_of_new++;
        pos++;
      }
      pos = 0;
      string friend_s = "friend";
      while ((pos = word.find(friend_s, pos)) != string::npos)
      {
        use_friend = true;
        num_of_friend++;
        pos++;
      }
      pos = 0;
      string static_s = "static";
      while ((pos = word.find(static_s, pos)) != string::npos)
      {
        use_static = true;
        num_of_static++;
        pos++;
      }
      pos = 0;
      string argc_s = "argc";
      while ((pos = word.find(argc_s, pos)) != string::npos)
      {
        use_argc = true;
        pos++;
        num_of_argc++;
      }
      //cout<< word << "\tsize: "<<size<<endl;
      content.push_back(word);
    }
    file.close();
  }
  else
  {
    cout << "Invalid file: " << file_name << endl;
    accepted_file = false;
  }
}

//replace strings in "" and '' with blankspace
string cppfile_measure::fix_quotation(string str)
{
  bool between_quotation = false;
  char quotation1 = '\"';
  char quotation2 = '\'';
  int size = str.size();
  for (int i = 0; i < size; i++)
  {
    if (str[i] == quotation1)
    {
      //first "
      if (between_quotation == false)
      {
        between_quotation = true;
        continue;
      }
      //second "
      else
      {
        between_quotation = false;
      }
    }
    if (between_quotation)
    {
      str[i] = ' ';
    }
  }

  for (int i = 0; i < size; i++)
  {
    if (str[i] == quotation2)
    {
      if (between_quotation == false)
      {
        between_quotation = true;
        continue;
      }
      else
      {
        between_quotation = false;
      }
    }
    if (between_quotation)
    {
      str[i] = ' ';
    }
  }
  return str;
}

//remove space at the start and end of the line
string cppfile_measure::fix_space(string str)
{
  if (str.empty())
  {
    return str;
  }
  str.erase(0, str.find_first_not_of(" "));
  str.erase(str.find_last_not_of(" ") + 1);
  return str;
}

//replace comments after // with blankspace
string cppfile_measure::remove_comment1(string comment1)
{
  string comment1_str = "//";
  int size_start = comment1.find(comment1_str) + 2;
  int size_end = comment1.size();

  for (int i = size_start; i < size_end; i++)
  {
    comment1[i] = ' ';
  }
  return comment1;
}

//replace comments between "/* */" with blankspace
string cppfile_measure::remove_comment2(string comment2)
{
  string comment1_str = "/*";
  string comment2_str = "*/";
  int size_start;
  if (comment2.find(comment1_str) != string::npos)
  {
    size_start = comment2.find(comment1_str) + 2;
  }
  else
  {
    size_start = 0;
  }
  int size_end;
  if (comment2.find(comment2_str) != string::npos)
  {
    size_end = comment2.find(comment2_str);
  }
  else
  {
    size_end = comment2.size();
  }

  for (int i = size_start; i < size_end; i++)
  {
    comment2[i] = ' ';
  }
  return comment2;
}

int main(int argc, char *argv[])
{

  cout << "Enter the number of cpp files to compare (no more than 15): ";
  int file_number = 0;
  cin >> file_number;
  while (file_number < 2 || file_number > 15)
  {
    cout << "Illegal number of files. Please enter a number between 2 and 15: ";
    cin >> file_number;
  }
  vector<string> files;
  for (int i = 0; i < file_number; i++)
  {
    cout << "Please enter the name of the file: ";
    string file_name;
    cin >> file_name;
    files.push_back(file_name);
  }

  FileCompare filecmp = FileCompare();

  vector<cppfile_measure> file_features;
  for (int i = 0; i < files.size(); i++)
  {
    cppfile_measure cpp = cppfile_measure();
    cpp.read_a_cppfile(files[i]);
    file_features.push_back(cpp);
  }

  cout << "Similarity matrix:" << endl;
  for (int i = 0; i < file_number; i++)
  {
    for (int j = 0; j < file_number; j++)
    {
      if (file_features[i].accepted_file && file_features[j].accepted_file)
      {
        vector<int> result1 = file_features[i].get_result_array();
        vector<int> result2 = file_features[j].get_result_array();
        //cout << filecmp.cosine_similarity(result1, result2) << "\t";
        cout << filecmp.equal_weight_similarity(result1, result2) << "\t";
      }
      else
      {
        cout << "-"
             << "\t";
      }
    }
    cout << endl;
  }
}