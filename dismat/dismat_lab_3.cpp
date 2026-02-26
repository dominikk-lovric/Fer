#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

struct Edge
{
   int v1, v2, color;
   Edge(int v1, int v2) : v1(v1), v2(v2), color(-1) {}
};

bool backtrack(vector<Edge> &edges, int currEdgeId, int delta, int nodes, bool usedColors[][100])
{
   if (currEdgeId == edges.size())
      return true;

   int v1 = edges[currEdgeId].v1;
   int v2 = edges[currEdgeId].v2;

   for (int color = 1; color <= delta; color++)
   {
      if (!usedColors[v1][color] && !usedColors[v2][color])
      {
         usedColors[v1][color] = true;
         usedColors[v2][color] = true;
         edges[currEdgeId].color = color;

         if (backtrack(edges, currEdgeId + 1, delta, nodes, usedColors))
         {
            return true;
         }

         usedColors[v1][color] = false;
         usedColors[v2][color] = false;
         edges[currEdgeId].color = -1;
      }
   }
   return false;
}

int colorEdges(vector<Edge> &edges, int nodes, int delta)
{
   if (edges.empty())
      return 0;

   bool used[100][100];
   for (int i = 0; i < 100; i++)
      for (int j = 0; j < 100; j++)
         used[i][j] = false;

   if (backtrack(edges, 0, delta, nodes, used))
   {
      return delta;
   }

   for (int i = 0; i < 100; i++)
      for (int j = 0; j < 100; j++)
         used[i][j] = false;

   backtrack(edges, 0, delta + 1, nodes, used);
   return delta + 1;
}

int main()
{
   string filename;
   cin >> filename;

   ifstream file(filename);
   if (!file)
   {
      cout << "Datoteka se nije otvorila." << endl;
      return 1;
   }

   int nodes;
   file >> nodes;
   int graph[100][100] = {0};
   int maxDelta = 0;

   for (int i = 0; i < nodes; i++)
   {
      string line;
      file >> line;

      int currDelta = 0;
      for (int j = 0; j < nodes; j++)
      {
         if (line[j] == '1')
         {
            graph[i][j] = 1;
            currDelta++;
         }
      }

      // TRAŽENJE DELTE
      if (currDelta > maxDelta)
      {
         maxDelta = currDelta;
      }
   }

   file.close();

   for (size_t i = 0; i < nodes; i++)
   {
      for (size_t j = 0; j < nodes; j++)
      {
         cout << graph[i][j];
      }
      cout << "\n";
   }

   vector<Edge> edgeList;
   for (int i = 0; i < nodes; i++)
   {
      for (int j = 0; j < nodes; j++)
      {
         if (graph[i][j] == 1)
         {
            edgeList.push_back(Edge(i, j));
            graph[i][j] = 0;
            graph[j][i] = 0;
         }
      }
   }

   int maxColor = colorEdges(edgeList, nodes, maxDelta);

   cout << maxDelta << endl;
   // cout << maxColor << endl;
   cout << (maxColor == maxDelta) << endl;

   return 0;
}