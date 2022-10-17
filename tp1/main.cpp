#include<bits/stdc++.h>

using namespace std;

class Image {
public :
    string format;
    string comment;
    int nbL{}, nbC{}, maxValue{};

    Image() = default;
};

void splitToInt(string s, int &lx, int &ly) {
    int i = 0;
    while (s[i] != ' ' && i < (int) s.size())
        i++;
    string x = s.substr(0, i);
    string y = s.substr(i + 1);
    lx = stoi(x);
    ly = stoi(y);
}

vector<vector<string>> readImage(string name, int &lx, int &ly, Image &image) {
    ifstream file;
    file.open(name);
    getline(file, image.format);
    getline(file, image.comment);
    string size;
    getline(file, size);
    splitToInt(size, lx, ly);
    image.nbC = ly;
    image.nbL = lx;
    file >> image.maxValue;
    vector<vector<string>> v(lx, vector<string>(ly));
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            file >> v[i][j];
        }
    }
    return v;
}

void writeImagePgm(string name, vector<vector<string>> v, int lx, int ly, Image &image) {
    ofstream outTxt;
    ofstream outPgm;
    outTxt.open(name + ".txt");
    outPgm.open(name + ".pgm");
    if (!outTxt) {
        cerr << "Error : file could not be opened" << endl;
        return;
    }
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            outTxt << v[i][j] << " ";
        }
        outTxt << endl;
    }
    if (!outTxt) {
        cerr << "Error : file could not be opened" << endl;
        return;
    }
    outPgm << image.format << endl;
    outPgm << image.comment << endl;
    outPgm << to_string(lx) << " " << to_string(ly) << endl;
    outPgm << image.maxValue << endl;
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            outPgm << v[i][j] << " ";
        }
        outPgm << endl;
    }
}

pair<double, double> calcMeanEcartType(vector<vector<string>> v, int lx, int ly) {
    double sum = 0;
    double x;
    double p = lx * ly;
    pair<double, double> ret;
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            sum += stoi(v[i][j]);
        }
    }
    double m = sum / p;
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            x += (stoi(v[i][j]) - m) * (stoi(v[i][j]) - m);
        }
    }
    ret.first = m;
    ret.second = sqrt(x / p);
    return ret;
}

vector<int> histogram(vector<vector<string>> v, int lx, int ly, int maxVal) {
    vector<int> ret(maxVal + 1, 0);
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            ret[stoi(v[i][j])]++;
        }
    }
    return ret;
}

vector<int> cumulatedHistogram(vector<int> v, int maxVal) {
    vector<int> ret(maxVal + 1, 0);
    ret[0] = v[0];
    for (int i = 1; i <= maxVal; i++) {
        ret[i] = ret[i - 1] + v[i];
    }
    return ret;
}

int main() {
    int lx, ly;
    Image image;
    vector<vector<string>> v = readImage("chat.pgm", lx, ly, image);
    writeImagePgm("matrice", v, lx, ly, image);
    pair<double, double> p = calcMeanEcartType(v, lx, ly);
    cout <<"moyenne "<<" ecart type"<<endl;
    cout << p.first << " " << p.second << endl;
    vector<int> h = histogram(v, lx, ly, image.maxValue);
    vector<int> hc = cumulatedHistogram(h, image.maxValue);
    cout <<"Histogramme: "<<endl;
    for (int x: h)
        cout << x << " ";
    cout << endl;
    cout <<"Histogramme Cumule: "<<endl;
    for (int x: hc)
        cout << x << " ";
    cout << endl;
}