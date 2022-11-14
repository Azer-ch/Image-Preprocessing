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

vector<int> normalization(vector<int> &cumulatedHistogram, int maxVal, int lx, int ly) {
    vector<int> n(maxVal + 1, 0);
    for (int i = 0; i <= maxVal; i++) {
        n[i] = (cumulatedHistogram[i] * maxVal) / (lx * ly);
    }
    return n;
}

vector<int> equalizedHistogram(vector<int> &n, vector<int> &histogram, int maxVal) {
    vector<int> ret = vector<int>(maxVal + 1);
    ret[n[0]] = histogram[0];
    for (int i = 1; i <= maxVal; i++) {
        if (n[i] == n[i - 1]) {
            ret[n[i]] += histogram[i];
        } else {
            ret[n[i]] = histogram[i];
        }
    }
    return ret;
}

vector<vector<string>> equalizeImage(vector<vector<string>> image, int lx, int ly, int maxVal) {
    vector<int> hist = histogram(image, lx, ly, maxVal);
    vector<int> cumulatedHist = cumulatedHistogram(hist, maxVal);
    vector<int> n = normalization(cumulatedHist, maxVal, lx, ly);
    vector<vector<string>> retImage(lx, vector<string>(ly));
    for (int c = 0; c < lx; c++) {
        for (int r = 0; r < ly; r++) {
            int pixel = stoi(image[c][r]);
            pixel = n[pixel];
            retImage[c][r] = to_string(pixel);
        }
    }
    return retImage;
}

struct line {
    line(pair<int, int> p1, pair<int, int> p2) {
        this->pt1 = p1;
        this->pt2 = p2;
        calculateProperties();
    }

    pair<int, int> pt1;
    pair<int, int> pt2;
    double a, b;

    void calculateProperties() {
        this->a = (pt2.second - pt1.second) / (pt2.first - pt1.first);
        this->b = pt1.second - a * pt1.first;
    }

    int transform(double level) {
        return int(level * a + b);
    }
};

vector<vector<string>>
filter(pair<int, int> x, pair<int, int> y, vector<vector<string>> &image, int lx, int ly, int maxVal) {
    vector<vector<string>> ret(lx, vector<string>(ly));
    line d1 = line({0, 0}, x);
    line d2 = line(x, y);
    line d3 = line(y, {maxVal, maxVal});
    for (int c = 0; c < lx; c++) {
        for (int r = 0; r < ly; r++) {
            int pixel = stoi(image[c][r]);
            if (pixel <= min(x.first, y.first)) {
                ret[c][r] = to_string(d1.transform(pixel));
            } else if (pixel <= max(x.first, y.first)) {
                ret[c][r] = to_string(d2.transform(pixel));
            } else {
                ret[c][r] = to_string(d3.transform(pixel));
            }
        }
    }
    return ret;
}

void addBruit(vector<vector<string>> &image, int lx, int ly) {
    int randomNumber;
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            randomNumber = rand() % 21;
            if (randomNumber == 0)
                image[i][j] = "0";
            else if (randomNumber == 20)
                image[i][j] = "255";
        }
    }
}

vector<vector<string>> aggregateFilter(vector<vector<string>> image, int lx, int ly, int n) {
    vector<vector<string>> ret(lx, vector<string>(ly));
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            if (i - n / 2 >= 0 && i + n / 2 < lx && j - n / 2 >= 0 && j + n / 2 < ly) {
                double m = 0;
                for (int k = i - n / 2; k <= i + n / 2; k++) {
                    for (int z = j - n / 2; z <= j + n / 2; z++) {
                        m += stoi(image[k][z]);
                    }
                }
                m /= (n * n);
                ret[i][j] = to_string((int) m);
            } else
                ret[i][j] = image[i][j];
        }
    }
    return ret;
}

vector<vector<string>> medianFilter(vector<vector<string>> image, int lx, int ly, int n) {
    vector<vector<string>> ret(lx, vector<string>(ly));
    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            if (i - n / 2 >= 0 && i + n / 2 < lx && j - n / 2 >= 0 && j + n / 2 < ly) {
                vector<int> pxs = {};
                for (int k = i - n / 2; k <= i + n / 2; k++) {
                    for (int z = j - n / 2; z <= j + n / 2; z++) {
                        pxs.push_back(stoi(image[k][z]));
                    }
                }
                std::sort(pxs.begin(), pxs.end());
                ret[i][j] = to_string(pxs[(n * n) / 2]);
            } else
                ret[i][j] = image[i][j];
        }
    }
    return ret;
}
vector<vector<string>>

int main() {
    int lx, ly;
    Image image;
    vector<vector<string>> v = readImage("chat.pgm", lx, ly, image);
//    writeImagePgm("matrice", v, lx, ly, image);
//    pair<double, double> p = calcMeanEcartType(v, lx, ly);
//    cout << "average" << " ecart type" << endl;
//    cout << p.first << " " << p.second << endl;
//    vector<int> h = histogram(v, lx, ly, image.maxValue);
//    vector<int> hc = cumulatedHistogram(h, image.maxValue);
//    cout << "Histogram: " << endl;
//    for (int x: h)
//        cout << x << " ";
//    cout << endl;
//    cout << "Cumulated Histogram: " << endl;
//    for (int x: hc)
//        cout << x << " ";
//    cout << endl;
//    vector<vector<string>> eq = equalizeImage(v, lx, ly, image.maxValue);
//    writeImagePgm("equalized", eq, lx, ly, image);
//    vector<int> n = normalization(hc, image.maxValue, lx, ly);
//    vector<int> eqHistogram = equalizedHistogram(n, h, image.maxValue);
//    cout << "Normalization\n";
//    for (int x: n) {
//        cout << x << " ";
//    }
//    cout << endl;
//    cout << "Equalized Histogram\n";
//    for (int x: eqHistogram) {
//        cout << x << " ";
//    }
//    cout << endl;
//    vector<vector<string>> filteredImage = filter({50, 100}, {150, 200}, v, lx, ly, image.maxValue);
//    writeImagePgm("filter", filteredImage, lx, ly, image);
    writeImagePgm("duplicate", v, lx, ly, image);
    addBruit(v, lx, ly);
    vector<vector<string>> agg = aggregateFilter(v, lx, ly, 3);
    vector<vector<string>> median = medianFilter(v, lx, ly, 3);
    writeImagePgm("median", median, lx, ly, image);
    writeImagePgm("aggregate", agg, lx, ly, image);
    vector<vector<string>> test {
            {"1", "1", "1", "1", "1"},
            {"1", "2", "3", "4", "1"},
            {"1", "10", "12", "13", "1"},
            {"1", "5", "6", "5", "1"},
            {"1", "1", "1", "1", "1"}
    };
    writeImagePgm("test", test, 5, 5, image );
    writeImagePgm("testmoy",aggregateFilter(test,5,5,3),5,5,image);
}