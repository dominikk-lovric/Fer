#include <iostream>
#include <fstream>
using namespace std;

int main()
{
   string fileName;
   string lines;

   cin >> fileName;
   ifstream MyReadFile(fileName);
   getline(MyReadFile, lines);
   int num = stoi(lines);

   // cout << num << "\n";
   int matrica[num][num];
   for (int i = 0; i < num; i++)
   {
      getline(MyReadFile, lines);
      for (int j = 0; j < num; j++)
      {
         matrica[i][j] = lines[j] - '0';
      }
   }
   /*for (int i = 0; i < num; i++)
   {
      for (int j = 0; j < num; j++)
      {
         cout << matrica[i][j];
      }
      cout << "\n";
   }*/
   int rez = 0;
   for (int i = 0; i < num; i++)
   {
      for (int j = i + 1; j < num; j++)
      {
         if (matrica[i][j] == 1)
         {
            for (int k = j + 1; k < num; k++)
            {
               if (matrica[i][k] == 1 && matrica[j][k] == 1)
               {
                  rez++;
               }
            }
         }
      }
   }
   cout << rez;
}