#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <fstream>

void Graph::loadFile(
    const std::string& gName,
    std::vector<std::vector<int>> &data
)
{
    std::ifstream fhandle(gName.c_str());
    if (!fhandle.is_open()) {
        HERE;
        std::cout << "Failed to open " << gName << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(fhandle, line)) {
        std::istringstream iss(line);
        data.push_back(
            std::vector<int>(std::istream_iterator<int>(iss),
                             std::istream_iterator<int>())
        );
    }
    fhandle.close();

    std::cout << "Graph " << gName << " is loaded." << std::endl;
}

int Graph::getMaxIdx(const std::vector<std::vector<int>> &data) {
    int maxIdx = data[0][0];
    for (auto it1 = data.begin(); it1 != data.end(); it1++) {
        for (auto it2 = it1->begin(); it2 != it1->end(); it2++) {
            if (maxIdx <= (*it2)) {
                maxIdx = *it2;
            }
        }
    }
    return maxIdx;
}

int Graph::getMinIdx(const std::vector<std::vector<int>> &data) {
    int minIdx = data[0][0];
    for (auto it1 = data.begin(); it1 != data.end(); it1++) {
        for (auto it2 = it1->begin(); it2 != it1->end(); it2++) {
            if (minIdx >= (*it2)) {
                minIdx = *it2;
            }
        }
    }
    return minIdx;
}

std::vector<std::vector<int>> Graph::getReverseEL(std::vector<std::vector<int>> ori) {
    std::vector<std::vector<int>> res;
    res.assign(ori.begin(), ori.end());
    for (auto edge = res.begin(); edge != res.end(); edge++) {
        int tmp=(*edge)[0];
        (*edge)[0] = (*edge)[1];
        (*edge)[1] = tmp;
    }
    return res;
}


Graph::Graph(const std::string& gName) {

    // Check if it is undirectional graph
    auto found = gName.find("ungraph", 0);
    if (found != std::string::npos)
        isUgraph = true;
    else
        isUgraph = false;

    std::vector<std::vector<int>> data;
    loadFile(gName, data);
    vertexNum = getMaxIdx(data) + 1;
    edgeNum = (int)data.size();
    std::cout << "vertex num: " << vertexNum << std::endl;
    std::cout << "edge num: " << edgeNum << std::endl;

    elo.assign(data.begin(), data.end());
    //print2dvector(elo);
    eli = getReverseEL(elo);
    
    for (int i = 0; i < vertexNum; i++) {
        Vertex* v = new Vertex(i);
        vertices.push_back(v);
    }

    for (auto it = data.begin(); it != data.end(); it++) {
        int srcIdx = (*it)[0];
        int dstIdx = (*it)[1];
        vertices[srcIdx]->outVid.push_back(dstIdx);
        vertices[dstIdx]->inVid.push_back(srcIdx);
        if (isUgraph && srcIdx != dstIdx) {
            vertices[dstIdx]->outVid.push_back(srcIdx);
            vertices[srcIdx]->inVid.push_back(dstIdx);
        }
    }

    for (auto it = vertices.begin(); it != vertices.end(); it++) {
        (*it)->inDeg = (int)(*it)->inVid.size();
        (*it)->outDeg = (int)(*it)->outVid.size();
    }
}

CSR::CSR(const Graph &g) : vertexNum(g.vertexNum), edgeNum(g.edgeNum), elo(g.elo), eli(g.eli), isWeighted(g.isWeighted) {
    rpao.resize(vertexNum + 1);
    rpai.resize(vertexNum + 1);
    rpao[0] = 0;
    rpai[0] = 0;
    for (int i = 0; i < vertexNum; i++) {
        rpao[i + 1] = rpao[i] + g.vertices[i]->outDeg;
        rpai[i + 1] = rpai[i] + g.vertices[i]->inDeg;
    }

    sort(elo.begin(), elo.end(), [](std::vector<int> &a, std::vector<int> &b){
        return (a[0] == b[0]) ? a[1] < b[1] : a[0] < b[0]; 
    });

    sort(eli.begin(), eli.end(), [](std::vector<int> &a, std::vector<int> &b){
        return (a[0] == b[0]) ? a[1] < b[1] : a[0] < b[0]; 
    });

    // sort the input and output vertex
    for (auto edge : elo) {
        ciao.push_back(edge[1]);
        if (isWeighted) {
            ePropso.push_back(edge[2]);
        }
    }
    for (auto edge : eli) {
        ciai.push_back(edge[1]);
        if (isWeighted) {
            ePropsi.push_back(edge[2]);
        }
    }
}

int CSR::save2File(const std::string &fName)
{
    std::ostringstream command;
    command << "mkdir -p ";
    command <<"csr/";
    command << fName;
    int ret = system(command.str().c_str());
    if (ret < 0)
    {
        return ret;
    }
    std::ostringstream offsetsName, columnsName;
    offsetsName << "csr/";
    offsetsName << fName;
    offsetsName << "/csr_offsets.txt";
    std::ofstream offset;
    offset.open(offsetsName.str().c_str());

    columnsName << "csr/";
    columnsName << fName;
    columnsName << "/csr_columns.txt";
    std::ofstream columns;
    columns.open(columnsName.str().c_str());

    offset << (rpao.size() - 1) << std::endl;
    for (auto item : rpao)
    {
        offset << item << std::endl;
    }
    offset.flush();
    offset.close();

    columns << ciao.size() << std::endl;
    for (auto item : ciao)
    {
        columns << item << std::endl;
    }
    columns.flush();
    columns.close();

    return 0;


}
