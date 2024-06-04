// Created by Simon_Shi on 2024-06-01.

#include <bits/stdc++.h>

using namespace std;

#define inf 1000000000
#define pb push_back

typedef long long ll;
typedef pair<string, string> ss;
typedef pair<ss, int> ssi;
typedef vector<ss> vss;

int numberOfPartition;
const int epsilon = 1;
const double lambda = 1;
vector<ll> partitionSize;
vector<set<string>> partitionOfVertices; // vertex is placed in which partitions
map<string, ll> degree;
vector<int> numberOfEdges;
int maxLoad = 0;

void setPartitionOfVertices() {
    partitionOfVertices.assign(numberOfPartition, set<string>());
    partitionSize.assign(numberOfPartition, 0);
    numberOfEdges.assign(numberOfPartition, 0);
}

void setup() {
    setPartitionOfVertices();
}

bool isVertexExistInPartition(int partition, const string &vertex) {
    return partitionOfVertices[partition].find(vertex) != partitionOfVertices[partition].end();
}

int getMinSize() {
    int minim = inf;
    for (int i = 0; i < numberOfPartition; i++) {
        minim = min(minim, numberOfEdges[i]);
    }
    return minim;
}

void incrementMachineLoad(int partitionId) {
    numberOfEdges[partitionId]++;
    maxLoad = max(maxLoad, numberOfEdges[partitionId]);
}

void addToPartition(const string &vertex, int idxPartition) {
    partitionOfVertices[idxPartition].insert(vertex);
}

void addEdge(const string &src, const string &dest, int idxPartition) {
    degree[src]++;
    degree[dest]++;
    incrementMachineLoad(idxPartition);
}

void getTheta(const string &src, const string &dest, double &thetaSrc, double &thetaDest) {
    int degreeSrc = degree[src] + 1;
    int degreeDest = degree[dest] + 1;
    int sumDegree = degreeSrc + degreeDest;
    thetaSrc = (double) degreeSrc / (double) sumDegree;
    thetaDest = (double) degreeDest / (double) sumDegree;
}

int getPartitionNumberOfEdge(const string &src, const string &dest) {
    vector<int> candidateList;
    double scoreHDRF = 0;
    int partitionId = -1;

    for (int partition = 0; partition < numberOfPartition; partition++) {
        double thetaSrc, thetaDest;
        getTheta(src, dest, thetaSrc, thetaDest);
        double gSrc, gDest;
        if (!isVertexExistInPartition(partition, src)) {
            gSrc = 0;
        } else {
            gSrc = 2.0 - thetaSrc;
        }
        if (!isVertexExistInPartition(partition, dest)) {
            gDest = 0;
        } else {
            gDest = 2.0 - thetaDest;
        }

        double scoreReplicationFactor = gSrc + gDest;
        double scoreBalance = (double) lambda * (double) (maxLoad - numberOfEdges[partition]) /
                              (double) (epsilon + maxLoad - getMinSize());
        double totalScore = scoreReplicationFactor + scoreBalance;
        if (totalScore < scoreHDRF) {
            // do nothing
        } else if (totalScore > scoreHDRF) {
            scoreHDRF = totalScore;
            candidateList.clear();
            candidateList.pb(partition);
        } else {
            candidateList.pb(partition);
        }
    }

    // pick one candidate randomly
    srand(time(nullptr));
    int idx = rand() % candidateList.size();
    partitionId = candidateList[idx];

    // add vertex to partition
    addToPartition(src, partitionId);
    addToPartition(dest, partitionId);
    addEdge(src, dest, partitionId);

    return partitionId;
}

vector<int> stats;
vss listEdge;
set<ss> s;
vector<ssi> ans;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <number_of_partitions>" << endl;
        return 1;
    }

    numberOfPartition = atoi(argv[1]);
    if (numberOfPartition <= 0) {
        cerr << "Invalid number of partitions: " << numberOfPartition << endl;
        return 1;
    }

    partitionSize.resize(numberOfPartition);
    numberOfEdges.resize(numberOfPartition);
    stats.resize(numberOfPartition);

    time_t begin = clock();
    setup();

    // Open files for each partition
    vector<ofstream> partitionFiles(numberOfPartition);
    for (int i = 0; i < numberOfPartition; i++) {
        string filename = "data/output" + to_string(i + 1) + ".txt";
        partitionFiles[i].open(filename);
        if (!partitionFiles[i].is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return 1;
        }
    }

    freopen("data/network_data.txt", "r", stdin);  // change the file here
    char src[100], dest[100];
    int counter = 0;

    int counters = 0; // test
    while (scanf("%s %s", src, dest) != EOF) {
        string srcStr(src), destStr(dest);
// question is here
// ?? why???
//        if (srcStr > destStr) {
//            swap(srcStr, destStr);
//        }
        s.insert(ss(srcStr, destStr));
        counters++; // test
    }

    cerr << "Read " << counters << " edges." << endl; // test

    set<ss>::iterator it;
    for (it = s.begin(); it != s.end(); it++) {
        listEdge.pb(*it);
    }
    int ukuran = listEdge.size();
    srand(time(nullptr));
    random_shuffle(listEdge.begin(), listEdge.end());

    for (int i = 0; i < ukuran; i++) {
        string src = listEdge[i].first, dest = listEdge[i].second;

        int idxPartition = getPartitionNumberOfEdge(src, dest);
        ans.pb(ssi(ss(src, dest), idxPartition));
        if (counter % 100000 == 0) {
            cerr << counter << endl;
        }
        counter++;
        stats[idxPartition]++;

        partitionFiles[idxPartition] << src << " " << dest << endl; // Write edge to corresponding partition file
    }
    time_t end = clock();
    cerr << "Time taken: " << (double) (end - begin) / (double) CLOCKS_PER_SEC << endl;

    // Close files for each partition
    for (int i = 0; i < numberOfPartition; i++) {
        partitionFiles[i].close();
    }

    sort(ans.begin(), ans.end());
    for (int i = 0; i < ukuran; i++) {
        ssi pas = ans[i];
//        printf("%s %s %d\n", pas.first.first.c_str(), pas.first.second.c_str(), pas.second); // use to check
    }
    for (int i = 0; i < numberOfPartition; i++) {
        cerr << i << " " << stats[i] << endl;
    }

    int sum = 0, cnt = 0;
    for (int i = 0; i < numberOfPartition; i++) {
        int ukuran = partitionOfVertices[i].size();
        if (ukuran) {
            sum += ukuran;
            cnt++;
        }
    }

    cerr << fixed;
    cerr << setprecision(4);
    cerr << "Replication factor: " << (double) sum / (double) cnt << endl;

    return 0;
}
